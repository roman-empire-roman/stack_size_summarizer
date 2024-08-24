#ifndef SIGNAL_HANDLER_OBSERVER_H
#define SIGNAL_HANDLER_OBSERVER_H

#include <atomic>
#include <thread>

#include "signal_safe_mutex.h"

class SignalHandlerObserver {
    SignalHandlerObserver();

public:
    static SignalHandlerObserver& getInstance() {
        static SignalHandlerObserver observer;
        return observer;
    }

    SignalHandlerObserver(const SignalHandlerObserver& other) = delete;
    SignalHandlerObserver& operator=(const SignalHandlerObserver& other) = delete;
    ~SignalHandlerObserver();
    SignalHandlerObserver(SignalHandlerObserver&& other) = delete;
    SignalHandlerObserver& operator=(SignalHandlerObserver&& other) = delete;

private:
    static void setSignalNumber(int signalNumber);
    static int getSignalNumber();

private:
    std::thread m_thread;
    std::atomic<bool> m_readyToStop{ false };

    static inline SignalSafeMutex s_signalMutex;
    static inline int s_signalNumber = 0;
};

#endif // SIGNAL_HANDLER_OBSERVER_H
