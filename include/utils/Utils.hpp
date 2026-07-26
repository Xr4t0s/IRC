#pragma once

#include <string>
#include <vector>

class Channel;
class Client;

std::string intToString(int nb);
int stringToInt(const std::string& str);
std::vector<std::string> splitBy(const std::string& full, const char sep);
std::string serializeMode(Channel* channel, Client& client);