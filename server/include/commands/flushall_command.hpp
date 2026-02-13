#pragma once
#include "icommand.hpp"

class FlushAllCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (!request.m_args.empty())
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'FLUSHALL'"};

        storage.flushall();

        return Response{.m_type = ResponseType::SIMPLE_STRING, .m_value = "OK"};
    }
};