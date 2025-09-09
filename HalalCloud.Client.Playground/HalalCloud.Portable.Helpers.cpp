/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HalalCloud.Portable.Helpers.cpp
 * PURPOSE:    Implementation for Portable Helpers
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "HalalCloud.Portable.Helpers.h"

HalalCloud::ThreadPool::ThreadPool(
    std::size_t const& MaximumWorkerCount)
{
    for (size_t i = 0; i < MaximumWorkerCount; ++i)
    {
        this->m_Workers.emplace_back([this]()
        {
            for (;;)
            {
                std::function<void()> Task;
                {
                    std::unique_lock<std::mutex> Lock(this->m_TaskQueueMutex);
                    this->m_TaskQueueCondition.wait(
                        Lock,
                        [this]()
                    {
                        return !this->m_Running || !this->m_TaskQueue.empty();
                    });
                    if (!this->m_Running && this->m_TaskQueue.empty())
                    {
                        return;
                    }
                    Task = std::move(this->m_TaskQueue.front());
                    this->m_TaskQueue.pop();
                }
                Task();
            }
        });
    }
}

HalalCloud::ThreadPool::ThreadPool() :
    ThreadPool(std::thread::hardware_concurrency())
{
}

HalalCloud::ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> Lock(this->m_TaskQueueMutex);
        this->m_Running = false;
    }
    this->m_TaskQueueCondition.notify_all();
    for (std::thread& Worker : this->m_Workers)
    {
        if (Worker.joinable())
        {
            Worker.join();
        }
    }
}

void HalalCloud::ThreadPool::Enqueue(
    std::function<void()> const& Task)
{
    {
        std::unique_lock<std::mutex> Lock(this->m_TaskQueueMutex);
        if (!this->m_Running)
        {
            throw std::runtime_error(
                "Unable to enqueue a task to a stopped thread pool.");
        }
        this->m_TaskQueue.push(Task);
    }
    this->m_TaskQueueCondition.notify_one();
}
