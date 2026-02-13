#pragma once

#include "command_handler.hpp"
#include "config.hpp"
#include "event_poll.hpp"
#include "session.hpp"
#include "socket.hpp"

#include <algorithm>
#include <unordered_map>
struct Connection {
    Socket  socket;
    Session session{};

    explicit Connection(Socket&& socket) : socket(std::move(socket)) {}

    Connection(Connection&&)            = default;
    Connection& operator=(Connection&&) = default;

    Connection(const Connection&)            = delete;
    Connection& operator=(const Connection&) = delete;
};

class Server {
  public:
    explicit Server(Config& config, CommandHandler& handler) : m_config(config), m_handler(handler) {
        Logger::log(LogLevel::INFO, "Server initialized.");
    };

    void start();
    void stop();

  private:
    void        addConnection(Socket&& client_socket);
    void        removeConnection(int fd);
    Connection& getConnection(int fd);
    bool        hasConnection(int fd) const;

    void handleClient(int fd, EventPoll& poll);
    void cleanupClient(int fd, EventPoll& poll);

    Socket&  getSocket(int fd);
    Session& getSession(int fd);

    Config&         m_config;
    CommandHandler& m_handler;

    std::unordered_map<int, Connection> m_connections;

    bool m_running = false;
};