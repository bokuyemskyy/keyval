#pragma once
#include "icommand.hpp"

class SMembersCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.size() != 1) {
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'smembers' command"};
        }

        const std::string&              key     = request.args[0];
        std::unordered_set<std::string> members = storage.smembers(session.db(), key);

        auto responses = std::vector<std::shared_ptr<Response>>{};
        responses.reserve(members.size());

        for (const auto& member : members) {
            responses.push_back(std::make_shared<Response>(Response{ResponseType::BULK_STRING, member}));
        }

        return Response{ResponseType::ARRAY, responses};
    }
};
