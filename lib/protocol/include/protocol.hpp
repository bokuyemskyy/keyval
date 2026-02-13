#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <variant>
#include <memory>

struct Request
{
    std::string command;
    std::vector<std::string> args;
};

struct Response;
using ResponseValue = std::variant<
    std::monostate,
    std::string,
    long long,
    std::vector<std::shared_ptr<Response>>>;

enum class ResponseType
{
    SIMPLE_STRING,
    ERR,
    INTEGER,
    BULK_STRING,
    ARRAY,
    NIL,
    SHUTDOWN
};

struct Response
{
    ResponseType type;
    ResponseValue value;
};

class Protocol
{
public:
    static Request deserializeRequest(const std::string &input);
    static std::string serializeResponse(const Response &response);

private:
    static std::vector<std::string> parseArray(const std::string &input);
    static std::vector<std::string> parseInline(const std::string &input);
    static void getlineCRLF(std::istringstream &stream, std::string &line);
};
