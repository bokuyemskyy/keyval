#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

class Socket {
    int m_fd;

   public:
    Socket() : m_fd(-1) {}
    explicit Socket(int fd) : m_fd(fd) {}

    Socket(const Socket&)            = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept : m_fd(other.m_fd) { other.m_fd = -1; }
    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
            close();
            m_fd       = other.m_fd;
            other.m_fd = -1;
        }
        return *this;
    }
    ~Socket() { close(); }

    void create() {
        m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (m_fd < 0) throw std::runtime_error("socket creation failed");
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
        if (::bind(m_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
            throw std::runtime_error(std::string("bind failed: ") + strerror(errno));
    }

    void listen(int backlog = SOMAXCONN) const {
        if (::listen(m_fd, backlog) < 0) throw std::runtime_error(std::string("listen failed: ") + strerror(errno));
    }

    Socket accept() const {
        int client_fd = ::accept(m_fd, nullptr, nullptr);
        if (client_fd < 0) throw std::runtime_error(std::string("accept failed: ") + strerror(errno));
        return Socket(client_fd);
    }

    void connect(const std::string& host, uint16_t port) const {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port);
        if (::inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) throw std::runtime_error("invalid address");
        if (::connect(m_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
            throw std::runtime_error(std::string("connect failed: ") + strerror(errno));
    }

    ssize_t recv(void* buffer, size_t size) const {
        if (m_fd == -1) throw std::runtime_error("recv on invalid socket");

        ssize_t bytes = ::recv(m_fd, buffer, size, 0);
        if (bytes < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) return 0;  // not an error, just no data
            throw std::runtime_error(std::string("recv failed: ") + strerror(errno));
        }
        return bytes;
    }

    ssize_t recv(std::string& out, size_t max_size = 4096) const {
        std::vector<char> buffer(max_size);
        ssize_t           bytes = recv(buffer.data(), buffer.size());
        if (bytes > 0) {
            out.assign(buffer.data(), bytes);
        }
        return bytes;
    }
    ssize_t send(const void* data, size_t size) const {
        if (m_fd == -1) throw std::runtime_error("send on invalid socket");

        ssize_t sent = ::send(m_fd, data, size, 0);
        if (sent < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return 0;
            }
            throw std::runtime_error(std::string("send failed: ") + strerror(errno));
        }
        return sent;
    }
    ssize_t send(const std::string& data) const { return send(data.data(), data.size()); }

    void close() {
        if (m_fd != -1) {
            ::close(m_fd);
            m_fd = -1;
        }
    }

    int  fd() const { return m_fd; }
    bool valid() const { return m_fd != -1; }

    int release() {
        int fd = m_fd;
        m_fd   = -1;
        return fd;
    }
};
