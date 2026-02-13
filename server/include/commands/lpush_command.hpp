#pragma once
#include "icommand.hpp"

class LPushCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.size() != 2) {
            return Response{ResponseType::ERR, "ERR wrong number of arguments for 'lpush' command"};
        }

        const std::string& key   = request.args[0];
        const std::string& value = request.args[1];

        if (!storage.lpush(session.db(), key, value)) {
            return Response{ResponseType::ERR, "ERR could not perform lpush operation"};
        }

        int length = storage.llen(session.db(), key);
        return Response{ResponseType::INTEGER, length};
    }
};
