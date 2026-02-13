#pragma once
#include "icommand.hpp"

class SCardCommand : public ICommand {
  public:
    Response execute(const Request& request, Session& session, Storage& storage) final {
        if (request.m_args.size() != 1) {
            return Response{.m_type  = ResponseType::ERR,
                            .m_value = "ERR wrong number of arguments for 'scard' command"};
        }

        const std::string& key  = request.m_args[0];
        int                card = storage.scard(session.db(), key);

        return Response{.m_type = ResponseType::INTEGER, .m_value = card};
    }
};
