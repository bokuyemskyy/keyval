#pragma once
#include "icommand.hpp"

class EchoCommand : public ICommand
{
public:
    Response execute(const Request &request, Session &session, Storage &storage) override final
    {
        if (request.args.size() != 1)
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'ECHO'"};
        return Response{ResponseType::BULK_STRING, request.args[0]};
    }
};