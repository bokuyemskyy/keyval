#include "command_registry.hpp"

CommandRegistry::CommandRegistry() {
    // String commands
    m_commands["PING"]   = {"PING", 0, 1, {"[message]"}};
    m_commands["ECHO"]   = {"ECHO", 1, 1, {"message"}};
    m_commands["SET"]    = {"SET", 2, 2, {"key", "value"}};
    m_commands["GET"]    = {"GET", 1, 1, {"key"}};
    m_commands["DEL"]    = {"DEL", 1, 1, {"key"}};
    m_commands["APPEND"] = {"APPEND", 2, 2, {"key", "value"}};
    m_commands["INCR"]   = {"INCR", 1, 1, {"key"}};
    m_commands["DECR"]   = {"DECR", 1, 1, {"key"}};
    m_commands["EXISTS"] = {"EXISTS", 1, 1, {"key"}};
    m_commands["RENAME"] = {"RENAME", 2, 2, {"oldkey", "newkey"}};
    m_commands["TYPE"]   = {"TYPE", 1, 1, {"key"}};

    // Expiration commands
    m_commands["EXPIRE"] = {"EXPIRE", 2, 2, {"key", "seconds"}};
    m_commands["TTL"]    = {"TTL", 1, 1, {"key"}};

    // List commands
    m_commands["LPUSH"]  = {"LPUSH", 2, 2, {"key", "value"}};
    m_commands["RPUSH"]  = {"RPUSH", 2, 2, {"key", "value"}};
    m_commands["LPOP"]   = {"LPOP", 1, 1, {"key"}};
    m_commands["RPOP"]   = {"RPOP", 1, 1, {"key"}};
    m_commands["LLEN"]   = {"LLEN", 1, 1, {"key"}};
    m_commands["LRANGE"] = {"LRANGE", 3, 3, {"key", "start", "stop"}};

    // Set commands
    m_commands["SADD"]      = {"SADD", 2, -1, {"key", "member", "[member ...]"}};
    m_commands["SREM"]      = {"SREM", 2, -1, {"key", "member", "[member ...]"}};
    m_commands["SCARD"]     = {"SCARD", 1, 1, {"key"}};
    m_commands["SMEMBERS"]  = {"SMEMBERS", 1, 1, {"key"}};
    m_commands["SISMEMBER"] = {"SISMEMBER", 2, 2, {"key", "member"}};

    // Database commands
    m_commands["SELECT"]   = {"SELECT", 2, 2, {"db", "index"}};
    m_commands["DBSIZE"]   = {"DBSIZE", 0, 0, {}};
    m_commands["FLUSHDB"]  = {"FLUSHDB", 0, 0, {}};
    m_commands["FLUSHALL"] = {"FLUSHALL", 0, 0, {}};
    m_commands["KEYS"]     = {"KEYS", 1, 1, {"pattern"}};
    m_commands["INFO"]     = {"INFO", 0, 0, {}};

    // Server commands
    m_commands["SHUTDOWN"] = {"SHUTDOWN", 0, 0, {}};
}

const CommandMeta* CommandRegistry::find(const std::string& name) const {
    auto it = m_commands.find(name);
    if (it == m_commands.end())
        return nullptr;
    return &it->second;
}

std::vector<std::string> CommandRegistry::commandNames() const {
    std::vector<std::string> out;
    for (auto const& [k, _] : m_commands)
        out.push_back(k);
    return out;
}