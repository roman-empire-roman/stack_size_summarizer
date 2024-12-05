#ifndef SIGNAL_SAFE_MUTEX_H
#define SIGNAL_SAFE_MUTEX_H

#include <semaphore.h>

class SignalSafeMutex {
public:
    SignalSafeMutex();
    ~SignalSafeMutex();

    SignalSafeMutex(const SignalSafeMutex& other) = delete;
    SignalSafeMutex& operator=(const SignalSafeMutex& other) = delete;
    SignalSafeMutex(SignalSafeMutex&& other) = delete;
    SignalSafeMutex& operator=(SignalSafeMutex&& other) = delete;

    void lock();
    void unlock();

private:
    bool isValid() const { return m_isValid; }

private:
    sem_t m_semaphore = { .__size = { 0 } };
    bool m_isValid = false;
};

#endif // SIGNAL_SAFE_MUTEX_H
