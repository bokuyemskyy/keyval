#pragma once
#include "icommand.hpp"

class SAddCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.size() < 2) {
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'sadd' command"};
        }

        const std::string&       key = request.args[0];
        std::vector<std::string> members(request.args.begin() + 1, request.args.end());

        for (const auto& member : members) {
            if (!storage.sadd(session.db(), key, member)) {
                return Response{ResponseType::ERROR, "ERR could not perform sadd operation"};
            }
        }

        int added = members.size();
        return Response{ResponseType::INTEGER, added};
    }
};
