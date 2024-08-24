#include "file_stream_wrapper.h"

#include <iostream>

#include "common_functions.h"

FileStreamWrapper::FileStreamWrapper(const std::string& fileName, int streamOpenMode) :
    m_fileName{ fileName }
{
    if (fileName.empty()) {
        std::cerr << "{FileStreamWrapper::FileStreamWrapper}; "
            "file name is empty" << std::endl;
        return;
    }
    if (!isStreamOpenModeValid(streamOpenMode)) {
        std::cerr << "{FileStreamWrapper::FileStreamWrapper}; "
            "stream open mode '" << streamOpenMode << "' is NOT valid" << std::endl;
        return;
    }

    m_fileStream.open(
        fileName, static_cast<std::ios_base::openmode>(streamOpenMode)
    );
    if (!isOpen()) {
        std::cerr << "{FileStreamWrapper::FileStreamWrapper}; "
            "file '" << fileName << "' was NOT opened" << std::endl;
        CommonFunctions::printError(fileName);
        return;
    }

    // std::cout << "{FileStreamWrapper::FileStreamWrapper}; "
    //    "file '" << fileName << "' was successfully opened" << std::endl;
}

FileStreamWrapper::~FileStreamWrapper() {
    if (isOpen()) {
        m_fileStream.close();
        // std::cout << "{FileStreamWrapper::~FileStreamWrapper}; "
        //    "file '" << m_fileName << "' was successfully closed" << std::endl;
    }
}

bool FileStreamWrapper::isValid() const {
    if (!isOpen()) {
        return false;
    }
    if (isFailOrBadBitSet()) {
        CommonFunctions::printError(m_fileName);
        return false;
    }
    return true;
}

bool FileStreamWrapper::isStreamOpenModeValid(int streamOpenMode) const {
    int mask =
        static_cast<int>(std::ios::app) | static_cast<int>(std::ios::ate) |
        static_cast<int>(std::ios::binary) | static_cast<int>(std::ios::in) |
        static_cast<int>(std::ios::out) | static_cast<int>(std::ios::trunc);
    int applyingMaskResult = streamOpenMode & mask;
    return (0 != applyingMaskResult);
}
