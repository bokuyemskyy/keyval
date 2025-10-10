#pragma once
#include "icommand.hpp"

class SIsMemberCommand : public ICommand
{
public:
    Response execute(const Request &request, Session &session, Storage &storage) override final
    {
        if (request.args.size() != 2)
        {
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'sismember' command"};
        }

        const std::string &key = request.args[0];
        const std::string &member = request.args[1];

        bool exists = storage.sismember(session.db(), key, member);
        return Response{ResponseType::INTEGER, exists ? 1 : 0};
    }
};
