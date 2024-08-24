#include "signal_handler_observer.h"

#include <csignal>
#include <iostream>
#include <mutex>

#include "sleep_primitive.h"

namespace {
    constexpr int g_nSeconds = 1;
}

SignalHandlerObserver::SignalHandlerObserver() {
    if (SIG_ERR == std::signal(SIGINT, &SignalHandlerObserver::setSignalNumber)) {
        std::cerr << "{SignalHandlerObserver::SignalHandlerObserver}; "
            "unable to set signal handler for signal 'SIGINT'" << std::endl;
        return;
    }

    auto signalNumberChecker = [this] () {
        while (!m_readyToStop.load(std::memory_order_relaxed)) {
            auto signalNumber = SignalHandlerObserver::getSignalNumber();
            if (SIGINT == signalNumber) {
                auto& primitive = SleepPrimitive::getInstance();
                primitive.interruptSleep();
                SignalHandlerObserver::setSignalNumber(0);
                break;
            } else if (0 != signalNumber) {
                std::cerr << "{SignalHandlerObserver::SignalHandlerObserver}; "
                    "{signalNumberChecker}; unexpected signal number '" << signalNumber << "' was set" << std::endl;
                SignalHandlerObserver::setSignalNumber(0);
            }
            std::this_thread::sleep_for(
                std::chrono::seconds(g_nSeconds)
            );
        }
    };
    m_thread = std::thread(signalNumberChecker);
}

SignalHandlerObserver::~SignalHandlerObserver() {
    if (m_thread.joinable()) {
        m_readyToStop.store(true, std::memory_order_relaxed);
        m_thread.join();
    }
}

void SignalHandlerObserver::setSignalNumber(int signalNumber) {
    std::lock_guard<SignalSafeMutex> locker(s_signalMutex);
    s_signalNumber = signalNumber;
}

int SignalHandlerObserver::getSignalNumber() {
    std::lock_guard<SignalSafeMutex> locker(s_signalMutex);
    return s_signalNumber;
}
