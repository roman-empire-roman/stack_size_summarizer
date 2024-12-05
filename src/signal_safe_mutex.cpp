#include "signal_safe_mutex.h"

#include <iostream>

#include "common_functions.h"

namespace {
    enum SharedType {
        BETWEEN_THREADS = 0,
        BETWEEN_PROCESSES
    };

    constexpr int g_nMaxThreads = 1;
}

SignalSafeMutex::SignalSafeMutex() {
    auto initResult = sem_init(
        &m_semaphore, static_cast<int>(SharedType::BETWEEN_THREADS), g_nMaxThreads
    );
    if (-1 == initResult) {
        std::cerr << "{SignalSafeMutex::SignalSafeMutex}; "
            "error occurred during initialization of semaphore "
            "by using of function 'sem_init'" << std::endl;
        CommonFunctions::printSemaphoreError();
    } else {
        m_isValid = true;
    }
}

SignalSafeMutex::~SignalSafeMutex() {
    if (!isValid()) {
        return;
    }

    auto destroyResult = sem_destroy(&m_semaphore);
    if (-1 == destroyResult) {
        std::cerr << "{SignalSafeMutex::~SignalSafeMutex}; "
            "error occurred during destruction of semaphore "
            "by using of function 'sem_destroy'" << std::endl;
        CommonFunctions::printSemaphoreError();
    } else {
        m_isValid = false;
    }
}

void SignalSafeMutex::lock() {
    if (!isValid()) {
        return;
    }

    auto waitResult = sem_wait(&m_semaphore);
    if (-1 == waitResult) {
        std::cerr << "{SignalSafeMutex::lock}; "
            "error occurred during decrement of semaphore value "
            "by using of function 'sem_wait'" << std::endl;
        CommonFunctions::printSemaphoreError();
    }
}

void SignalSafeMutex::unlock() {
    if (!isValid()) {
        return;
    }

    auto postResult = sem_post(&m_semaphore);
    if (-1 == postResult) {
        std::cerr << "{SignalSafeMutex::unlock}; "
            "error occurred during increment of semaphore value "
            "by using of function 'sem_post'" << std::endl;
        CommonFunctions::printSemaphoreError();
    }
}
