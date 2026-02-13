#include "protocol.hpp"

#include <sstream>
#include <stdexcept>
#include <variant>

Request Protocol::deserializeRequest(const std::string& input) {
    std::vector<std::string> tokens;

    if (input.empty()) throw std::runtime_error("Empty request");

    if (input[0] == '*')
        tokens = parseArray(input);
    else
        tokens = parseInline(input);

    if (tokens.empty()) throw std::runtime_error("Invalid request");

    return {tokens[0], std::vector<std::string>(tokens.begin() + 1, tokens.end())};
}

std::vector<std::string> Protocol::parseArray(const std::string& input) {
    std::vector<std::string> result;
    std::istringstream       stream(input);
    std::string              line;

    getlineCRLF(stream, line);
    if (line.empty() || line[0] != '*') throw std::runtime_error("Invalid array request");

    int count = std::stoi(line.substr(1));

    for (int i = 0; i < count; i++) {
        getlineCRLF(stream, line);
        if (line.empty() || line[0] != '$') throw std::runtime_error("Expected bulk string");

        int len = std::stoi(line.substr(1));
        if (len < 0) {
            result.push_back("");
            continue;
        }

        std::string value(len, '\0');
        stream.read(&value[0], len);
        stream.ignore(2);
        result.push_back(value);
    }
    return result;
}

std::vector<std::string> Protocol::parseInline(const std::string& input) {
    std::vector<std::string> result;
    std::istringstream       stream(input);
    std::string              token;
    while (stream >> token) result.push_back(token);
    return result;
}

void Protocol::getlineCRLF(std::istringstream& stream, std::string& line) {
    std::getline(stream, line);
    if (!line.empty() && line.back() == '\r') line.pop_back();
}

std::string Protocol::serializeResponse(const Response& response) {
    switch (response.type) {
        case ResponseType::SIMPLE_STRING:
            if (auto str = std::get_if<std::string>(&response.value)) return "+" + *str + "\r\n";
            break;

        case ResponseType::ERR:
            if (auto str = std::get_if<std::string>(&response.value)) return "-" + *str + "\r\n";
            break;

        case ResponseType::INTEGER:
            if (auto val = std::get_if<long long>(&response.value)) return ":" + std::to_string(*val) + "\r\n";
            break;

        case ResponseType::BULK_STRING:
            if (auto val = std::get_if<std::string>(&response.value))
                return "$" + std::to_string(val->size()) + "\r\n" + *val + "\r\n";
            break;

        case ResponseType::ARRAY:
            if (auto arr = std::get_if<std::vector<std::shared_ptr<Response>>>(&response.value)) {
                std::string out = "*" + std::to_string(arr->size()) + "\r\n";
                for (const auto& el : *arr) out += serializeResponse(*el);
                return out;
            }
            break;
        case ResponseType::SHUTDOWN:
            return "";
        case ResponseType::NIL:
            return "$-1\r\n";
    }
    return "-ERR unknown response type\r\n";
}
