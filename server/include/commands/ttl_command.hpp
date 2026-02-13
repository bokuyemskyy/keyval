#pragma once
#include "icommand.hpp"

class TTLCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.size() != 1) return Response{ResponseType::ERR, "ERR wrong number of arguments for 'TTL'"};
        const std::string& key = request.args[0];
        int                ttl = storage.ttl(session.db(), key);
        return Response{ResponseType::INTEGER, std::to_string(ttl)};
    }
};
