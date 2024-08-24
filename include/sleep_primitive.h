#ifndef SLEEP_PRIMITIVE_H
#define SLEEP_PRIMITIVE_H

#include <condition_variable>
#include <mutex>

class SleepPrimitive {
public:
    static SleepPrimitive& getInstance() {
        static SleepPrimitive primitive;
        return primitive;
    }

    SleepPrimitive(const SleepPrimitive& other) = delete;
    SleepPrimitive& operator=(const SleepPrimitive& other) = delete;
    ~SleepPrimitive() = default;
    SleepPrimitive(SleepPrimitive&& other) = delete;
    SleepPrimitive& operator=(SleepPrimitive&& other) = delete;

    bool sleepFor(int nMinutes);
    void interruptSleep();

private:
    SleepPrimitive() = default;

private:
    std::mutex m_sleepMutex;
    std::condition_variable m_sleepCondVar;
    bool m_readyToInterrupt = false;
};

#endif // SLEEP_PRIMITIVE_H
