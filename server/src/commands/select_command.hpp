#pragma once
#include "icommand.hpp"

class SelectCommand : public ICommand
{
public:
    Response execute(const Request &request, Session &session, Storage &storage) override final
    {
        if (request.args.size() != 2)
        {
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'SELECT'"};
        }

        int db_index = std::stoi(request.args[1]);
        session.setDb(db_index);

        return Response{ResponseType::SIMPLE_STRING, "OK"};
    }
};
