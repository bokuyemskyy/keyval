#pragma once
#include "icommand.hpp"

class ExistsCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 1)
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'EXISTS'"};

        bool result = storage.exists(session.db(), request.m_args[0]);

        return Response{.m_type = ResponseType::INTEGER, .m_value = static_cast<int>(result)};
    }
};
