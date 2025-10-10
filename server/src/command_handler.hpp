#pragma once
#include "commands/icommand.hpp"
#include "common/protocol.hpp"
#include "storage.hpp"
#include <string>
#include "session.hpp"

class CommandHandler
{

public:
    explicit CommandHandler(Storage &storage);
    Response handle(const Request &request, Session &session);
    void registerCommand(const std::string &command_name, std::unique_ptr<ICommand> command)
    {
        m_commands[command_name] = std::move(command);
    }

private:
    Storage &m_storage;

    std::unordered_map<std::string, std::unique_ptr<ICommand>> m_commands;
};
