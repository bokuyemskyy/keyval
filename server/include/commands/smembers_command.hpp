#pragma once
#include "icommand.hpp"

class SMembersCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 1) {
            return Response{.m_type  = ResponseType::ERR,
                            .m_value = "ERR wrong number of arguments for 'smembers' command"};
        }

        const std::string&              key     = request.m_args[0];
        std::unordered_set<std::string> members = storage.smembers(session.db(), key);

        auto responses = std::vector<std::shared_ptr<Response>>{};
        responses.reserve(members.size());

        for (const auto& member : members) {
            responses.push_back(
                std::make_shared<Response>(Response{.m_type = ResponseType::BULK_STRING, .m_value = member}));
        }

        return Response{ResponseType::ARRAY, responses};
    }
};
