#pragma once
#include "icommand.hpp"

class AppendCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 2)
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'APPEND'"};

        const std::string& key   = request.m_args[0];
        const std::string& value = request.m_args[1];

        int result = storage.append(session.db(), key, value);

        return Response{.m_type = ResponseType::INTEGER, .m_value = result};
    }
};