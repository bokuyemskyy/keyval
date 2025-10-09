#pragma once

#include <string>
#include <iostream>
#include <ctime>

enum class LogLevel
{
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger
{
public:
    static void log(LogLevel level, const std::string &message)
    {
        std::time_t t = std::time(nullptr);
        std::string timeString = std::ctime(&t);
        if (timeString.ends_with("\n"))
        {
            timeString.pop_back();
        }
        std::cout << "[" << timeString << "] ";
        switch (level)
        {
        case LogLevel::DEBUG:
            std::cout << "[DEBUG] ";
            break;
        case LogLevel::INFO:
            std::cout << "[INFO] ";
            break;
        case LogLevel::WARN:
            std::cerr << "[WARN] ";
            break;
        case LogLevel::ERROR:
            std::cerr << "[ERROR] ";
            break;
        }
        std::cout << message << "\n";
    }
};
