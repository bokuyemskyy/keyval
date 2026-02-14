#pragma once
#include "icommand.hpp"

class LRangeCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 3) {
            return Response{.m_type  = ResponseType::ERR,
                            .m_value = "ERR wrong number of arguments for 'lrange' command"};
        }

        const std::string& key   = request.m_args[0];
        int                start = std::stoi(request.m_args[1]);
        int                stop  = std::stoi(request.m_args[2]);

        std::vector<std::string> elements = storage.lrange(session.db(), key, start, stop);

        auto responses = std::vector<std::shared_ptr<Response>>{};
        responses.reserve(elements.size());

        for (const auto& element : elements) {
            responses.push_back(
                std::make_shared<Response>(Response{.m_type = ResponseType::BULK_STRING, .m_value = element}));
        }

        return Response{.m_type = ResponseType::ARRAY, .m_value = responses};
    }
};
