#pragma once
#include "icommand.hpp"

class LPopCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.size() != 1) {
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'lpop' command"};
        }

        const std::string& key = request.args[0];
        std::string        value;

        if (!storage.lpop(session.db(), key, value)) {
            return Response{ResponseType::ERROR, "ERR could not perform lpop operation"};
        }

        return Response{ResponseType::SIMPLE_STRING, value};
    }
};
