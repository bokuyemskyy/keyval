#pragma once
#include "common/protocol.hpp"
#include "storage.hpp"
#include <string>
#include "session.hpp"

class CommandHandler
{
    Storage &storage;

public:
    explicit CommandHandler(Storage &db);
    std::string handle(const Command &cmd, Session &session);

private:
    std::string handlePing(const Command &cmd, Session &session);
    std::string handleEcho(const Command &cmd, Session &session);
    std::string handleSet(const Command &cmd, Session &session);
    std::string handleGet(const Command &cmd, Session &session);
    std::string handleExists(const Command &cmd, Session &session);
};
