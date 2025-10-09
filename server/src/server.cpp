#include "server.hpp"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <array>
#include "event_poll.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <array>
#include <cstring>

void Server::addSession(int fd, const Session &session)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessions.emplace(fd, session);
}

void Server::removeSession(int fd)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessions.erase(fd);
}
bool Server::hasSession(int fd) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_sessions.find(fd) != m_sessions.end();
}

Session &Server::getSession(int fd)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_sessions.at(fd);
}

void Server::start()
{
    int listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket_fd < 0)
    {
        perror("socket failed");
        return;
    }

    int opt = 1;
    setsockopt(listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in listen_address{};
    listen_address.sin_family = AF_INET;
    listen_address.sin_addr.s_addr = INADDR_ANY;
    listen_address.sin_port = htons(m_config.getInt("port"));

    if (bind(listen_socket_fd, reinterpret_cast<sockaddr *>(&listen_address), sizeof(listen_address)) < 0)
    {
        perror("bind failed");
        close(listen_socket_fd);
        return;
    }

    if (listen(listen_socket_fd, SOMAXCONN) < 0)
    {
        perror("listen failed");
        close(listen_socket_fd);
        return;
    }

    EventPoll poll;
    poll.addFd(listen_socket_fd, EPOLLIN);

    Logger::log(LogLevel::INFO, "Ready to accept TCP connections.");

    m_running = true;
    while (m_running)
    {
        poll.waitEvents(-1);
        for (const auto &ev : poll.events())
        {
            if (ev.fd == listen_socket_fd) // new connection
            {
                sockaddr_in client_address{};
                socklen_t client_len = sizeof(client_address);
                int client_socket_fd = accept(listen_socket_fd, reinterpret_cast<sockaddr *>(&client_address), &client_len);

                if (client_socket_fd >= 0)
                {
                    poll.addFd(client_socket_fd, EPOLLIN | EPOLLET);

                    Session session(client_socket_fd);
                    addSession(client_socket_fd, session); // track session
                }
            }
            else // existing connection
            {

                std::array<char, 1024> buffer;
                int bytes = recv(ev.fd, buffer.data(), buffer.size(), 0);

                if (bytes <= 0) // disconnect
                {
                    poll.removeFd(ev.fd);
                    removeSession(ev.fd);
                    close(ev.fd);
                }
                else // got data
                {
                    std::string request(buffer.data(), bytes);

                    std::string response = m_handler.handle(Protocol::deserialize(request), getSession(ev.fd));

                    send(ev.fd, response.c_str(), response.size(), 0);
                }
            }
        }
    }

    close(listen_socket_fd);
}

void Server::stop()
{
    m_running = false;
}