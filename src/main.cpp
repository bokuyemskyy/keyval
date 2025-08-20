#include "database.hpp"
#include "protocol.hpp"
#include "server.hpp"
#include "config.hpp"
#include "app_info.hpp"

#include <csignal>

Server *g_server = nullptr;

void handle_sigint(int)
{
  Logger::log(LogLevel::INFO, "Received SIGINT. Shutting down.");
  if (g_server)
    g_server->stop();
}
int main(int argc, char **argv)
{
  try
  {
    Logger::log(LogLevel::INFO, std::format("{} is starting", AppInfo::Name));

    Config config(argc, argv);
    Database database;
    Protocol protocol(database);
    Server server(config, protocol);

    g_server = &server;
    std::signal(SIGINT, handle_sigint);

    printBanner(config);

    server.start();
  }
  catch (const std::exception &e)
  {
    Logger::log(LogLevel::ERROR, std::string("Fatal error: ") + e.what());
    return 1;
  }
  return 0;
}
