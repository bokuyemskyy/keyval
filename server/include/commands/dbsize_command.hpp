#pragma once
#include "icommand.hpp"

class DbSizeCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (!request.m_args.empty())
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'DBSIZE'"};

        return Response{.m_type = ResponseType::INTEGER, .m_value = std::to_string(storage.dbsize(session.db()))};
    }
};