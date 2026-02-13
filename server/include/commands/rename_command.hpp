#pragma once
#include "icommand.hpp"

class RenameCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 2)
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR wrong number of arguments for 'RENAME'"};

        const auto& old_key = request.m_args[0];
        const auto& new_key = request.m_args[1];

        if (!storage.exists(session.db(), old_key))
            return Response{.m_type = ResponseType::ERR, .m_value = "ERR no such key '" + old_key + "'"};

        storage.rename(session.db(), old_key, new_key);

        return Response{.m_type = ResponseType::SIMPLE_STRING, .m_value = "OK"};
    }
};