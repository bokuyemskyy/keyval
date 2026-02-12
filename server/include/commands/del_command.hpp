#pragma once
#include "icommand.hpp"

class DelCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.size() != 1) return Response{ResponseType::ERROR, "ERR wrong number of arguments for 'DEL'"};
        int result = storage.del(session.db(), request.args[0]);
        return Response{ResponseType::INTEGER, result};
    }
};
