#pragma once

#include <vector>
#include <atomic>
#include <functional>
#include <future>
#include <memory>

#include "SafeQueue.hpp"
#include "Task.hpp"
#include "WorkerThread.hpp"

using Thread = WorkerThread<SafeQueue, Task>;
using ThreadPtr = std::unique_ptr<Thread>;
using Threads = std::vector<ThreadPtr>;

using TaskQueue = SafeQueue<Task>;

class ThreadPool
{
public:
    explicit ThreadPool(size_t numberOfThread = 5)
    {
        for (size_t i = 0; i < 5; ++i)
            addThread();
    }

    virtual ~ThreadPool()
    { stop(); }

    void stop()
    {
        if (_run)
        {
            _run = false;
            _taskQueue.notifyAll();
            _threads.clear();
        }
    }

    size_t size() const
    { return _threads.size(); }

    void resize(size_t n)
    {
        if (n > size())
            while (n > size())
                addThread();
        else if (n < size())
        {
            while (n < size())
                _threads.pop_back();
        }
    }

    void addThread()
    { _threads.emplace_back(std::make_unique<Thread>(_taskQueue, _run)); }

    template <typename F, typename... Args>
    auto push(F&& function, Args&&... args)
    {
        auto bindedTask = std::bind(std::forward<F>(function), std::forward<Args>(args)...);
        
        using ResultType = std::result_of_t<decltype(bindedTask)()>;

        std::packaged_task<ResultType()> task { std::move(bindedTask) };
        std::future<ResultType> result = task.get_future();
        _taskQueue.push(std::move(task));
        return result;
    }

private:
    Threads             _threads;
    TaskQueue           _taskQueue;
    std::atomic<bool>   _run { true };
};