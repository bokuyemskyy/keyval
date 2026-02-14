#pragma once
#include "icommand.hpp"

class ExpireCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 2)
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'EXPIRE'"};

        const std::string& key     = request.m_args[0];
        int                seconds = 0;

        try {
            seconds = std::stoi(request.m_args[1]);
        } catch (const std::invalid_argument&) {
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR value is not an integer or out of range"};
        }

        bool result = storage.expire(session.db(), key, seconds);

        return Response{.m_type = ResponseType::INTEGER, .m_value = static_cast<int>(result)};
    }
};
