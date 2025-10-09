#include "common/protocol.hpp"
#include "app_info.hpp"
#include "config.hpp"
#include "storage.hpp"
#include "server.hpp"

#include <csignal>
#include <format>

Server *g_server = nullptr;

void handleSignal(int)
{
  Logger::log(LogLevel::INFO, "Received signal. Shutting down.");
  if (g_server)
    g_server->stop();
}

int main(int argc, char **argv)
{
  try
  {
    Logger::log(LogLevel::INFO, std::format("{} is starting", AppInfo::Name));

    Config config(argc, argv);
    Storage storage;
    CommandHandler handler(storage);
    Server server(config, handler);

    g_server = &server;
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

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
