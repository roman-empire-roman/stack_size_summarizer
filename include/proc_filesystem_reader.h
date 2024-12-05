#ifndef PROC_FILESYSTEM_READER_H
#define PROC_FILESYSTEM_READER_H

#include <optional>

struct PROCTAB;

class ProcFilesystemReader {
public:
    using StackSizeType = unsigned long;

    ProcFilesystemReader();
    ProcFilesystemReader(const ProcFilesystemReader& other) = delete;
    ProcFilesystemReader& operator=(const ProcFilesystemReader& other) = delete;
    ~ProcFilesystemReader();
    ProcFilesystemReader(ProcFilesystemReader&& other) = delete;
    ProcFilesystemReader& operator=(ProcFilesystemReader&& other) = delete;

    bool setup();
    bool reset();

    std::optional<StackSizeType> calcSumStackSize() const;
    std::optional<StackSizeType> getKernelStackSize() const;

private:
    std::optional<StackSizeType> getStackSizeFromVmStat() const;
    std::optional<StackSizeType> getStackSizeFromZoneInfo() const;

private:
    PROCTAB* m_procDataAccessPoint = nullptr;
};

#endif // PROC_FILESYSTEM_READER_H
