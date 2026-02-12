#pragma once
#include "icommand.hpp"

class SRemCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.size() < 2) {
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'srem' command"};
        }

        const std::string&       key = request.args[0];
        std::vector<std::string> members(request.args.begin() + 1, request.args.end());

        for (const auto& member : members) {
            if (!storage.srem(session.db(), key, member)) {
                return Response{ResponseType::ERROR, "ERR could not perform srem operation"};
            }
        }

        int removed = members.size();
        return Response{ResponseType::INTEGER, removed};
    }
};
