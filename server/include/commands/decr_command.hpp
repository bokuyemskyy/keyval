#pragma once
#include "icommand.hpp"

class DecrCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.size() != 1) return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'DECR'"};
        int result = storage.decr(session.db(), request.args[0]);
        return Response{ResponseType::INTEGER, result};
    }
};
