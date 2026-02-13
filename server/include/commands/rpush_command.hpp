#pragma once
#include "icommand.hpp"

class RPushCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 2) {
            return Response{.m_type  = ResponseType::ERR,
                            .m_value = "ERR wrong number of arguments for 'rpush' command"};
        }

        const std::string& key   = request.m_args[0];
        const std::string& value = request.m_args[1];

        if (!storage.rpush(session.db(), key, value)) {
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR could not perform rpush operation"};
        }

        int length = storage.llen(session.db(), key);

        return Response{.m_type = ResponseType::INTEGER, .m_value = length};
    }
};
