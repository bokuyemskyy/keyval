#pragma once

#ifdef _WIN32
#include <process.h>
#define getpid() _getpid()
#else
#include <unistd.h>
#endif

#include <array>
#include <format>
#include <iostream>
#include <vector>

#include "config.hpp"

namespace AppInfo {
const std::string Name    = "Keyval";
const std::string Version = "0.1.1";
const std::string Website = "https://github.com/bokuyemskyy/keyval";

#if defined(__x86_64__) || defined(_M_X64)
const std::string Arch = "x86_64";
#elif defined(__i386__) || defined(_M_IX86)
const std::string Arch = "i686";
#elif defined(__aarch64__)
const std::string Arch = "arm64";
#elif defined(__arm__) || defined(_M_ARM)
const std::string Arch = "arm";
#else
const std::string Arch = "unknown";
#endif

}  // namespace AppInfo

void printBanner(const Config& config) {
    // clang-format off
    std::array<std::string, 6> logo = {
        "                                   ", 
        "            .--.                   ",
        "           /.-. '----------.       ", 
        "           \\'-' .--\"--\"\"-\"-'       ",
        "            '--'                   ", 
        "                                   "};
    // clang-format on
    std::vector<std::string> info = {
        AppInfo::Name + " " + AppInfo::Version + " " + AppInfo::Arch,
        "Port: " + config.get("port"),
        "PID: " + std::to_string(getpid()),
        AppInfo::Website
    };

    for (size_t i = 0; i < logo.size(); i++) {
        std::cout << logo[i];
        if (i - 1 < info.size()) {
            std::cout << "  " << info[i - 1];
        }
        std::cout << "\n";
    }
}