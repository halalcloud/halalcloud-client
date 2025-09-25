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

#include <HccApi.h>

HalalCloud::DownloadManager::~DownloadManager()
{
    {
        std::unique_lock<std::mutex> Lock(this->m_Mutex);
        for (auto& Task : this->m_Tasks)
        {
            // Cancel all waiting tasks via marking them as failed.
            if (DownloadTaskStatus::Waiting == Task.second.Status)
            {
                Task.second.Status = DownloadTaskStatus::Failed;
            }
        }
    }
    this->m_Condition.notify_all();
    // Then, wait for all running tasks to complete via the destructor of
    // the thread pool.
}

void HalalCloud::DownloadManager::Clear()
{
    std::unique_lock<std::mutex> Lock(this->m_Mutex);
    this->m_Tasks.clear();
}

void HalalCloud::DownloadManager::Add(
    std::string const& TaskId,
    std::string const& SourceUrl,
    std::string const& TargetPath)
{
    std::unique_lock<std::mutex> Lock(this->m_Mutex);
    if (this->m_Tasks.contains(TaskId))
    {
        throw std::runtime_error("Task already exists.");
    }
    HalalCloud::DownloadTask NewTask;
    NewTask.Status = HalalCloud::DownloadTaskStatus::Waiting;
    NewTask.Source = SourceUrl;
    NewTask.Target = TargetPath;
    this->m_Tasks.emplace(TaskId, NewTask);
    this->m_Workers.Enqueue([this, TaskId]()
    {
        std::string SourceUrl;
        std::string TargetPath;
        {
            std::unique_lock<std::mutex> Lock(this->m_Mutex);
            if (!this->m_Tasks.contains(TaskId))
            {
                // The task has been removed.
                return;
            }
            this->m_Tasks[TaskId].Status =
                HalalCloud::DownloadTaskStatus::Running;
            SourceUrl = this->m_Tasks[TaskId].Source;
            TargetPath = this->m_Tasks[TaskId].Target;
        }

        bool Success = (MO_RESULT_SUCCESS_OK == ::HccDownloadFile(
            SourceUrl.c_str(),
            TargetPath.c_str()));

        {
            std::unique_lock<std::mutex> Lock(this->m_Mutex);
            if (!this->m_Tasks.contains(TaskId))
            {
                // The task has been removed.
                return;
            }
            this->m_Tasks[TaskId].Status = static_cast<std::uint8_t>(
                Success
                ? HalalCloud::DownloadTaskStatus::Ready
                : HalalCloud::DownloadTaskStatus::Failed);
        }
        this->m_Condition.notify_all();
    });
}

bool HalalCloud::DownloadManager::Wait(
    std::string const& TaskId)
{
    std::unique_lock<std::mutex> Lock(this->m_Mutex);
    if (!this->m_Tasks.contains(TaskId))
    {
        throw std::runtime_error("Task not found.");
    }
    this->m_Condition.wait(
        Lock,
        [this, TaskId]()
    {
        std::uint8_t Status = this->m_Tasks[TaskId].Status;
        return (
            HalalCloud::DownloadTaskStatus::Ready == Status ||
            HalalCloud::DownloadTaskStatus::Failed == Status);
    });
    std::uint8_t Status = this->m_Tasks[TaskId].Status;
    return HalalCloud::DownloadTaskStatus::Ready == Status;
}

void HalalCloud::DownloadManager::Remove(
    std::string const& TaskId)
{
    std::unique_lock<std::mutex> Lock(this->m_Mutex);
    if (!this->m_Tasks.contains(TaskId))
    {
        throw std::runtime_error("Task not found.");
    }
    this->m_Tasks.erase(TaskId);
}

std::uint8_t HalalCloud::DownloadManager::GetStatus(
    std::string const& TaskId)
{
    std::unique_lock<std::mutex> Lock(this->m_Mutex);
    if (!this->m_Tasks.contains(TaskId))
    {
        throw std::runtime_error("Task not found.");
    }
    return this->m_Tasks[TaskId].Status;
}

std::string HalalCloud::DownloadManager::GetSource(
    std::string const& TaskId)
{
    std::unique_lock<std::mutex> Lock(this->m_Mutex);
    if (!this->m_Tasks.contains(TaskId))
    {
        throw std::runtime_error("Task not found.");
    }
    return this->m_Tasks[TaskId].Source;
}

std::string HalalCloud::DownloadManager::GetTarget(
    std::string const& TaskId)
{
    std::unique_lock<std::mutex> Lock(this->m_Mutex);
    if (!this->m_Tasks.contains(TaskId))
    {
        throw std::runtime_error("Task not found.");
    }
    return this->m_Tasks[TaskId].Target;
}
