#include "common/protocol.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

Command Protocol::deserialize(const std::string &data)
{
    Command cmd;
    std::istringstream stream(data);
    std::string token;

    if (!(stream >> token))
    {
        throw std::runtime_error("Empty command");
    }

    std::transform(token.begin(), token.end(), token.begin(),
                   [](unsigned char c)
                   { return std::toupper(c); });
    cmd.name = token;

    while (stream >> token)
    {
        cmd.args.push_back(token);
    }

    return cmd;
}

std::string Protocol::serialize(const Command &cmd)
{
    std::ostringstream stream;
    stream << cmd.name;

    for (const auto &arg : cmd.args)
    {
        stream << " " << arg;
    }

    stream << "\n";
    return stream.str();
}
