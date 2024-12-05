#ifndef SLEEP_PRIMITIVE_H
#define SLEEP_PRIMITIVE_H

#include <condition_variable>
#include <mutex>

#include "subscriber.h"

class SleepPrimitive : public Subscriber {
public:
    enum class SleepResult {
        FINISHED_NORMALLY,
        INTERRUPTED,
        ERROR
    };

    SleepPrimitive() = default;
    SleepPrimitive(const SleepPrimitive& other) = delete;
    SleepPrimitive& operator=(const SleepPrimitive& other) = delete;
    ~SleepPrimitive();
    SleepPrimitive(SleepPrimitive&& other) = delete;
    SleepPrimitive& operator=(SleepPrimitive&& other) = delete;

    SleepResult sleepFor(unsigned int nMinutes);
    void notify() override;

private:
    bool interruptSleep();

private:
    std::mutex m_sleepMutex;
    std::condition_variable m_sleepCondVar;
    bool m_readyToInterrupt = false;
    bool m_isSleepInProgress = false;
};

#endif // SLEEP_PRIMITIVE_H
