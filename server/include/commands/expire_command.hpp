#pragma once
#include "icommand.hpp"

class ExpireCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.size() != 2)
            return Response{ResponseType::ERR, "ERR wrong number of arguments for 'EXPIRE'"};
        const std::string& key = request.args[0];
        int                seconds;
        try {
            seconds = std::stoi(request.args[1]);
        } catch (const std::invalid_argument&) {
            return Response{ResponseType::ERR, "ERR value is not an integer or out of range"};
        }
        bool result = storage.expire(session.db(), key, seconds);
        return Response{ResponseType::INTEGER, std::to_string(result)};
    }
};
