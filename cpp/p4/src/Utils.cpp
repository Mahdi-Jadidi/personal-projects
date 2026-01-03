#include "include/Utils.hpp"
#include <sstream>

std::vector<std::string> chopString(const std::string &input, char separator) {
    std::vector<std::string> parts;
    std::string part;
    std::istringstream stream(input);
    while (getline(stream, part, separator)) {
        parts.push_back(part);
    }
    return parts;
}

std::string stripMarks(const std::string &input) {
    return input.substr(2, input.length() - 3);
}
