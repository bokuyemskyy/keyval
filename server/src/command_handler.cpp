#include "command_handler.hpp"
#include <algorithm>

CommandHandler::CommandHandler(Storage& storage) : m_storage(storage) {}

Response CommandHandler::handle(const Request& request, Session& session) {
    std::string command = request.command;
    std::transform(command.begin(), command.end(), command.begin(), [](unsigned char c) { return std::toupper(c); });

    if (m_commands.find(command) == m_commands.end())
        return Response{ResponseType::ERR, std::string("ERR unknown command '") + command + "'"};

    return m_commands[command]->execute(request, session, m_storage);
}