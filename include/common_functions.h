#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include <string>

namespace CommonFunctions {
    bool convertStringToULong(const std::string& input, unsigned long& output);
    void printFileError(const std::string& fileName);
    void printSemaphoreError();
    bool fileExists(const std::string& fileName);
    bool isRegularFile(const std::string& fileName);
}

#endif // COMMON_FUNCTIONS_H
