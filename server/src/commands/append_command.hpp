#pragma once
#include "icommand.hpp"

class AppendCommand : public ICommand
{
public:
    Response execute(const Request &request, Session &session, Storage &storage) override final
    {
        if (request.args.size() != 2)
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'APPEND'"};

        const std::string &key = request.args[0];
        const std::string &value = request.args[1];
        storage.append(session.db(), key, value);
        return Response{ResponseType::SIMPLE_STRING, "OK"};
    }
};