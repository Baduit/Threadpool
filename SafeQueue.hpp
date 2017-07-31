#pragma once

#include <mutex>
#include <queue>
#include <future>
#include <condition_variable>

#include "CondVar.hpp"

template <typename T>
class SafeQueue
{
public:
    SafeQueue() = default;
    virtual ~SafeQueue() = default;

    void    push(T&& t)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(std::move(t));
        notifyAll();
    }

    bool    tryPop(T &t)
    {
        std::lock_guard<std::mutex> lock(_mutex);

        if (_queue.empty())
            return false;
        
        t = std::move(_queue.front());
        _queue.pop();
        return true;
    }

    bool    tryPop()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        if (_queue.empty())
            return false;
        
        _queue.pop();
        return true;
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.clear();
    }

    size_t  size()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.size();
    }

    bool    empty()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.empty();
    }
    
    auto&    getNotifier()
    { return  _notifier; }

    void    notifyAll()
    { _notifier.notifyAll(); }

private:
    std::queue<T>   _queue;
    std::mutex      _mutex;
    CondVar         _notifier;
};