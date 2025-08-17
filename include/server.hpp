#pragma once

#include "protocol.hpp"

class Server
{
public:
    Server(Protocol &protocol, int port = 6380, int maxQueue = 1) : m_protocol(protocol), m_port(port), m_maxQueue(maxQueue) {};
    void start();

private:
    Protocol &m_protocol;

    int m_port;
    int m_maxQueue;
};