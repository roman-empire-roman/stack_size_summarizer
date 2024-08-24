#include "sleep_primitive.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

bool SleepPrimitive::sleepFor(int nMinutes) {
    std::unique_lock<std::mutex> locker(m_sleepMutex);
    if (nMinutes <= 0) {
        std::cerr << "{SleepPrimitive::sleepFor}; number of minutes is less than or equal to zero" << std::endl;
        return false;
    }

    m_readyToInterrupt = false;
    {
        // auto threadId = std::this_thread::get_id();
        // std::cout << "{SleepPrimitive::sleepFor}; sleeping of thread with "
        //    "id '" << threadId << "' was successfully started" << std::endl;
    }
    auto sleepPredicate = [this] () {
        return m_readyToInterrupt;
    };

    auto isSleepInterrupted = m_sleepCondVar.wait_for(
        locker, std::chrono::minutes(nMinutes), sleepPredicate
    );
    {
        auto threadId = std::this_thread::get_id();
        if (isSleepInterrupted) {
            std::cout << std::endl;
            std::cout << "{SleepPrimitive::sleepFor}; sleeping of thread with "
                "id '" << threadId << "' was successfully interrupted" << std::endl;
            return false;
        }
        // std::cout << "{SleepPrimitive::sleepFor}; sleeping of thread with "
        //    "id '" << threadId << "' was successfully finished" << std::endl;
    }
    return true;
}

void SleepPrimitive::interruptSleep() {
    {
        std::lock_guard<std::mutex> locker(m_sleepMutex);
        m_readyToInterrupt = true;
    }
    m_sleepCondVar.notify_all();
}
