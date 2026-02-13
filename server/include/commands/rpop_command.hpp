#pragma once
#include "icommand.hpp"

class RPopCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 1) {
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'rpop' command"};
        }

        const std::string& key = request.m_args[0];
        std::string        value;

        if (!storage.rpop(session.db(), key, value)) {
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR could not perform rpop operation"};
        }

        return Response{.m_type = ResponseType::SIMPLE_STRING, .m_value = value};
    }
};
