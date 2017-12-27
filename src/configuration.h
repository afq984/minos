#include <string>

namespace minos {
namespace config {

const std::string& get_server_address();
std::string get_config_path();
int parse(const std::string&);

}

}
