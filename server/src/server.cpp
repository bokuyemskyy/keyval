#include "server.hpp"

#include "event_poll.hpp"
#include "protocol.hpp"
#include "socket.hpp"

void Server::addConnection(Socket&& client_socket) {
    int fd = client_socket.fd();

    m_connections.emplace(fd, Connection(std::move(client_socket)));
    Logger::log(LogLevel::DEBUG, "Connection added: fd=" + std::to_string(fd));
}

void Server::removeConnection(int fd) {
    auto it = m_connections.find(fd);
    if (it != m_connections.end()) {
        m_connections.erase(it);
        Logger::log(LogLevel::DEBUG, "Connection removed: fd=" + std::to_string(fd));
    }
}

Connection& Server::getConnection(int fd) {
    return m_connections.at(fd);
}

bool Server::hasConnection(int fd) const {
    return m_connections.find(fd) != m_connections.end();
}

Socket& Server::getSocket(int fd) {
    return m_connections.at(fd).socket;
}
Session& Server::getSession(int fd) {
    return m_connections.at(fd).session;
}

void Server::start() {
    Socket listen_socket;

    listen_socket.create();
    listen_socket.setReuseAddr(true);
    listen_socket.setNonBlocking(true);
    listen_socket.bind("0.0.0.0", m_config.getInt("port"));
    listen_socket.listen();

    EventPoll poll;
    poll.addFd(listen_socket.fd(), PollEvent::READ);

    Logger::log(LogLevel::INFO, "Ready to accept TCP connections.");
    m_running = true;

    while (m_running) {
        poll.wait(500);

        for (const auto& ev : poll.events()) {
            if (ev.m_fd == listen_socket.fd()) // new connection
            {
                try {
                    Socket client_socket = listen_socket.accept();

                    client_socket.setNonBlocking(true);

                    int client_fd = client_socket.fd();
                    poll.addFd(client_fd, PollEvent::READ);

                    addConnection(std::move(client_socket)); // track connection

                    Logger::log(LogLevel::INFO, "New client connected: fd=" + std::to_string(client_fd));
                } catch (const std::exception& e) {
                    Logger::log(LogLevel::ERR, std::string("Accept failed: ") + e.what());
                }
            } else // existing connection
            {
                try {
                    handleClient(ev.m_fd, poll);
                } catch (const std::exception& e) {
                    Logger::log(LogLevel::ERR, std::string("Error handling client: ") + e.what());
                    cleanupClient(ev.m_fd, poll);
                }
            }
        }
    }

    Logger::log(LogLevel::INFO, "Shutting down server...");

    listen_socket.close();

    for (const auto& [fd, conn_data] : m_connections) {
        poll.removeFd(fd);
        // socket destructor will close the fd
    }
    m_connections.clear();

    Logger::log(LogLevel::INFO, "Server stopped.");
}

void Server::stop() {
    m_running = false;
}

void Server::handleClient(int fd, EventPoll& poll) {
    Socket& sock = getSocket(fd);

    std::string   request_raw;
    socket_size_t bytes = sock.recv(request_raw);

    if (bytes <= 0) {
        if (bytes == 0) {
            Logger::log(LogLevel::INFO, "Client disconnected: fd=" + std::to_string(fd));
        }
        cleanupClient(fd, poll);
        return;
    }

    Request request;
    try {
        request = Protocol::deserializeRequest(request_raw);
    } catch (const std::exception& e) {
        Logger::log(LogLevel::WARN, "Protocol error from fd=" + std::to_string(fd) + ": " + e.what());

        std::string error_response =
            Protocol::serializeResponse(Response{ResponseType::ERR, std::string("ERR ") + e.what()});
        sock.send(error_response);

        cleanupClient(fd, poll);
        return;
    }

    Response response = m_handler.handle(request, getSession(fd));

    if (response.m_type == ResponseType::SHUTDOWN) {
        Logger::log(LogLevel::INFO, "Shutdown requested by client fd=" + std::to_string(fd));
        stop();
        return;
    }

    std::string response_raw = Protocol::serializeResponse(response);
    sock.send(response_raw);
}

void Server::cleanupClient(int fd, EventPoll& poll) {
    poll.removeFd(fd);
    removeConnection(fd);
}