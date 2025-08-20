#pragma once

#include <iostream>
#include <vector>
#include <unistd.h>

#include "config.hpp"

namespace AppInfo
{
  inline constexpr auto Name = "Squirrel";
  inline constexpr auto Version = "0.1.0";
  inline constexpr auto Website = "https://github.com/bokuyemskyy/squirrel";

#ifdef __x86_64__
  inline constexpr auto Arch = "x86_64";
#else
  inline constexpr auto Arch = "i686";
#endif

}

void printBanner(const Config &config)
{
  std::array<std::string, 17> logo = {
      "                       ▓▓  ▓▓         ",
      "     ▓▓▓▓▓▓▓▓         ▓▓▓▓▒▒▓▓        ",
      "  ▓▓▓▓▒▒▒▒▒▒▒▒▓▓      ▓▓░░  ░░▓▓      ",
      "    ▓▓░░▒▒░░▒▒▒▒▓▓    ▓▓░░▓▓  ░░▓▓    ",
      "      ▓▓  ▒▒▒▒▒▒▓▓    ▓▓░░░░░░░░░░▓▓  ",
      "        ▓▓░░▒▒░░▒▒▓▓▓▓░░░░░░░░░░  ▒▒  ",
      "          ▓▓  ▒▒▒▒▒▒▓▓░░░░░░▓▓▓▓▒▒    ",
      "          ▓▓░░▒▒▒▒▓▓░░░░▒▒▓▓          ",
      "        ▓▓░░▒▒░░▓▓░░░░░░░░░░▓▓▓▓▒▒    ",
      "      ▓▓  ▒▒▒▒▓▓░░░░░░░░░░░░░░▓▓▓▓    ",
      "    ▓▓░░▒▒░░▓▓░░░░░░░░░░░░  ▓▓▓▓      ",
      "    ▓▓░░▒▒▒▒▓▓░░░░▒▒░░░░  ▓▓          ",
      "  ▓▓  ▒▒▒▒▓▓░░░░░░░░▒▒  ▓▓            ",
      "    ▓▓░░▒▒▓▓░░░░░░░░▒▒  ▓▓▓▓          ",
      "    ▓▓░░▒▒▓▓░░░░░░▒▒▒▒▒▒▓▓▒▒▒▒        ",
      "      ▓▓  ▓▓░░░░░░░░░░░░░░▓▓▓▓        ",
      "        ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓          "};

  std::vector<std::string> info = {
      std::format("{} {} {}", AppInfo::Name, AppInfo::Version, AppInfo::Arch),
      std::format("Port: {}", config.get("port")),
      std::format("PID: {}", getpid()),
      std::format("{}", AppInfo::Website)};

  std::cout << "\n";
  for (size_t i = 0; i < logo.size(); i++)
  {
    std::cout << logo[i];
    if (i > 2 && i - 3 < info.size())
    {
      std::cout << "  " << info[i - 3];
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}