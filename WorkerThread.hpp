#pragma once

#include <thread>
#include <future>
#include <atomic>
#include <condition_variable>

#include "CondVar.hpp"

template <template <class> class TaskQueue, class Callback>
class WorkerThread
{
public:
    WorkerThread(TaskQueue<Callback>& q, std::atomic<bool>& run) :
        _taskQueue(q), _run(run), _notification(_taskQueue.getNotifier())
    {
        _thread = std::thread([this]() {
            Callback cb;

            while (_run)
            {
                if (_taskQueue.empty())
                    _notification.wait();

                if (_run && _taskQueue.tryPop(cb))
                    cb();
            }
        });
    }

    // copy
    WorkerThread(const WorkerThread&) = delete;
    WorkerThread& operator=(const WorkerThread&) = delete;

    //move
    WorkerThread& operator=(WorkerThread&&) = default;
    WorkerThread(WorkerThread&&) = default;

    virtual ~WorkerThread()
    {
        if (!_stopped)
            quit();
    }

    void quit()
    {
        _stopped = true;
        _thread.join();
    }

private:
    TaskQueue<Callback>&                _taskQueue;
    std::atomic<bool>&                  _run;
    bool                                _stopped = false;

    std::thread                         _thread;
    decltype(_taskQueue.getNotifier())  _notification;
};