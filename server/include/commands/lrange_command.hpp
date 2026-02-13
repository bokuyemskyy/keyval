#pragma once
#include "icommand.hpp"

class LRangeCommand : public ICommand {
   public:
    Response execute(const Request& request, Session& session, Storage& storage) override final {
        if (request.args.size() != 3) {
            return Response{ResponseType::ERR, "ERR wrong number of arguments for 'lrange' command"};
        }

        const std::string& key   = request.args[0];
        int                start = std::stoi(request.args[1]);
        int                stop  = std::stoi(request.args[2]);

        std::vector<std::string> elements = storage.lrange(session.db(), key, start, stop);
        if (elements.empty()) {
            return Response{ResponseType::ERR, "ERR could not perform lrange operation"};
        }

        auto responses = std::vector<std::shared_ptr<Response>>{};
        responses.reserve(elements.size());

        for (const auto& element : elements) {
            responses.push_back(std::make_shared<Response>(Response{ResponseType::BULK_STRING, element}));
        }

        return Response{ResponseType::ARRAY, responses};
    }
};
