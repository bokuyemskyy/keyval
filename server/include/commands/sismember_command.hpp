#pragma once
#include "icommand.hpp"

class SIsMemberCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 2) {
            return Response{.m_type  = ResponseType::ERR,
                            .m_value = "ERR wrong number of arguments for 'sismember' command"};
        }

        const std::string& key    = request.m_args[0];
        const std::string& member = request.m_args[1];

        bool exists = storage.sismember(session.db(), key, member);

        return Response{.m_type = ResponseType::INTEGER, .m_value = exists ? 1 : 0};
    }
};
