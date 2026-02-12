#pragma once

#include "protocol.hpp"
#include "session.hpp"
#include "storage.hpp"

class ICommand {
   public:
    virtual ~ICommand()                                                                  = default;
    virtual Response execute(const Request& request, Session& session, Storage& storage) = 0;
};
