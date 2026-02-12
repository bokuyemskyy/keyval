#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cstring>
#include <stdexcept>
#include <string>

class Socket {
    int m_fd;

   public:
    Socket() : m_fd(-1) {}
    explicit Socket(int fd) : m_fd(fd) {}
    ~Socket() { close(); }

    void create() {
        m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (m_fd < 0) throw std::runtime_error("socket failed");
    }

    void setReuseAddr(bool enable = true) const {
        int opt = enable ? 1 : 0;
        if (::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
    }

    void setNonBlocking(bool enable = true) const {
        int flags = fcntl(m_fd, F_GETFL, 0);
        if (flags == -1) throw std::runtime_error("fcntl(F_GETFL) failed");
        if (enable)
            flags |= O_NONBLOCK;
        else
            flags &= ~O_NONBLOCK;
        if (fcntl(m_fd, F_SETFL, flags) == -1) throw std::runtime_error("fcntl(F_SETFL) failed");
    }

    void bind(uint16_t port) const {
        sockaddr_in addr{};
        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port        = htons(port);
        if (::bind(m_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) throw std::runtime_error("bind failed");
    }

    void listen(int backlog = SOMAXCONN) const {
        if (::listen(m_fd, backlog) < 0) throw std::runtime_error("listen failed");
    }

    Socket accept() const {
        int client_fd = ::accept(m_fd, nullptr, nullptr);
        if (client_fd < 0) throw std::runtime_error("accept failed");
        return Socket(client_fd);
    }

    void connect(const std::string& host, uint16_t port) const {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port);
        if (::inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) throw std::runtime_error("invalid address");
        if (::connect(m_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
            throw std::runtime_error("connect failed");
    }

    ssize_t recv(std::string& out) const {
        std::array<char, 1024> buf;
        ssize_t                bytes = ::recv(m_fd, buf.data(), buf.size(), 0);
        if (bytes > 0) out.assign(buf.data(), bytes);
        return bytes;
    }

    void send(const std::string& data) const { ::send(m_fd, data.c_str(), data.size(), 0); }

    void close() {
        if (m_fd != -1) {
            ::close(m_fd);
            m_fd = -1;
        }
    }

    int fd() const { return m_fd; }
};
