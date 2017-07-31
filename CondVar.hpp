#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std::chrono_literals;

class CondVar
{
public:
    void wait()
    {
        std::unique_lock<std::mutex> lk(_m);
        _notifier.wait_for(lk, 100ms); // little security, not really an elegant way to do it but it works
    }

    void notifyOne()
    { _notifier.notify_one(); }

    void notifyAll()
    { _notifier.notify_all(); }

private:
    std::mutex                  _m;
    std::condition_variable     _notifier;
};