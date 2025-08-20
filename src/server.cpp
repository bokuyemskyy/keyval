#include "server.hpp"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <array>

void Server::start()
{
    int listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0); // creating a socket, getting its file descriptor

    // check
    if (listen_socket_fd < 0)
    {
        perror("socket failed");
        return;
    }

    // allowed rebinding to the same port quickly
    int opt = 1;
    setsockopt(listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in listen_address{};                             // structure for storing the server address
    listen_address.sin_family = AF_INET;                      // ipv4
    listen_address.sin_addr.s_addr = INADDR_ANY;              // all interfaces
    listen_address.sin_port = htons(m_config.getInt("port")); // port host to network

    // binding the listen address to the socket + check
    if (bind(listen_socket_fd, reinterpret_cast<sockaddr *>(&listen_address), sizeof(listen_address)) < 0)
    {
        perror("bind failed");
        close(listen_socket_fd);
        return;
    }

    // I put the socket into listening state (maximum queue size) + check
    if (listen(listen_socket_fd, SOMAXCONN) < 0)
    {
        perror("listen failed");
        close(listen_socket_fd);
        return;
    }

    int epoll_fd = epoll_create1(0); // creating the epoll
    // check
    if (epoll_fd < 0)
    {
        perror("epoll_create1 failed");
        return;
    }

    epoll_event event{}; // the event I have to add to the epoll watch
    // wait for the listen_socket_fd to be readable (pending connection)
    event.events = EPOLLIN;
    event.data.fd = listen_socket_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_socket_fd, &event); // add to the epoll

    constexpr int MAX_EVENTS = 10;
    epoll_event events[MAX_EVENTS]; // a buffer for a batch of MAX_EVENTS events

    Logger::log(LogLevel::INFO, "Ready to accept TCP connections.");

    m_running = true;
    while (m_running)
    {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1); // wait for an event, return <= MAX_EVENTS at a time
        // check
        if (n < 0)
        {
            if (errno == EINTR)
                continue;
            perror("epoll_wait failed");
            break;
        }

        for (int i = 0; i < n; ++i) // for each new event
        {
            if (events[i].data.fd == listen_socket_fd) // listen event => new connection
            {
                sockaddr_in client_address{};
                socklen_t client_len = sizeof(client_address);
                int client_socket_fd = accept(listen_socket_fd, reinterpret_cast<sockaddr *>(&client_address), &client_len);

                epoll_event client_ev{};
                client_ev.events = EPOLLIN;
                client_ev.data.fd = client_socket_fd; // now adding the client_socket_fd to the poll
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket_fd, &client_ev);
            }
            else // client event => new data
            {
                int client_socket_fd = events[i].data.fd;
                std::array<char, 1024> buffer;
                int bytes = recv(client_socket_fd, buffer.data(), buffer.size(), 0);

                if (bytes <= 0) // error or a client sent EOF
                {
                    close(client_socket_fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket_fd, nullptr); // remove from poll
                }
                else // process the request
                {
                    std::string request(buffer.data(), bytes);
                    std::string response = m_protocol.handle(request); // protocol is responsible
                    send(client_socket_fd, response.c_str(), response.size(), 0);
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