#pragma once
#include "icommand.hpp"

class TypeCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 1)
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'TYPE'"};

        const std::string& key  = request.m_args[0];
        const std::string& type = storage.type(session.db(), key);

        return Response{.m_type = ResponseType::SIMPLE_STRING, .m_value = type};
    }
};
