#include "signal_handler_observer.h"

#include <chrono>
#include <csignal>
#include <iostream>

namespace {
    constexpr int g_nSeconds = 1;

    using SubscriberWeakPtr = SignalHandlerObserver::SubscriberWeakPtr;
}

bool SignalHandlerObserver::addSubscriber(const SubscriberWeakPtr& subscriberWeakPtr) {
    try {
        std::lock_guard<std::mutex> locker(m_observerMutex);
        m_subscribers.emplace_back(subscriberWeakPtr);
    } catch (const std::bad_alloc& exception) {
        std::cerr << "{SignalHandlerObserver::addSubscriber}; "
            "exception 'std::bad_alloc' was successfully caught while "
            "adding a weak subscriber pointer to the subscriber list; "
            "exception description: '" << exception.what() << "'" << std::endl;
        return false;
    } catch (...) {
        std::cerr << "{SignalHandlerObserver::addSubscriber}; "
            "unknown exception was caught while "
            "adding a weak subscriber pointer to the subscriber list" << std::endl;
        return false;
    }
    return true;
}

SignalHandlerObserver::SignalHandlerObserver() {
    if (SIG_ERR == std::signal(SIGINT, &SignalHandlerObserver::setSignalNumber)) {
        std::cerr << "{SignalHandlerObserver::SignalHandlerObserver}; "
            "unable to set signal handler 'SignalHandlerObserver::setSignalNumber' for "
            "signal 'SIGINT'" << std::endl;
        return;
    }
    std::cout << "{SignalHandlerObserver::SignalHandlerObserver}; "
        "signal handler 'SignalHandlerObserver::setSignalNumber' has been successfully set for "
        "signal 'SIGINT'" << std::endl;

    auto signalNumberChecker = [this] () {
        while (!m_readyToStop.load(std::memory_order_relaxed)) {
            auto signalNumber = SignalHandlerObserver::getSignalNumber();
            if (SIGINT == signalNumber) {
                {
                    std::lock_guard<std::mutex> locker(m_observerMutex);
                    for (const auto& subscriberWeakPtr : m_subscribers) {
                        auto subscriberSharedPtr = subscriberWeakPtr.lock();
                        if (nullptr == subscriberSharedPtr) {
                            continue;
                        }
                        subscriberSharedPtr->notify();
                    }
                }
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

    if (SIG_ERR == std::signal(SIGINT, SIG_DFL)) {
        std::cerr << "{SignalHandlerObserver::~SignalHandlerObserver}; "
            "unable to set default signal handler 'SIG_DFL' for "
            "signal 'SIGINT'" << std::endl;
        return;
    }
    std::cout << "{SignalHandlerObserver::~SignalHandlerObserver}; "
        "default signal handler 'SIG_DFL' has been successfully set for "
        "signal 'SIGINT'" << std::endl;

    {
        std::lock_guard<std::mutex> locker(m_observerMutex);
        m_subscribers.clear();
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
