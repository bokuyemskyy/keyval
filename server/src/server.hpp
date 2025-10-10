#pragma once

#include <unordered_map>

#include "common/protocol.hpp"
#include "command_handler.hpp"
#include "config.hpp"
#include "session.hpp"

class Server
{
public:
    Server(Config &config, CommandHandler &handler) : m_config(config), m_handler(handler)
    {
        Logger::log(LogLevel::INFO, "Server initialized.");
    };
    void start();
    void stop();

    void addSession(int fd, const Session &session);
    void removeSession(int fd);
    Session &getSession(int fd);

private:
    Config &m_config;
    CommandHandler &m_handler;

    std::unordered_map<int, Session> m_sessions;
    mutable std::mutex m_mutex;

    bool m_running = false;
};