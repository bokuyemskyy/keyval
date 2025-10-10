#pragma once
#include "icommand.hpp"

class RenameCommand : public ICommand
{
public:
    Response execute(const Request &request, Session &session, Storage &storage) override final
    {
        if (request.args.size() != 2)
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'RENAME'"};

        const auto &old_key = request.args[0];
        const auto &new_key = request.args[1];

        if (!storage.exists(session.db(), old_key))
            return Response{ResponseType::ERROR, "ERR no such key '" + old_key + "'"};

        storage.rename(session.db(), old_key, new_key);
        return Response{ResponseType::SIMPLE_STRING, "OK"};
    }
};