#pragma once

#include "protocol.hpp"
#include "session.hpp"
#include "storage.hpp"

#include <memory.h>

class ICommand {
  public:
    virtual ~ICommand() = default;

    ICommand(const ICommand&)            = delete;
    ICommand& operator=(const ICommand&) = delete;
    ICommand(ICommand&&)                 = delete;
    ICommand& operator=(ICommand&&)      = delete;

    virtual Response execute(const Request& request, Session& session, Storage& storage) = 0;

  protected:
    ICommand() = default;
};
