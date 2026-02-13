#pragma once
#include "icommand.hpp"

class DbSizeCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (!request.args.empty()) return Response{ResponseType::ERR, "ERR wrong number of arguments for 'DBSIZE'"};
        return Response{ResponseType::INTEGER, std::to_string(storage.dbsize(session.db()))};
    }
};