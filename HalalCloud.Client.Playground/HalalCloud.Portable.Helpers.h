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
#include <map>
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

    namespace DownloadTaskStatus
    {
        enum
        {
            Waiting = 0,
            Running = 1,
            Ready = 2,
            Failed = 3,
        };
    }

    struct DownloadTask
    {
        std::atomic_uint8_t Status = DownloadTaskStatus::Waiting;
        std::string Source;
        std::string Hash;
        std::string Target;
    };

    class DownloadManager
    {
    private:

        std::mutex m_Mutex;
        std::condition_variable m_Condition;
        std::map<std::string, DownloadTask> m_Tasks;
        ThreadPool m_Workers;

    public:

        DownloadManager() = default;

        ~DownloadManager() = default;

        void Add(
            std::string const& TaskId,
            std::string const& SourceUrl,
            std::string const& TargetPath,
            std::string const& Sha256Hash = std::string());

        bool Wait(
            std::string const& TaskId);

        DownloadTask Query(
            std::string const& TaskId);
    };
}

#endif // !HALALCLOUD_PORTABLE_HELPERS
