#include "utils/Utils.hpp"
#include <sstream>

std::string intToString(int nb) {
    std::ostringstream oss;
    oss << nb;
    return oss.str();
}

std::vector<std::string> splitBy(const std::string& full, const char sep)
{
    std::vector<std::string> result;
    std::string current;

    for (std::string::size_type i = 0; i < full.size(); ++i)
    {
        if (full[i] == sep)
        {
            result.push_back(current);
            current.clear();
        }
        else
        {
            current += full[i];
        }
    }

    result.push_back(current);
    return result;
}