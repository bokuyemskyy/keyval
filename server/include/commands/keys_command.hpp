#pragma once
#include "icommand.hpp"

class KeysCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 1)
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'KEYS'"};

        auto keys      = storage.keys(session.db(), request.m_args[0]);
        auto responses = std::vector<std::shared_ptr<Response>>{};

        responses.reserve(keys.size());

        for (const auto& key : keys) {
            responses.push_back(
                std::make_shared<Response>(Response{.m_type = ResponseType::BULK_STRING, .m_value = key}));
        }

        return Response{.m_type = ResponseType::ARRAY, .m_value = responses};
    }
};
