#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include <string>

namespace CommonFunctions {
    bool convertStringToULong(const std::string& input, unsigned long& output);
    void printError(const std::string& streamName);
}

#endif // COMMON_FUNCTIONS_H
