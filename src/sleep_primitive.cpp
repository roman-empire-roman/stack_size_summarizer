#include "sleep_primitive.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

namespace {
    using SleepResult = SleepPrimitive::SleepResult;
}

SleepPrimitive::~SleepPrimitive() {
    {
        std::lock_guard<std::mutex> locker(m_sleepMutex);
        if (!m_isSleepInProgress) {
            return;
        }
        m_readyToInterrupt = true;
    }
    m_sleepCondVar.notify_one();
}

SleepResult SleepPrimitive::sleepFor(unsigned int nMinutes) {
    std::unique_lock<std::mutex> locker(m_sleepMutex);
    if (0 == nMinutes) {
        std::cerr << "{SleepPrimitive::sleepFor}; number of minutes is equal to zero" << std::endl;
        return SleepResult::ERROR;
    }

    if (m_isSleepInProgress) {
        std::cerr << "{SleepPrimitive::sleepFor}; sleep is already happening at the moment" << std::endl;
        return SleepResult::ERROR;
    }
    {
        // auto threadId = std::this_thread::get_id();
        // std::cout << "{SleepPrimitive::sleepFor}; sleeping of thread with "
        //    "id '" << threadId << "' was successfully started" << std::endl;
    }
    m_isSleepInProgress = true;
    auto isSleepInterrupted = m_sleepCondVar.wait_for(
        locker, std::chrono::minutes(nMinutes),
        [this] () {
            return m_readyToInterrupt;
        }
    );
    m_readyToInterrupt = false;
    if (!m_isSleepInProgress) {
        std::cerr << "{SleepPrimitive::sleepFor}; sleep progress flag has already been reset" << std::endl;
        return SleepResult::ERROR;
    }
    m_isSleepInProgress = false;

    {
        auto threadId = std::this_thread::get_id();
        if (isSleepInterrupted) {
            std::cout << std::endl;
            std::cout << "{SleepPrimitive::sleepFor}; sleeping of thread with "
                "id '" << threadId << "' was successfully interrupted" << std::endl;
            return SleepResult::INTERRUPTED;
        }
        // std::cout << "{SleepPrimitive::sleepFor}; sleeping of thread with "
        //    "id '" << threadId << "' was normally finished" << std::endl;
    }
    return SleepResult::FINISHED_NORMALLY;
}

void SleepPrimitive::notify() {
    interruptSleep();
}

bool SleepPrimitive::interruptSleep() {
    {
        std::lock_guard<std::mutex> locker(m_sleepMutex);
        if (!m_isSleepInProgress) {
            std::cerr << "{SleepPrimitive::interruptSleep}; sleep is NOT happening at the moment" << std::endl;
            return false;
        }
        if (m_readyToInterrupt) {
            std::cerr << "{SleepPrimitive::interruptSleep}; readiness for interruption was already set" << std::endl;
            return false;
        }
        m_readyToInterrupt = true;
    }
    m_sleepCondVar.notify_one();
    return true;
}
