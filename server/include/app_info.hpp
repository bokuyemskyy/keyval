#pragma once

#ifdef _WIN32
#include <process.h>
#define getpid() _getpid()
#else
#include <unistd.h>
#endif

#include "config.hpp"

#include <array>
#include <iostream>
#include <vector>

namespace AppInfo {
const std::string NAME    = "Keyval";
const std::string VERSION = "0.1.1";
const std::string WEBSITE = "https://github.com/bokuyemskyy/keyval";

#if defined(__x86_64__) || defined(_M_X64)
const std::string ARCH = "x86_64";
#elif defined(__i386__) || defined(_M_IX86)
const std::string ARCH = "i686";
#elif defined(__aarch64__)
const std::string ARCH = "arm64";
#elif defined(__arm__) || defined(_M_ARM)
const std::string ARCH = "arm";
#else
const std::string ARCH = "unknown";
#endif

} // namespace AppInfo

void printBanner(const Config& config) {
    // clang-format off
    std::array<std::string, 6> logo = {
        "                                   ", 
        "            .--.                   ",
        "           /.-. '----------.       ", 
        R"(           \'-' .--"--""-"-'       )",
        "            '--'                   ", 
        "                                   "};
    // clang-format on
    std::vector<std::string> info = {AppInfo::NAME + " " + AppInfo::VERSION + " " + AppInfo::ARCH,
                                     "Port: " + config.get("port"), "PID: " + std::to_string(getpid()),
                                     AppInfo::WEBSITE};

    for (size_t i = 0; i < logo.size(); i++) {
        std::cout << logo[i];
        if (i - 1 < info.size()) {
            std::cout << "  " << info[i - 1];
        }
        std::cout << "\n";
    }
}