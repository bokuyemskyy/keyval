#include "command_handler.hpp"
#include <algorithm>

CommandHandler::CommandHandler(Storage &db) : storage(db) {}

std::string CommandHandler::handle(const Command &cmd, Session &session)
{
    if (cmd.name == "PING")
        return handlePing(cmd, session);
    if (cmd.name == "ECHO")
        return handleEcho(cmd, session);
    if (cmd.name == "SET")
        return handleSet(cmd, session);
    if (cmd.name == "GET")
        return handleGet(cmd, session);
    if (cmd.name == "EXISTS")
        return handleExists(cmd, session);

    return "-ERR unknown command\r\n";
}

std::string CommandHandler::handlePing(const Command &cmd, Session &session)
{
    return "+PONG\r\n";
}

std::string CommandHandler::handleEcho(const Command &cmd, Session &session)
{
    if (cmd.args.empty())
        return "-ERR wrong number of arguments for 'ECHO'\r\n";
    return "+" + cmd.args[0] + "\r\n";
}

std::string CommandHandler::handleSet(const Command &cmd, Session &session)
{
    if (cmd.args.size() < 2)
        return "-ERR wrong number of arguments for 'SET'\r\n";
    storage.set(session.db(), cmd.args[0], cmd.args[1]);
    return "+OK\r\n";
}

std::string CommandHandler::handleGet(const Command &cmd, Session &session)
{
    if (cmd.args.size() < 1)
        return "-ERR wrong number of arguments for 'GET'\r\n";
    auto value = storage.get(session.db(), cmd.args[0]);
    if (!value.has_value())
        return "$-1\r\n";
    return "$" + std::to_string(value->size()) + "\r\n" + *value + "\r\n";
}

std::string CommandHandler::handleExists(const Command &cmd, Session &session)
{
    if (cmd.args.empty())
        return "-ERR wrong number of arguments for 'EXISTS'\r\n";
    return ":" + std::to_string(storage.exists(session.db(), cmd.args[0])) + "\r\n";
}
