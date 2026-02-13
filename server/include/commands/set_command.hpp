#pragma once
#include "icommand.hpp"

class SetCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.size() != 2) return Response{ResponseType::ERR, "ERR wrong number of arguments for 'SET'"};

        const auto& key   = request.args[0];
        const auto& value = request.args[1];

        storage.set(session.db(), key, value);
        return Response{ResponseType::SIMPLE_STRING, "OK"};
    }
};
