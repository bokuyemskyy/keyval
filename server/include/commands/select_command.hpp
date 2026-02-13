#pragma once
#include "icommand.hpp"

class SelectCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 2) {
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'SELECT'"};
        }

        int db_index = std::stoi(request.m_args[1]);
        session.setDb(db_index);

        return Response{.m_type = ResponseType::SIMPLE_STRING, .m_value = "OK"};
    }
};
