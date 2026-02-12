#pragma once
#include "icommand.hpp"

class PingCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.empty()) return Response{ResponseType::SIMPLE_STRING, "PONG"};
        return Response{ResponseType::BULK_STRING, request.args[0]};
    }
};
