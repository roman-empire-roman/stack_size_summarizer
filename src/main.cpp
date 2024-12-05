#include <iostream>
#include <string_view>

#include "proc_filesystem_reader.h"
#include "signal_handler_observer.h"
#include "sleep_primitive.h"

namespace {
    constexpr int g_nMinutes = 1;

    using StackSizeType = ProcFilesystemReader::StackSizeType;
    using SleepResult = SleepPrimitive::SleepResult;
}

int main() {
    auto& observer = SignalHandlerObserver::getInstance();
    if (!observer.isActive()) {
        return -1;
    }

    std::shared_ptr<SleepPrimitive> sleepPrimitive{ nullptr };
    {
        std::shared_ptr<Subscriber> subscriberSharedPtr = std::make_shared<SleepPrimitive>();
        std::weak_ptr<Subscriber> subscriberWeakPtr{ subscriberSharedPtr };
        if (!observer.addSubscriber(subscriberWeakPtr)) {
            return -1;
        }

        sleepPrimitive = std::dynamic_pointer_cast<SleepPrimitive>(subscriberSharedPtr);
        if (nullptr == sleepPrimitive) {
            std::cerr << "{main}; unable to cast subscriber shared pointer to "
                "sleep primitive shared pointer" << std::endl;
            return -1;
        }
    }

    auto differencePrinter = [] (
        const std::optional<StackSizeType>& prevStackSize,
        const std::optional<StackSizeType>& curStackSize,
        std::string_view spaceType
    ) -> bool {
        if (!curStackSize.has_value() || spaceType.empty()) {
            return false;
        }
        if (prevStackSize.has_value()) {
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

    ProcFilesystemReader reader;
    std::optional<StackSizeType> prevSumStackSize{ std::nullopt };
    std::optional<StackSizeType> prevKernelStackSize{ std::nullopt };
    while (true) {
        if (!reader.setup()) {
            return -1;
        }

        auto sumStackSize = reader.calcSumStackSize();
        if (!sumStackSize.has_value()) {
            return -1;
        }
        std::cout << "{main}; sum stack size: '" << sumStackSize.value() << " kilobyte(s)'" << std::endl;

        auto kernelStackSize = reader.getKernelStackSize();
        if (!kernelStackSize.has_value()) {
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

        auto sleepResult = sleepPrimitive->sleepFor(g_nMinutes);
        if ((SleepResult::ERROR == sleepResult) || (SleepResult::INTERRUPTED == sleepResult)) {
            break;
        }

        if (!reader.reset()) {
            return -1;
        }
    }
    return 0;
}
