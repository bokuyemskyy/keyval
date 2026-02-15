#include "cli.hpp"

#include <iostream>
#include <sstream>
#include <string>

Cli::Cli(const std::string& host, uint16_t port) : m_host(host), m_port(port) {
    m_socket.create();
    m_socket.connect(host, port);

    setupCompletion();
    setupHints();
}

void Cli::setupCompletion() {
    m_repl.set_completion_callback(
        [this](std::string const& input, int& context_len) -> std::vector<replxx::Replxx::Completion> {
            std::vector<replxx::Replxx::Completion> completions;

            std::istringstream iss(input);
            std::string        first;
            iss >> first;

            context_len = first.length();

            for (auto const& cmd : m_registry.commandNames()) {
                if (cmd.rfind(first, 0) == 0) {
                    completions.emplace_back(cmd, replxx::Replxx::Color::DEFAULT);
                }
            }

            return completions;
        });
}
void Cli::setupHints() {
    m_repl.set_hint_callback(
        [this](std::string const& input, int& context_len, replxx::Replxx::Color& color) -> std::vector<std::string> {
            std::vector<std::string> hints;
            std::istringstream       iss(input);
            std::string              cmd;
            iss >> cmd;

            auto meta = m_registry.find(cmd);
            if (!meta)
                return hints;

            if (input.length() >= cmd.length()) {
                context_len = 0;
                color       = replxx::Replxx::Color::GRAY;
                std::ostringstream hint;

                std::string after_cmd = input.substr(cmd.length());

                if (after_cmd.empty()) {
                    hint << " ";
                } else if (!after_cmd.empty() && after_cmd.back() != ' ') {
                    hint << " ";
                }

                for (size_t i = 0; i < meta->argHints.size(); ++i) {
                    if (i > 0)
                        hint << " ";
                    hint << meta->argHints[i];
                }
                hints.push_back(hint.str());
            }
            return hints;
        });
}
std::string Cli::formatResponse(const std::string& resp) {
    std::cout << resp;
    if (resp.empty())
        return "";

    char        type    = resp[0];
    std::string content = resp.substr(1);

    if (content.size() >= 2 && content.substr(content.size() - 2) == "\r\n")
        content = content.substr(0, content.size() - 2);

    switch (type) {
        case '+': // Simple string
            return content;

        case '-': // Error
            return "(error) " + content;

        case ':': // Integer
            return "(integer) " + content;

        case '$': { // Bulk string
            size_t newline_pos = content.find("\r\n");
            if (newline_pos == std::string::npos)
                return content;

            int len = std::stoi(content.substr(0, newline_pos));
            if (len == -1)
                return "(nil)";

            return "\"" + content.substr(newline_pos + 2) + "\"";
        }

        case '*': { // Array
            size_t newline_pos = content.find("\r\n");
            if (newline_pos == std::string::npos)
                return content;

            int count = std::stoi(content.substr(0, newline_pos));
            if (count == 0)
                return "(empty array)";

            // Recursively parse each element
            std::string result;
            std::string remaining = resp.substr(resp.find("\r\n") + 2);

            for (int i = 0; i < count; i++) {
                result += std::to_string(i + 1) + ") ";

                // Find the next complete element
                if (remaining.empty())
                    break;

                char elem_type = remaining[0];
                if (elem_type == '$') {
                    size_t len_end  = remaining.find("\r\n");
                    int    elem_len = std::stoi(remaining.substr(1, len_end - 1));

                    if (elem_len == -1) {
                        result += "(nil)\n";
                        remaining = remaining.substr(len_end + 2);
                    } else {
                        std::string value = remaining.substr(len_end + 2, elem_len);
                        result += "\"" + value + "\"\n";
                        remaining = remaining.substr(len_end + 2 + elem_len + 2);
                    }
                }
            }

            return result;
        }

        default:
            return resp;
    }
}

void Cli::handleLine(const std::string& line) {
    if (line == "quit" || line == "exit")
        std::exit(0);

    try {
        std::string request = line + "\r\n";
        m_socket.send(request);

        std::string response;
        m_socket.recv(response);

        std::string formatted = formatResponse(response);
        if (!formatted.empty())
            std::cout << formatted << "\n";

    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void Cli::run() {
    std::string prompt = m_host + ":" + std::to_string(m_port) + "> ";
    while (true) {
        const char* input = m_repl.input(prompt.c_str());
        if (!input)
            break;
        std::string line(input);
        if (!line.empty()) {
            m_repl.history_add(line);
            handleLine(line);
        }
    }
}