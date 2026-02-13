#pragma once
#include "icommand.hpp"

class DelCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 1)
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'DEL'"};

        int result = static_cast<int>(storage.del(session.db(), request.m_args[0]));

        return Response{.m_type = ResponseType::INTEGER, .m_value = result};
    }
};
