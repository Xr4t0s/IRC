#include "utils/Utils.hpp"
#include "core/Channel.hpp"
#include "core/Client.hpp"
#include <sstream>
#include <climits>


std::string intToString(int nb) {
    std::ostringstream oss;
    oss << nb;
    return oss.str();
}

int stringToInt(const std::string& str) {
    if (str.empty())
        return -1;

    long result = 0;

    for (std::size_t i = 0; i < str.size(); ++i) {
        if (str[i] < '0' || str[i] > '9')
            return -1;

        result = result * 10 + (str[i] - '0');

        if (result > INT_MAX)
            return -1;
    }

    return static_cast<int>(result);
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

std::string serializeMode(Channel* channel, Client& client) {
    std::string modes = "+";
    std::string params;

    if (channel->i)
        modes += "i";

    if (channel->t)
        modes += "t";

    if (!channel->k.empty()) {
        modes += "k";
        params += " ";
        if (channel->findClient(client))
            params += channel->k;
        else
            params += "********";
    }

    if (channel->l != -1) {
        modes += "l";
        params += " ";
        params += intToString(channel->l);
    }

    return modes + params;
}