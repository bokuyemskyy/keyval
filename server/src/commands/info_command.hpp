#pragma once
#include "icommand.hpp"

class InfoCommand : public ICommand
{
public:
    Response execute(const Request &request, Session &session, Storage &storage) override final
    {
        if (request.args.size() != 0)
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'INFO'"};

        size_t db_index = session.db();
        if (!storage.exists(db_index))
            return Response{ResponseType::ERROR, "ERR invalid DB index"};

        size_t key_count = storage.dbsize(db_index);
        std::string info = "# Storage Info\n";
        info += "db_index:" + std::to_string(db_index) + "\n";
        info += "keys:" + std::to_string(key_count) + "\n";
        return Response{ResponseType::BULK_STRING, info};
    }
};