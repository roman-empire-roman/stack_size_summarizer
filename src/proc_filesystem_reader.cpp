#include "proc_filesystem_reader.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/mpl/at.hpp>
#include <proc/readproc.h>

#include "common_functions.h"
#include "file_stream_wrapper.h"
#include "ptr_wrapper.h"

namespace {
    template <typename FuncType>
    using ResultType = typename boost::function_types::result_type<FuncType>::type;

    template <typename FuncType, size_t argIndex>
    using ArgType = typename boost::mpl::at_c<
        boost::function_types::parameter_types<FuncType>, argIndex
    >::type;

    using StackSizeType = ProcFilesystemReader::StackSizeType;

    constexpr std::string g_stackLabelName("nr_kernel_stack");
}

namespace StreamOpenModes {
    constexpr auto ReadMode = std::ios::in;
}

ProcFilesystemReader::ProcFilesystemReader() {
    // openproc
    static_assert(
        true == std::is_same<
            ResultType<decltype(openproc)>, PROCTAB*
        >::value
    );

    // closeproc
    static_assert(
        true == std::is_same<
            ArgType<decltype(closeproc), 0>, PROCTAB*
        >::value
    );

    // readproc
    static_assert(
        true == std::is_same<
            ResultType<decltype(readproc)>, proc_t*
        >::value
    );
    static_assert(
        true == std::is_same<
            ArgType<decltype(readproc), 0>, PROCTAB*
        >::value
    );
    static_assert(
        true == std::is_same<
            ArgType<decltype(readproc), 1>, proc_t*
        >::value
    );

    // readtask
    static_assert(
        true == std::is_same<
            ResultType<decltype(readtask)>, proc_t*
        >::value
    );
    static_assert(
        true == std::is_same<
            ArgType<decltype(readtask), 0>, PROCTAB*
        >::value
    );
    static_assert(
        true == std::is_same<
            ArgType<decltype(readtask), 1>, const proc_t*
        >::value
    );
    static_assert(
        true == std::is_same<
            ArgType<decltype(readtask), 2>, proc_t*
        >::value
    );

    // vm_stack
    static_assert(
        true == std::is_same<
            decltype(proc_t::vm_stack), StackSizeType
        >::value
    );
}

ProcFilesystemReader::~ProcFilesystemReader() {
    reset();
}

bool ProcFilesystemReader::setup() {
    if (nullptr != m_procDataAccessPoint) {
        std::cout << "{ProcFilesystemReader::setup}; pointer to PROCTAB data structure is NOT NULL" << std::endl;
        return true;
    }
    m_procDataAccessPoint = openproc(PROC_FILLSTATUS);
    if (nullptr == m_procDataAccessPoint) {
        std::cerr << "{ProcFilesystemReader::setup}; pointer to PROCTAB data structure is NULL" << std::endl;
        return false;
    }
    return true;
}

bool ProcFilesystemReader::reset() {
    if (nullptr == m_procDataAccessPoint) {
        std::cout << "{ProcFilesystemReader::reset}; pointer to PROCTAB data structure is NULL" << std::endl;
        return false;
    }
    closeproc(m_procDataAccessPoint);
    m_procDataAccessPoint = nullptr;
    return true;
}

std::optional<StackSizeType> ProcFilesystemReader::calcSumStackSize() const {
    using namespace PtrWrapperSpace;

    if (nullptr == m_procDataAccessPoint) {
        std::cerr << "{ProcFilesystemReader::calcSumStackSize}; pointer to PROCTAB data structure is NULL" << std::endl;
        return std::nullopt;
    }

    StackSizeType sumStackSize = 0;
    constexpr auto maxStackSize = std::numeric_limits<StackSizeType>::max();
    do {
        auto processInfoCreator = [this] () -> proc_t* {
            auto processInfo = readproc(m_procDataAccessPoint, nullptr);
            return processInfo;
        };
        PtrWrapper<proc_t> processInfo(processInfoCreator, freeproc);

        bool noMoreProcessesLeft = (nullptr == processInfo.get());
        if (noMoreProcessesLeft) {
            break;
        }

        StackSizeType sumThreadStackSize = 0;
        std::size_t nThreads = 0;
        do {
            proc_t threadInfo = {};
            {
                auto ptrToThreadInfo = readtask(m_procDataAccessPoint, processInfo.get(), &threadInfo);
                bool noMoreThreadsLeft = (nullptr == ptrToThreadInfo);
                if (noMoreThreadsLeft) {
                    break;
                }
            }
            ++nThreads;

            const auto& threadStackSize = threadInfo.vm_stack;
            std::cout << "{ProcFilesystemReader::calcSumStackSize}; "
                "process id: '" << processInfo->tgid << "'; "
                "thread id: '" << threadInfo.tid << "'; "
                "thread stack size: '" << threadStackSize << " kilobyte(s)'" << std::endl;

            if ((maxStackSize - threadStackSize) <= sumThreadStackSize) {
                std::cerr << "{ProcFilesystemReader::calcSumStackSize}; summed thread stack size overflow" << std::endl;
                return std::nullopt;
            }
            sumThreadStackSize += threadStackSize;
        } while(true);

        if (0 == nThreads) {
            std::cerr << "{ProcFilesystemReader::calcSumStackSize}; number of threads is equal to zero; "
                "process id: '" << processInfo->tgid << "'" << std::endl;
            return std::nullopt;
        }

        if ((maxStackSize - sumThreadStackSize) <= sumStackSize) {
            std::cerr << "{ProcFilesystemReader::calcSumStackSize}; summed stack size overflow" << std::endl;
            return std::nullopt;
        }
        sumStackSize += sumThreadStackSize;
    } while(true);

    return std::make_optional<StackSizeType>(sumStackSize);
}

std::optional<StackSizeType> ProcFilesystemReader::getKernelStackSize() const {
    auto kernelStackSize = getStackSizeFromVmStat();
    if (kernelStackSize) {
        // std::cout << "{ProcFilesystemReader::getKernelStackSize}; kernel stack size was successfully extracted from "
        //     "file '/proc/vmstat'" << std::endl;
    } else {
        std::cerr << "{ProcFilesystemReader::getKernelStackSize}; unable to get kernel stack size from file '/proc/vmstat'" << std::endl;
        kernelStackSize = getStackSizeFromZoneInfo();
        if (kernelStackSize) {
            // std::cout << "{ProcFilesystemReader::getKernelStackSize}; kernel stack size was successfully extracted from "
            //     "file '/proc/zoneinfo'" << std::endl;
        } else {
            std::cerr << "{ProcFilesystemReader::getKernelStackSize}; unable to get kernel stack size from file '/proc/zoneinfo'" << std::endl;
            return std::nullopt;
        }
    }
    return kernelStackSize;
}

std::optional<StackSizeType> ProcFilesystemReader::getStackSizeFromVmStat() const {
    static constexpr std::string fileName("/proc/vmstat");
    FileStreamWrapper streamWrapper(
        fileName, static_cast<int>(StreamOpenModes::ReadMode)
    );
    if (!streamWrapper.isValid()) {
        return std::nullopt;
    }

    while (true) {
        std::string line;
        std::getline(streamWrapper.getFileStream(), line);
        if (streamWrapper.isEndOfStreamReached()) {
            break;
        }
        // std::cout << line << std::endl;

        boost::algorithm::trim(line);
        if (line.empty()) {
            continue;
        }

        std::istringstream stringStream(line, StreamOpenModes::ReadMode);
        std::string labelName;
        StackSizeType labelValue = 0;
        stringStream >> labelName >> labelValue;
        if (g_stackLabelName != labelName) {
            continue;
        }

        return std::make_optional<StackSizeType>(labelValue);
    }
    std::cerr << "{ProcFilesystemReader::getStackSizeFromVmStat}; "
        "stack label '" << g_stackLabelName << "' was NOT found in "
        "file '" << fileName << "'" << std::endl;
    return std::nullopt;
}

std::optional<StackSizeType> ProcFilesystemReader::getStackSizeFromZoneInfo() const {
    static constexpr std::string fileName("/proc/zoneinfo");
    FileStreamWrapper streamWrapper(
        fileName, static_cast<int>(StreamOpenModes::ReadMode)
    );
    if (!streamWrapper.isValid()) {
        return std::nullopt;
    }

    while (true) {
        std::string line;
        std::getline(streamWrapper.getFileStream(), line);
        if (streamWrapper.isEndOfStreamReached()) {
            break;
        }
        // std::cout << line << std::endl;

        boost::algorithm::trim(line);
        if (line.empty()) {
            continue;
        }

        std::vector<std::string> tokens;
        try {
            boost::split(
                tokens, line, boost::is_any_of(" ")
            );
        } catch (const std::bad_alloc& exception) {
            std::cerr << "{ProcFilesystemReader::getStackSizeFromZoneInfo}; "
                "exception 'std::bad_alloc' was successfully caught while "
                "splitting line '" << line << "' into the token list; "
                "exception description: '" << exception.what() << "'" << std::endl;
            return std::nullopt;
        } catch (...) {
            std::cerr << "{ProcFilesystemReader::getStackSizeFromZoneInfo}; "
                "unknown exception was caught while "
                "splitting line '" << line << "' into the token list" << std::endl;
            return std::nullopt;
        }

        if (2 != tokens.size()) {
            continue;
        }
        const auto& labelName = tokens.at(0);
        const auto& labelValue = tokens.at(1);
        if (g_stackLabelName != labelName) {
            continue;
        }

        StackSizeType stackSize = 0;
        if (!CommonFunctions::convertStringToULong(labelValue, stackSize)) {
            continue;
        }
        return std::make_optional<StackSizeType>(stackSize);
    }
    std::cerr << "{ProcFilesystemReader::getStackSizeFromZoneInfo}; "
        "stack label '" << g_stackLabelName << "' was NOT found in "
        "file '" << fileName << "'" << std::endl;
    return std::nullopt;
}
