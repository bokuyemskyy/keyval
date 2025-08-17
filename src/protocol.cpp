#include <sstream>

#include "protocol.hpp"
std::string Protocol::handle(const std::string &request)
{
    std::istringstream iss(request);
    std::string command;
    iss >> command;

    if (command == "SET")
    {
        std::string key, value;
        iss >> key >> value;
        if (key.empty() || value.empty())
        {
            return "ERR wrong number of arguments for 'SET' command\n";
        }
        m_database.set(key, value);
        return "OK\n";
    }

    if (command == "GET")
    {
        std::string key;
        iss >> key;
        if (key.empty())
            return "ERR wrong number of arguments for 'GET' command\n";

        auto value = m_database.get(key);
        if (value.has_value())
        {
            return *value;
        }
        else
        {
            return "(nil)\n";
        }
    }

    if (command == "DEL")
    {
        std::string key;
        iss >> key;
        if (key.empty())
            return "ERR wrong number of arguments for 'DEL' command\n";

        auto result = m_database.del(key);
        if (result)
        {
            return "1\n";
        }
        else
        {
            return "0\n";
        }
    }

    if (command == "PING")
    {
        return "PONG\n";
    }

    if (command == "KEYS")
    {
        std::string pattern;
        iss >> pattern;
        if (pattern != "*")
            return "ERR only '*' pattern is supported\n";

        if (m_database.empty())
            return "(empty list)\n";

        return m_database.keys();
    }

    return "ERR unknown command\n";
}