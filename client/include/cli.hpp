#pragma once

#include "command_registry.hpp"
#include "socket.hpp"

#include <replxx.hxx>
#include <string>

class Cli {
  public:
    Cli(const std::string& host, uint16_t port);
    void run();

  private:
    void        setupCompletion();
    void        setupHints();
    void        handleLine(const std::string& line);
    std::string formatResponse(const std::string& resp);

    std::string     m_host;
    int             m_port;
    Socket          m_socket;
    replxx::Replxx  m_repl;
    CommandRegistry m_registry;
};