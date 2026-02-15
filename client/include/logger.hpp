#pragma once

#include <ctime>
#include <iostream>
#include <string>

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERR
};

class Logger {
  public:
    static void log(LogLevel level, const std::string& message) {
        std::time_t t           = std::time(nullptr);
        std::string time_string = std::ctime(&t);
        if (time_string.ends_with("\n")) {
            time_string.pop_back();
        }
        std::cout << "[" << time_string << "] ";
        switch (level) {
            case LogLevel::DEBUG:
                std::cout << "[DEBUG] ";
                break;
            case LogLevel::INFO:
                std::cout << "[INFO] ";
                break;
            case LogLevel::WARN:
                std::cerr << "[WARN] ";
                break;
            case LogLevel::ERR:
                std::cerr << "[ERROR] ";
                break;
        }
        std::cout << message << "\n";
    }
};
