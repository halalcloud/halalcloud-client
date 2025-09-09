/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HalalCloud.Portable.Helpers.h
 * PURPOSE:    Definition for Portable Helpers
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef HALALCLOUD_PORTABLE_HELPERS
#define HALALCLOUD_PORTABLE_HELPERS

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace HalalCloud
{
    class ThreadPool
    {
    private:

        std::vector<std::thread> m_Workers;
        std::queue<std::function<void()>> m_TaskQueue;
        std::mutex m_TaskQueueMutex;
        std::condition_variable m_TaskQueueCondition;
        std::atomic_bool m_Running = true;

    public:

        ThreadPool(
            std::size_t const& MaximumWorkerCount);

        ThreadPool();

        ~ThreadPool();

        void Enqueue(
            std::function<void()> const& Task);
    };
}

#endif // !HALALCLOUD_PORTABLE_HELPERS
