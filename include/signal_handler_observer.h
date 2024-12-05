#ifndef SIGNAL_HANDLER_OBSERVER_H
#define SIGNAL_HANDLER_OBSERVER_H

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

#include "signal_safe_mutex.h"
#include "subscriber.h"

class SignalHandlerObserver {
public:
    static SignalHandlerObserver& getInstance() {
        static SignalHandlerObserver observer;
        return observer;
    }

    using SubscriberWeakPtr = Subscriber::SubscriberWeakPtr;
    bool addSubscriber(const SubscriberWeakPtr& subscriberWeakPtr);

    bool isActive() const { return m_thread.joinable(); }

private:
    SignalHandlerObserver();
    SignalHandlerObserver(const SignalHandlerObserver& other) = delete;
    SignalHandlerObserver& operator=(const SignalHandlerObserver& other) = delete;
    ~SignalHandlerObserver();
    SignalHandlerObserver(SignalHandlerObserver&& other) = delete;
    SignalHandlerObserver& operator=(SignalHandlerObserver&& other) = delete;

    static void setSignalNumber(int signalNumber);
    static int getSignalNumber();

private:
    std::thread m_thread;
    std::atomic<bool> m_readyToStop{ false };

    static inline SignalSafeMutex s_signalMutex;
    static inline int s_signalNumber = 0;

    std::mutex m_observerMutex;
    std::vector<SubscriberWeakPtr> m_subscribers;
};

#endif // SIGNAL_HANDLER_OBSERVER_H
