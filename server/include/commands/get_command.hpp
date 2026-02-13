#pragma once
#include "icommand.hpp"

class GetCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 1)
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'GET'"};

        const std::string& key = request.m_args[0];

        auto value = storage.get(session.db(), key);

        if (!value.has_value())
            return Response{.m_type = ResponseType::NIL, .m_value = ""};

        return Response{.m_type = ResponseType::BULK_STRING, .m_value = *value};
    }
};
