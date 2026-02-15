#include "cli.hpp"
#include "config.hpp"

#include <iostream>

int main(int argc, char** argv) {
    Config config{argc, argv};

    try {
        Cli cli(config.host, config.port);
        cli.run();
    } catch (std::exception const& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }

    return 0;
}