#pragma once
#include "icommand.hpp"

class TypeCommand : public ICommand
{
public:
    Response execute(const Request &request, Session &session, Storage &storage) override final
    {
        if (request.args.size() != 1)
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'TYPE'"};

        const std::string &key = request.args[0];
        const std::string &type = storage.type(session.db(), key);
        return Response{ResponseType::SIMPLE_STRING, type};
    }
};
