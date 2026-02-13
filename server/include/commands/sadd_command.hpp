#pragma once
#include "icommand.hpp"

class SAddCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() < 2) {
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'sadd' command"};
        }

        const std::string&       key = request.m_args[0];
        std::vector<std::string> members(request.m_args.begin() + 1, request.m_args.end());

        for (const auto& member : members) {
            if (!storage.sadd(session.db(), key, member)) {
                return Response{.m_type = ResponseType::ERR, .m_value = "ERR could not perform sadd operation"};
            }
        }

        int added = members.size();

        return Response{.m_type = ResponseType::INTEGER, .m_value = added};
    }
};
