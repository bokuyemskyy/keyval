#pragma once

#include "logger.hpp"

#include <assert.h>
#include <regex>
#include <string>

class Config {
  public:
    Config() { parseArgs(0, nullptr); };
    Config(int argc, char** argv) { parseArgs(argc, argv); }

    int         port = 6380;
    std::string host = "127.0.0.1";

  private:
    void parseArgs(int argc, char** argv) {
        if (argc > 0 && argv == nullptr) {
            Logger::log(LogLevel::WARN, "Invalid arguments: argv is null");
            argc = 0;
        }

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            std::string param;

            if (arg.starts_with("--")) {
                param = arg.substr(2);
            } else if (arg.starts_with("-")) {
                param = arg.substr(1);
                if (param.size() != 1) {
                    Logger::log(LogLevel::ERR, "Invalid parameter: " + param);
                    exit(1);
                }
            }

            if (i + 1 >= argc) {
                Logger::log(LogLevel::ERR, "No value specified for parameter " + param);
                exit(1);
            }

            std::string value = argv[++i];

            if (param == "p" || param == "port") {
                std::regex port_pattern(R"(^\d+$)");
                if (!std::regex_match(value, port_pattern)) {
                    Logger::log(LogLevel::ERR, "Invalid port specified: " + value);
                    exit(1);
                }
                port = std::stoi(value);

                if (port < 1 || port > 65535) {
                    Logger::log(LogLevel::ERR, "Invalid port specified: " + value);
                    exit(1);
                }
            } else if (param == "h" || param == "host") {
                std::regex ipv4_pattern(
                    R"(^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)");
                if (!std::regex_match(value, ipv4_pattern)) {
                    Logger::log(LogLevel::ERR, "Invalid host specified: " + value);
                    exit(1);
                }
                host = value;
            } else {
                Logger::log(LogLevel::ERR, "Invalid parameter " + param);
                exit(1);
            }
        }
    }
};