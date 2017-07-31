#pragma once

#include <memory>

class Task
{
    struct IPlaceHolder
    {
        virtual ~IPlaceHolder() = default;
        virtual void operator()() = 0;
    };

    template <typename T>
    struct PlaceHolder : public IPlaceHolder
    {
        PlaceHolder(T&& t): c(std::move(t)) {}

        virtual void operator()()
        { c(); }

        T c;
    };

public:
    Task() = default;

    // copie
    Task(const Task&) = delete;
    Task(Task&&) = default;

    //move
    Task& operator=(const Task&) = delete;
    Task& operator=(Task&&) = default;

    // assignment
    template <typename T>
    Task(T&& t) : _placeHolder(std::make_unique<PlaceHolder<T>>(std::move(t))) {}

    template <typename T>
    Task& operator=(T&& t)
    { _placeHolder = std::make_unique<PlaceHolder<T>>(std::move(t)); }

    // call
    void operator()()
    {
        if (_placeHolder)
            (*_placeHolder)();
    }

private:
    std::unique_ptr<IPlaceHolder> _placeHolder= nullptr;
};