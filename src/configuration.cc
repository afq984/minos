#include <string>
#include <fstream>
#include <sstream>

#include "config.h"
#include "configuration.h"

namespace minos {
namespace config {

static std::string server_address;

std::string get_config_path() {
    return sysconfdir "/minos.conf";
}

const std::string& get_server_address() {
    return server_address;
}

int parse(const std::string& path) {
    std::ifstream fin(path);
    if (not fin) {
        return 1;
    }
    std::string line;
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        std::string key;
        std::string value;
        if (iss >> key >> value) {
            if (key == "Server") {
                server_address = value;
                return 0;
            }
        }
    }
    return 2;
}

}
}
