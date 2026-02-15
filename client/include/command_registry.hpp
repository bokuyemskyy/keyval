#pragma once
#include <string>
#include <unordered_map>
#include <vector>

struct CommandMeta {
    std::string              name;
    int                      minArgs;
    int                      maxArgs;
    std::vector<std::string> argHints;
};

class CommandRegistry {
  public:
    CommandRegistry();

    const CommandMeta*       find(const std::string& name) const;
    std::vector<std::string> commandNames() const;

  private:
    std::unordered_map<std::string, CommandMeta> m_commands;
};