#include "file_stream_wrapper.h"

#include <iostream>

#include "common_functions.h"

FileStreamWrapper::FileStreamWrapper(const std::string& fileName, int streamOpenMode) {
    if (fileName.empty()) {
        std::cerr << "{FileStreamWrapper::FileStreamWrapper}; "
            "file name is empty" << std::endl;
        return;
    }
    if (!CommonFunctions::fileExists(fileName)) {
        return;
    }
    if (!CommonFunctions::isRegularFile(fileName)) {
        return;
    }
    m_fileName = fileName;

    if (!isStreamOpenModeValid(streamOpenMode)) {
        std::cerr << "{FileStreamWrapper::FileStreamWrapper}; "
            "stream open mode '" << streamOpenMode << "' is NOT valid" << std::endl;
        return;
    }
    m_streamOpenMode = streamOpenMode;

    m_fileStream.open(
        fileName, static_cast<std::ios_base::openmode>(streamOpenMode)
    );
}

FileStreamWrapper::~FileStreamWrapper() {
    if (m_fileName.empty() || (-1 == m_streamOpenMode)) {
        return;
    }

    if (isOpen()) {
        m_fileStream.close();
        if (isFailOrBadBitSet()) {
            CommonFunctions::printFileError(m_fileName);
        } else {
            // std::cout << "{FileStreamWrapper::~FileStreamWrapper}; "
            //    "file '" << m_fileName << "' was successfully closed" << std::endl;
        }
    }
}

bool FileStreamWrapper::isValid() const {
    if (m_fileName.empty() || (-1 == m_streamOpenMode)) {
        return false;
    }

    if (!isOpen()) {
        std::cerr << "{FileStreamWrapper::isValid}; "
            "file '" << m_fileName << "' was NOT opened" << std::endl;
        CommonFunctions::printFileError(m_fileName);
        return false;
    }
    if (isFailOrBadBitSet()) {
        CommonFunctions::printFileError(m_fileName);
        return false;
    }
    // std::cout << "{FileStreamWrapper::isValid}; "
    //    "file '" << m_fileName << "' was successfully opened" << std::endl;
    return true;
}

bool FileStreamWrapper::isStreamOpenModeValid(int streamOpenMode) const {
    if (0 == streamOpenMode) {
        return false;
    }

    int mask =
        static_cast<int>(std::ios::app) | static_cast<int>(std::ios::ate) |
        static_cast<int>(std::ios::binary) | static_cast<int>(std::ios::in) |
        static_cast<int>(std::ios::out) | static_cast<int>(std::ios::trunc);
    int applyingMaskResult = streamOpenMode & mask;
    return (streamOpenMode == applyingMaskResult);
}
