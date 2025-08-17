#include "server.hpp"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <array>

void Server::start()
{
    int listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0); // creating a socket, getting its file descriptor
    int opt = 1;
    setsockopt(listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (listen_socket_fd < 0)
    {
        perror("socket failed");
        return;
    }

    sockaddr_in listen_address{};                // structure for storing the adress
    listen_address.sin_family = AF_INET;         // ipv4
    listen_address.sin_addr.s_addr = INADDR_ANY; // all interfaces
    listen_address.sin_port = htons(m_port);     // port

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

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
    {
        perror("epoll_create1 failed");
        return;
    }

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = listen_socket_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_socket_fd, &ev);

    constexpr int MAX_EVENTS = 10;
    epoll_event events[MAX_EVENTS];

    while (true)
    {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (n < 0)
        {
            perror("epoll_wait failed");
            break;
        }

        for (int i = 0; i < n; ++i)
        {
            if (events[i].data.fd == listen_socket_fd)
            {
                sockaddr_in client_address{};
                socklen_t client_len = sizeof(client_address);
                int client_fd = accept(listen_socket_fd, reinterpret_cast<sockaddr *>(&client_address), &client_len);

                epoll_event client_ev{};
                client_ev.events = EPOLLIN;
                client_ev.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);
            }
            else
            {
                int client_fd = events[i].data.fd;
                std::array<char, 1024> buffer;
                int bytes = recv(client_fd, buffer.data(), buffer.size(), 0);

                if (bytes <= 0)
                {
                    close(client_fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
                }
                else
                {
                    std::string request(buffer.data(), bytes);
                    std::string response = m_protocol.handle(request);
                    send(client_fd, response.c_str(), response.size(), 0);
                }
            }
        }
    }

    close(listen_socket_fd);
}
