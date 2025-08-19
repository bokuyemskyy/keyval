#pragma once

#include <fstream>

#include "logger.hpp"
#include <assert.h>

class Config
{
public:
    Config(int argc, char **argv) { parseArgs(argc, argv); }

    std::string get(const std::string &key) const
    {
        auto it = options.find(key);
        assert(it != options.end());
        return it->second;
    }

    int getInt(const std::string &key) const
    {
        return std::stoi(get(key));
    }

private:
    std::unordered_map<std::string, std::string> options{
        {"port", "6380"},
    };
    void parseArgs(int argc, char **argv)
    {
        int i = 1;

        // first argument: is it a --option or a stdin / file input
        if (i < argc && !std::string(argv[i]).starts_with("--"))
        {
            loadFile(argv[i]);
            i++;
        }
        // all the following ones
        for (; i < argc; i++)
        {
            std::string arg = argv[i];
            if (arg.starts_with("--"))
            {
                std::string key = arg.substr(2);
                std::string value;
                // check if there any arg left AND current parameter isnt just a true/false
                if (i + 1 < argc && argv[i + 1][0] != '-')
                {
                    value = argv[++i];
                }
                else // the current parameter is a true / false
                {
                    value = "true";
                }
                options[key] = value;
            }
        }
    }
    void loadFile(const std::string &path)
    {
        std::istream *in;   // generic pointer to the stream, in which the config will flow
        std::ifstream file; // out of "if" scope because will be accessed after
        if (path != "-")
        {
            file.open(path);
            if (!file)
            {
                Logger::log(LogLevel::WARN, "Config file not found. Proceeding.");
                return;
            }
            in = &file;
        }
        else
        {
            in = &std::cin;
        }
        std::string key, value;
        while (*in >> key >> value)
        {
            options[key] = value;
        }
    }
};