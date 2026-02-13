#pragma once
#include "icommand.hpp"

class ShutdownCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 0)
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'SHUTDOWN'"};

        session.requestShutdown();

        return Response{.m_type = ResponseType::SHUTDOWN};
    }
};
