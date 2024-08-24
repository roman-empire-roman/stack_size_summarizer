#include <iostream>
#include <optional>

#include "proc_filesystem_reader.h"
#include "signal_handler_observer.h"
#include "sleep_primitive.h"

namespace {
    constexpr int g_nMinutes = 1;

    using StackSizeType = ProcFilesystemReader::StackSizeType;
}

int main() {
    SignalHandlerObserver::getInstance();

    ProcFilesystemReader reader;

    std::optional<StackSizeType> prevSumStackSize{ std::nullopt };
    std::optional<StackSizeType> prevKernelStackSize{ std::nullopt };

    auto differencePrinter = [] (
        const std::optional<StackSizeType>& prevStackSize,
        const std::optional<StackSizeType>& curStackSize,
        std::string&& spaceType
    ) -> bool {
        if (!curStackSize || spaceType.empty()) {
            return false;
        }
        if (prevStackSize) {
            if (curStackSize.value() > prevStackSize.value()) {
                auto difference = curStackSize.value() - prevStackSize.value();
                std::cout << "{main}; {differencePrinter}; the amount of memory reserved for the stack in " << spaceType << " has increased by "
                    "'" << difference << " kilobyte(s)' in the last '" << g_nMinutes << " minute(s)'" << std::endl;
            } else if (prevStackSize.value() > curStackSize.value()) {
                auto difference = prevStackSize.value() - curStackSize.value();
                std::cout << "{main}; {differencePrinter}; the amount of memory reserved for the stack in " << spaceType << " has decreased by "
                    "'" << difference << " kilobyte(s)' in the last '" << g_nMinutes << " minute(s)'" << std::endl;
            } else {
                std::cout << "{main}; {differencePrinter}; the amount of memory reserved for the stack in " << spaceType << " hasn't changed in the "
                    "last '" << g_nMinutes << " minute(s)'" << std::endl;
            }
        }
        return true;
    };

    while (true) {
        if (!reader.setup()) {
            return -1;
        }

        auto sumStackSize = reader.calcSumStackSize();
        if (!sumStackSize) {
            return -1;
        }
        std::cout << "{main}; sum stack size: '" << sumStackSize.value() << " kilobyte(s)'" << std::endl;

        auto kernelStackSize = reader.getKernelStackSize();
        if (!kernelStackSize) {
            return -1;
        }
        std::cout << "{main}; kernel stack size: '" << kernelStackSize.value() << " kilobyte(s)'" << std::endl;

        if (!differencePrinter(
            prevSumStackSize, sumStackSize, "user space"
        )) {
            return -1;
        }
        if (!differencePrinter(
            prevKernelStackSize, kernelStackSize, "kernel space"
        )) {
            return -1;
        }
        prevSumStackSize = sumStackSize;
        prevKernelStackSize = kernelStackSize;

        auto& primitive = SleepPrimitive::getInstance();
        if (!primitive.sleepFor(g_nMinutes)) {
            break;
        }

        if (!reader.reset()) {
            return -1;
        }
    }
    return 0;
}
