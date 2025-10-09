#pragma once
#include <string>
#include <vector>

struct Command
{
    std::string name;
    std::vector<std::string> args;
};

class Protocol
{
public:
    static Command deserialize(const std::string &data);
    static std::string serialize(const Command &cmd);
};
