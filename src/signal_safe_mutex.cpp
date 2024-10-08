#include "signal_safe_mutex.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

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
        if (0 != errno) {
            auto errorDescriptionPtr = std::strerror(errno);
            if (nullptr == errorDescriptionPtr) {
                return;
            }

            auto descriptionLength = std::strlen(errorDescriptionPtr);
            std::string errorDescription(errorDescriptionPtr, descriptionLength);
            if (errorDescription.empty()) {
                return;
            }
            std::cerr << "{SignalSafeMutex::SignalSafeMutex}; "
                "error '" << errno << "' occurred during initialization of semaphore "
                "by using of function 'sem_init'; "
                "error description: '" << errorDescription << "'" << std::endl;
        }
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
        if (0 != errno) {
            auto errorDescriptionPtr = std::strerror(errno);
            if (nullptr == errorDescriptionPtr) {
                return;
            }

            auto descriptionLength = std::strlen(errorDescriptionPtr);
            std::string errorDescription(errorDescriptionPtr, descriptionLength);
            if (errorDescription.empty()) {
                return;
            }
            std::cerr << "{SignalSafeMutex::~SignalSafeMutex}; "
                "error '" << errno << "' occurred during destruction of semaphore "
                "by using of function 'sem_destroy'; "
                "error description: '" << errorDescription << "'" << std::endl;
        }
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
        if (0 != errno) {
            auto errorDescriptionPtr = std::strerror(errno);
            if (nullptr == errorDescriptionPtr) {
                return;
            }

            auto descriptionLength = std::strlen(errorDescriptionPtr);
            std::string errorDescription(errorDescriptionPtr, descriptionLength);
            if (errorDescription.empty()) {
                return;
            }
            std::cerr << "{SignalSafeMutex::lock}; "
                "error '" << errno << "' occurred during decrement of semaphore value "
                "by using of function 'sem_wait'; "
                "error description: '" << errorDescription << "'" << std::endl;
        }
    }
}

void SignalSafeMutex::unlock() {
    if (!isValid()) {
        return;
    }

    auto postResult = sem_post(&m_semaphore);
    if (-1 == postResult) {
        if (0 != errno) {
            auto errorDescriptionPtr = std::strerror(errno);
            if (nullptr == errorDescriptionPtr) {
                return;
            }

            auto descriptionLength = std::strlen(errorDescriptionPtr);
            std::string errorDescription(errorDescriptionPtr, descriptionLength);
            if (errorDescription.empty()) {
                return;
            }
            std::cerr << "{SignalSafeMutex::unlock}; "
                "error '" << errno << "' occurred during increment of semaphore value "
                "by using of function 'sem_post'; "
                "error description: '" << errorDescription << "'" << std::endl;
        }
    }
}
