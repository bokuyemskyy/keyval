#pragma once
#include "icommand.hpp"

class FlushDbCommand : public ICommand
{
public:
    Response execute(const Request &request, Session &session, Storage &storage) override final
    {
        if (!request.args.empty())
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'FLUSHDB'"};

        size_t db_index = session.db();
        if (!storage.exists(db_index))
            return Response{ResponseType::ERROR, "ERR invalid DB index"};

        storage.flushdb(db_index);
        return Response{ResponseType::SIMPLE_STRING, "OK"};
    }
};