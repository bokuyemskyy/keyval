#pragma once

#include <unistd.h>

#include <array>
#include <format>
#include <iostream>
#include <vector>

#include "config.hpp"

namespace AppInfo {
inline constexpr auto Name    = "Keyval";
inline constexpr auto Version = "0.1.1";
inline constexpr auto Website = "https://github.com/bokuyemskyy/keyval";

#if defined(__x86_64__) || defined(_M_X64)
inline constexpr auto Arch = "x86_64";
#elif defined(__i386__) || defined(_M_IX86)
inline constexpr auto Arch = "i686";
#elif defined(__aarch64__)
inline constexpr auto Arch = "arm64";
#elif defined(__arm__) || defined(_M_ARM)
inline constexpr auto Arch = "arm";
#else
inline constexpr auto Arch = "unknown";
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
    std::vector<std::string> info = {std::format("{} {} {}", AppInfo::Name, AppInfo::Version, AppInfo::Arch),
                                     std::format("Port: {}", config.get("port")), std::format("PID: {}", getpid()),
                                     std::format("{}", AppInfo::Website)};

    for (size_t i = 0; i < logo.size(); i++) {
        std::cout << logo[i];
        if (i - 1 < info.size()) {
            std::cout << "  " << info[i - 1];
        }
        std::cout << "\n";
    }
}