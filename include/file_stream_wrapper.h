#ifndef FILE_STREAM_WRAPPER_H
#define FILE_STREAM_WRAPPER_H

#include <fstream>
#include <string>

class FileStreamWrapper {
public:
    explicit FileStreamWrapper(const std::string& fileName, int streamOpenMode);
    ~FileStreamWrapper();

    FileStreamWrapper(const FileStreamWrapper& other) = delete;
    FileStreamWrapper& operator=(const FileStreamWrapper& other) = delete;
    FileStreamWrapper(FileStreamWrapper&& other) = delete;
    FileStreamWrapper& operator=(FileStreamWrapper&& other) = delete;

    bool isValid() const;
    bool isEndOfStreamReached() const { return m_fileStream.eof(); }

    std::ifstream& getFileStream() { return m_fileStream; }

private:
    bool isOpen() const { return m_fileStream.is_open(); }
    bool isFailOrBadBitSet() const { return m_fileStream.fail(); }
    bool isStreamOpenModeValid(int streamOpenMode) const;

private:
    std::ifstream m_fileStream;
    std::string m_fileName;
    int m_streamOpenMode = -1;
};

#endif // FILE_STREAM_WRAPPER_H
