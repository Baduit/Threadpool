#pragma once

#include <iostream>
#include <mutex>

void print()
{}

template <typename T>
void print(T&& a1)
{ std::cout << a1; }

template <typename T, typename... Args>
void print(T&& a1, Args&&... args)
{
    std::cout << a1;
    print(args...);
}

template <typename... Args>
void println(Args&&... args)
{
    print(args...);
    std::cout << std::endl;
}

template <typename... Args>
void safePrintln(Args&&... args)
{
    static std::mutex m;
    std::lock_guard<std::mutex> lk(m);
    println(args...);
}