#pragma once
#include "icommand.hpp"

class SCardCommand : public ICommand
{
public:
    Response execute(const Request &request, Session &session, Storage &storage) override final
    {
        if (request.args.size() != 1)
        {
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'scard' command"};
        }

        const std::string &key = request.args[0];
        int card = storage.scard(session.db(), key);
        return Response{ResponseType::INTEGER, card};
    }
};
