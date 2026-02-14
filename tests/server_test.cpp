#include "command_handler.hpp"
#include "command_registry.hpp"
#include "config.hpp"
#include "server.hpp"
#include "socket.hpp"
#include "storage.hpp"

#include <string>
#include <thread>
#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

std::string sendCommand(const std::string& command) {
    Socket client;
    client.create();
    client.connect("127.0.0.1", 6380);
    client.send(command + "\r\n");

    std::string response;
    client.recv(response, 4096);
    client.close();
    return response;
}

TEST_CASE("Server: Integration tests") {
    Config         config;
    Storage        storage;
    CommandHandler handler(storage);
    registerCommands(handler);
    Server server(config, handler);

    std::thread server_thread([&]() { server.start(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    SECTION("Server commands") {
        REQUIRE(sendCommand("PING").find("PONG") != std::string::npos);
        REQUIRE(sendCommand("ECHO hello").find("hello") != std::string::npos);
    }

    SECTION("Key management") {
        sendCommand("SET key1 value1");
        REQUIRE(sendCommand("EXISTS key1").find("1") != std::string::npos);
        REQUIRE(sendCommand("DEL key1").find("1") != std::string::npos);
        REQUIRE(sendCommand("TYPE key1").find("none") != std::string::npos);
        REQUIRE(sendCommand("KEYS *").length() > 0);

        sendCommand("SET expkey val");
        REQUIRE(sendCommand("EXPIRE expkey 1").find("1") != std::string::npos);
        REQUIRE(sendCommand("TTL expkey").find("1") == std::string::npos);
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
        REQUIRE(sendCommand("TTL expkey").find("-2") == std::string::npos);

        sendCommand("SET oldkey val");
        REQUIRE(sendCommand("RENAME oldkey newkey").find("OK") != std::string::npos);

        REQUIRE(sendCommand("FLUSHDB").find("OK") != std::string::npos);
    }

    SECTION("String commands") {
        REQUIRE(sendCommand("SET key val").find("OK") != std::string::npos);
        REQUIRE(sendCommand("GET key").find("val") != std::string::npos);

        sendCommand("SET num 10");
        REQUIRE(sendCommand("INCR num").find("11") != std::string::npos);
        REQUIRE(sendCommand("DECR num").find("10") != std::string::npos);

        sendCommand("SET str Hello");
        REQUIRE(sendCommand("APPEND str World").find("10") != std::string::npos);
    }

    SECTION("List commands") {
        REQUIRE(sendCommand("LPUSH list val1").find("1") != std::string::npos);
        REQUIRE(sendCommand("RPUSH list val2").find("2") != std::string::npos);
        REQUIRE(sendCommand("LPOP list").find("val1") != std::string::npos);
        REQUIRE(sendCommand("RPOP list").find("val2") != std::string::npos);

        sendCommand("RPUSH mylist a");
        sendCommand("RPUSH mylist b");
        sendCommand("RPUSH mylist c");
        REQUIRE(sendCommand("LRANGE mylist 0 -1").find("a") != std::string::npos);
        REQUIRE(sendCommand("LLEN mylist").find("3") != std::string::npos);
    }

    SECTION("Set commands") {
        REQUIRE(sendCommand("SADD set m1").find("1") != std::string::npos);
        REQUIRE(sendCommand("SADD set m2 m3").find("2") != std::string::npos);
        REQUIRE(sendCommand("SREM set m1").find("1") != std::string::npos);
        REQUIRE(sendCommand("SISMEMBER set m2").find("1") != std::string::npos);
        REQUIRE(sendCommand("SMEMBERS set").find("m2") != std::string::npos);
        REQUIRE(sendCommand("SCARD set").find("2") != std::string::npos);
    }

    server.stop();
    server_thread.join();
}
