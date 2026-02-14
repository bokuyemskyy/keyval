#pragma once

#include <memory> // NOLINT
#include <sstream>
#include <string>
#include <variant>
#include <vector>

struct Request {
    std::string              m_command;
    std::vector<std::string> m_args;
};

struct Response;
using ResponseValue = std::variant<std::monostate, std::string, long long, std::vector<std::shared_ptr<Response>>>;

enum class ResponseType : uint8_t {
    SIMPLE_STRING,
    ERR,
    INTEGER,
    BULK_STRING,
    ARRAY,
    NIL,
    SHUTDOWN
};

struct Response {
    ResponseType  m_type;
    ResponseValue m_value;
};

class Protocol {
  public:
    static Request     deserializeRequest(const std::string& input);
    static std::string serializeResponse(const Response& response);

  private:
    static std::vector<std::string> parseArray(const std::string& input);
    static std::vector<std::string> parseInline(const std::string& input);
    static void                     getlineCRLF(std::istringstream& stream, std::string& line);
};
