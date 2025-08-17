#include "database.hpp"
#include "protocol.hpp"
#include "server.hpp"

int main()
{
  Database database;
  Protocol protocol(database);
  Server server(protocol, 6380, 10);

  server.start();
  return 0;
}