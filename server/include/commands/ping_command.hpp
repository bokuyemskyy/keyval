#pragma once
#include "icommand.hpp"

class PingCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.empty())
            return Response{.m_type = ResponseType::SIMPLE_STRING, .m_value = "PONG"};

        return Response{.m_type = ResponseType::BULK_STRING, .m_value = request.m_args[0]};
    }
};
