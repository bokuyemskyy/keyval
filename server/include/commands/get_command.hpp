#pragma once
#include "icommand.hpp"

class GetCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.size() != 1) return Response{ResponseType::ERR, "ERR wrong number of arguments for 'GET'"};
        const std::string& key   = request.args[0];
        auto               value = storage.get(session.db(), key);
        if (!value.has_value()) return Response{ResponseType::NIL, ""};
        return Response{ResponseType::BULK_STRING, *value};
    }
};
