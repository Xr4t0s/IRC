#include "utils/Utils.hpp"
#include <sstream>

std::string intToString(int nb) {
    std::ostringstream oss;
    oss << nb;
    return oss.str();
}
