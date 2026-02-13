#pragma once
#include "icommand.hpp"

class KeysCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.size() != 1) return Response{ResponseType::ERR, "ERR wrong number of arguments for 'KEYS'"};

        auto keys      = storage.keys(session.db(), request.args[0]);
        auto responses = std::vector<std::shared_ptr<Response>>{};
        responses.reserve(keys.size());

        for (const auto& key : keys) {
            responses.push_back(std::make_shared<Response>(Response{ResponseType::BULK_STRING, key}));
        }

        return Response{ResponseType::ARRAY, responses};
    }
};
