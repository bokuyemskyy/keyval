#pragma once
#include "icommand.hpp"

class ShutdownCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.size() != 0)
            return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'SHUTDOWN'"};

        session.requestShutdown();
        return Response{ResponseType::SHUTDOWN};
    }
};
