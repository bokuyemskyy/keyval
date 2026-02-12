#include "server.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cstring>
#include <mutex>
#include <variant>

// #include "event_poll.hpp"
#include "protocol.hpp"
#include "socket.hpp"

void Server::addSession(int fd, const Session& session) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessions.emplace(fd, session);
}

void Server::removeSession(int fd) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessions.erase(fd);
}

Session& Server::getSession(int fd) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_sessions.at(fd);
}

void Server::start() {
    Socket listen_socket;

    listen_socket.create();
    listen_socket.setReuseAddr(true);
    listen_socket.setNonBlocking(true);
    listen_socket.bind(m_config.getInt("port"));
    listen_socket.listen();

    EventPoll poll;
    poll.addFd(listen_socket.fd(), EPOLLIN);

    Logger::log(LogLevel::INFO, "Ready to accept TCP connections.");

    m_running = true;
    while (m_running) {
        poll.waitEvents(-1);
        for (const auto& ev : poll.events()) {
            if (ev.fd == listen_socket.fd())  // new connection
            {
                sockaddr_in client_address{};
                socklen_t   client_len = sizeof(client_address);
                int         client_socket_fd =
                    accept(listen_socket.fd(), reinterpret_cast<sockaddr*>(&client_address), &client_len);

                if (client_socket_fd >= 0) {
                    poll.addFd(client_socket_fd, EPOLLIN);

                    Session session(client_socket_fd);
                    addSession(client_socket_fd, session);  // track session
                }
            } else  // existing connection
            {
                std::array<char, 1024> buffer;
                int                    bytes = recv(ev.fd, buffer.data(), buffer.size(), 0);

                if (bytes <= 0)  // disconnect
                {
                    poll.removeFd(ev.fd);
                    removeSession(ev.fd);
                    close(ev.fd);
                } else  // got data
                {
                    std::string request_raw(buffer.data(), bytes);

                    Request request;
                    try {
                        request = Protocol::deserializeRequest(request_raw);
                    } catch (const std::exception& e) {
                        std::string error_response =
                            Protocol::serializeResponse(Response{ResponseType::ERROR, std::string("ERR ") + e.what()});
                        send(ev.fd, error_response.c_str(), error_response.size(), 0);
                        continue;
                    }
                    Response response = m_handler.handle(request, getSession(ev.fd));
                    if (response.type == ResponseType::SHUTDOWN) {
                        Logger::log(LogLevel::INFO, "Shutting down the server.");
                        stop();
                        break;
                    } else {
                        std::string response_raw = Protocol::serializeResponse(response);
                        send(ev.fd, response_raw.c_str(), response_raw.size(), 0);
                    }
                }
            }
        }
    }

    listen_socket.close();
    for (const auto& session : m_sessions) {
        shutdown(session.first, SHUT_RDWR);
        close(session.first);
    }
    m_sessions.clear();
}

void Server::stop() { m_running = false; }