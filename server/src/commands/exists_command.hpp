#pragma once
#include "icommand.hpp"

class ExistsCommand : public ICommand
{
public:
    Response execute(const Request &request, Session &session, Storage &storage) override final
    {
        if (request.args.size() != 1)
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'EXISTS'"};
        bool result = storage.exists(session.db(), request.args[0]);
        return Response{ResponseType::INTEGER, std::to_string(result)};
    }
};
