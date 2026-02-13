#pragma once
#include "icommand.hpp"

class FlushDbCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (!request.m_args.empty())
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'FLUSHDB'"};

        size_t db_index = session.db();

        if (!storage.exists(db_index))
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR invalid DB index"};

        storage.flushdb(db_index);

        return Response{.m_type = ResponseType::SIMPLE_STRING, .m_value = "OK"};
    }
};