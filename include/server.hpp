#pragma once

#include "protocol.hpp"
#include "config.hpp"

class Server
{
public:
    Server(Config &config, Protocol &protocol) : m_protocol(protocol), m_config(config)
    {
        Logger::log(LogLevel::INFO, "Server initialized.");
    };
    void start();
    void stop();

private:
    Protocol &m_protocol;
    Config &m_config;

    bool m_running = false;
};