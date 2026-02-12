#pragma once
#include "icommand.hpp"

class FlushAllCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (!request.args.empty()) return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'FLUSHALL'"};

        storage.flushall();
        return Response{ResponseType::SIMPLE_STRING, "OK"};
    }
};