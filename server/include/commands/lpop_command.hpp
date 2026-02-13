#pragma once
#include "icommand.hpp"

class LPopCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 1) {
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'lpop' command"};
        }

        const std::string& key = request.m_args[0];
        std::string        value;

        if (!storage.lpop(session.db(), key, value)) {
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR could not perform lpop operation"};
        }

        return Response{.m_type = ResponseType::SIMPLE_STRING, .m_value = value};
    }
};
