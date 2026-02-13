#pragma once

#include <cstdint>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#include <BaseTsd.h>
typedef SSIZE_T socket_size_t;
typedef SOCKET socket_t;
#define INVALID_SOCKET_FD INVALID_SOCKET
#else
#include <sys/socket.h>
#include <sys/types.h>
typedef ssize_t socket_size_t;
typedef int socket_t;
#define INVALID_SOCKET_FD -1
#endif

class Socket {
public:
    Socket();
    explicit Socket(socket_t fd);
    ~Socket();

    Socket(Socket&&) noexcept;
    Socket& operator=(Socket&&) noexcept;

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    void create();
    void close();
    bool valid() const;
    socket_t  fd() const;
    int  release();

    void setReuseAddr(bool enable = true);
    void setNonBlocking(bool enable = true);
    
    void bind(uint16_t port);
    void listen(int backlog = SOMAXCONN);
    Socket accept();
    void connect(const std::string& host, uint16_t port);

    socket_size_t recv(void* buffer, size_t size);
    socket_size_t recv(std::string& out, size_t max_size = 4096);
    socket_size_t send(const void* data, size_t size);
    socket_size_t send(const std::string& data);

private:
    socket_t m_fd;
};
