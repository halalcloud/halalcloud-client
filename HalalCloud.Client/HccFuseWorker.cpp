/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HccFuseWorker.cpp
 * PURPOSE:    Implementation for Halal Cloud Client FUSE Worker
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "HccFuseWorker.h"

#include <fuse.h>

#include <filesystem>
#include <map>
#include <mutex>
#include <stack>
#include <thread>

namespace
{
    static std::mutex g_FileInformationCacheMutex;
    static std::map<std::string, HalalCloud::FileList> g_FileInformationCache;

    static bool IsFileInformationCacheInitialized()
    {
        // Empty cache means uninitialized because at least the root path a.k.a
        // "/" should be cached.
        return !g_FileInformationCache.empty();
    }

    static void UninitializeFileInformationCache()
    {
        if (!::IsFileInformationCacheInitialized())
        {
            return;
        }
        std::lock_guard<std::mutex> Lock(g_FileInformationCacheMutex);
        g_FileInformationCache.clear();
    }

    static void InitializeFileInformationCache()
    {
        if (::IsFileInformationCacheInitialized())
        {
            return;
        }

        std::lock_guard<std::mutex> Lock(g_FileInformationCacheMutex);

        g_FileInformationCache.clear();

        std::stack<std::string> PathStack;
        PathStack.push("/");

        HalalCloud::GlobalConfigurations& Configurations =
            HalalCloud::GetGlobalConfigurations();
        HalalCloud::UserToken& CurrentToken = Configurations.CurrentToken;

        while (!PathStack.empty())
        {
            std::string CurrentPath = PathStack.top();
            PathStack.pop();
            HalalCloud::FileList Candidate = {};
            try
            {
                Candidate = HalalCloud::GetFileList(
                    CurrentToken,
                    CurrentPath);
            }
            catch (...)
            {
                continue;
            }
            g_FileInformationCache[CurrentPath] = Candidate;
            for (auto const& Item : Candidate)
            {
                if (Item.FileAttributes.Fields.IsDirectory)
                {
                    std::filesystem::path ChildPathObject =
                        std::filesystem::path(CurrentPath) / Item.FileName;
                    std::string ChildPath = HalalCloud::PathToUtf8String(
                        ChildPathObject,
                        true);
                    PathStack.push(ChildPath);
                }
            }
        }
    }
}

namespace
{
    static bool g_WorkerRunning = false;
    static std::thread g_WorkerThread;

    static std::string g_MountPoint;
    static std::string g_VolumeName;
    static fuse_operations g_Operations = {};
    static fuse_args g_Arguments = FUSE_ARGS_INIT(0, nullptr);

    static fuse_chan* g_Channel = nullptr;
    static fuse* g_Instance = nullptr;
}

static void CleanupWorkerContext()
{
    ::UninitializeFileInformationCache();

    if (g_Channel)
    {
        ::fuse_unmount(g_MountPoint.c_str(), g_Channel);
        g_Channel = nullptr;
    }

    if (g_Instance)
    {
        ::fuse_destroy(g_Instance);
        g_Instance = nullptr;
    }

    ::fuse_opt_free_args(&g_Arguments);
    g_Arguments = FUSE_ARGS_INIT(0, nullptr);

    g_Operations = {};

    g_VolumeName.clear();

    g_MountPoint.clear();

    g_WorkerRunning = false;
}

static void FuseWorker()
{
    g_Channel = ::fuse_mount(g_MountPoint.c_str(), &g_Arguments);
    if (g_Channel)
    {
        g_Instance = ::fuse_new(
            g_Channel,
            &g_Arguments,
            &g_Operations,
            sizeof(g_Operations),
            nullptr); // Should be session pointer
        if (g_Instance)
        {
            ::fuse_loop_mt(g_Instance);
        }
    }
    ::CleanupWorkerContext();
}

static int OpenCallback(
    const char* path,
    fuse_file_info* fi)
{
    MO_UNREFERENCED_PARAMETER(path);
    MO_UNREFERENCED_PARAMETER(fi);

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
    {
        return -EINVAL;
    }

    return 0;
}

static int ReadCallback(
    const char* path,
    char* buf,
    size_t size,
    FUSE_OFF_T offset,
    fuse_file_info* fi)
{
    MO_UNREFERENCED_PARAMETER(path);
    MO_UNREFERENCED_PARAMETER(buf);
    MO_UNREFERENCED_PARAMETER(size);
    MO_UNREFERENCED_PARAMETER(offset);
    MO_UNREFERENCED_PARAMETER(fi);

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
    {
        return -EINVAL;
    }

    return 0; // size
}

static int StatFsCallback(
    const char* path,
    struct statvfs* buf)
{
    MO_UNREFERENCED_PARAMETER(path);
    MO_UNREFERENCED_PARAMETER(buf);

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
    {
        return -EINVAL;
    }

    try
    {
        HalalCloud::GlobalConfigurations& Configurations =
            HalalCloud::GetGlobalConfigurations();
        HalalCloud::UserToken& CurrentToken = Configurations.CurrentToken;

        HalalCloud::UserStatistics Statistics =
            HalalCloud::GetUserStatistics(CurrentToken);

        std::memset(buf, 0, sizeof(statvfs));
        buf->f_bsize = 512;
        buf->f_blocks = Statistics.BytesQuota / buf->f_bsize;
        buf->f_bfree = Statistics.BytesUsed / buf->f_bsize;
        buf->f_bavail = buf->f_blocks - buf->f_bfree;
    }
    catch (...)
    {
        return -EINVAL;
    }

    return 0;
}

static void ToFuseFileStatistics(
    FUSE_STAT* Output,
    HalalCloud::FileInformation const& Input)
{
    Output->st_mode = S_IREAD | S_IWRITE;
    Output->st_mode |= Input.FileAttributes.Fields.IsDirectory
        ? S_IFDIR
        : S_IFREG;
    if (!Input.FileAttributes.Fields.IsDirectory)
    {
        Output->st_size = Input.FileSize;
    }
    Output->st_atim.tv_sec = Input.LastWriteTime / 1000;
    Output->st_mtim.tv_sec = Input.LastWriteTime / 1000;
    Output->st_ctim.tv_sec = Input.LastWriteTime / 1000;
    Output->st_birthtim.tv_sec = Input.CreationTime / 1000;
}

static int GetAttributesCallback(
    const char* path,
    FUSE_STAT* buf)
{
    MO_UNREFERENCED_PARAMETER(path);

    if (!buf)
    {
        return -EINVAL;
    }
    std::memset(buf, 0, sizeof(FUSE_STAT));

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
    {
        return -EINVAL;
    }

    std::string NormalizedPath = HalalCloud::PathToUtf8String(
        HalalCloud::PathFromUtf8String(path).lexically_normal(),
        true);

    if ("/" == NormalizedPath)
    {
        buf->st_mode = S_IREAD | S_IWRITE | S_IFDIR;
        return 0;
    }

    try
    {
        std::lock_guard<std::mutex> Lock(g_FileInformationCacheMutex);

        std::filesystem::path NormalizedPathObject =
            HalalCloud::PathFromUtf8String(NormalizedPath);
        std::string DirectoryPath = HalalCloud::PathToUtf8String(
            NormalizedPathObject.parent_path());
        std::string FileName = HalalCloud::PathToUtf8String(
            NormalizedPathObject.filename());

        auto Iterator = g_FileInformationCache.find(DirectoryPath);
        if (Iterator == g_FileInformationCache.end())
        {
            // Directory is not found.
            return -ENOENT;
        }

        HalalCloud::FileInformation Information = {};

        bool Available = false;
        for (auto const& Item : Iterator->second)
        {
            if (FileName == Item.FileName)
            {
                Information = Item;
                Available = true;
                break;
            }
        }
        if (!Available)
        {
            // File is not found.
            return -ENOENT;
        }

        ::ToFuseFileStatistics(buf, Information);
    }
    catch (...)
    {
        return -EINVAL;
    }

    return 0;
}

static int ReadDirectoryCallback(
    const char* path,
    void* buf,
    fuse_fill_dir_t filler,
    FUSE_OFF_T off,
    fuse_file_info* fi)
{
    MO_UNREFERENCED_PARAMETER(off);
    MO_UNREFERENCED_PARAMETER(fi);

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
    {
        return -EINVAL;
    }

    std::string NormalizedPath = HalalCloud::PathToUtf8String(
        HalalCloud::PathFromUtf8String(path).lexically_normal(),
        true);

    try
    {
        std::lock_guard<std::mutex> Lock(g_FileInformationCacheMutex);

        auto Iterator = g_FileInformationCache.find(NormalizedPath);
        if (Iterator == g_FileInformationCache.end())
        {
            // Directory is not found.
            return -ENOENT;
        }

        for (auto const& Item : Iterator->second)
        {
            FUSE_STAT stbuf = {};
            ::ToFuseFileStatistics(&stbuf, Item);
            if (filler(buf, Item.FileName.c_str(), &stbuf, 0))
            {
                break;
            }
        }
    }
    catch (...)
    {
        return -EINVAL;
    }

    return 0;
}

static void* InitializeCallback(
    fuse_conn_info* conn)
{
    UNREFERENCED_PARAMETER(conn);

    fuse_context* Context = ::fuse_get_context();
    if (Context)
    {
        return Context->private_data;
    }

    return nullptr;
}

void HalalCloud::StartFuseWorker()
{
    if (g_WorkerRunning)
    {
        return;
    }
    g_WorkerRunning = true;

    ::InitializeFileInformationCache();

    {
        HalalCloud::GlobalConfigurations& Configurations =
            HalalCloud::GetGlobalConfigurations();

        g_MountPoint = Configurations.MountPoint;

        HalalCloud::UserToken& CurrentToken = Configurations.CurrentToken;
        if (CurrentToken.Validate())
        {
            HalalCloud::UserInformation Information =
                HalalCloud::GetUserInformation(CurrentToken);
            g_VolumeName = Information.Name;
        }
    }

    g_Operations.open = ::OpenCallback;
    g_Operations.read = ::ReadCallback;
    g_Operations.statfs = ::StatFsCallback;
    g_Operations.getattr = ::GetAttributesCallback;
    g_Operations.readdir = ::ReadDirectoryCallback;
    g_Operations.init = ::InitializeCallback;

    ::fuse_opt_add_arg(
        &g_Arguments,
        "HalalCloud");
    ::fuse_opt_add_arg(
        &g_Arguments,
        "-odaemon_timeout=86400");
    ::fuse_opt_add_arg(
        &g_Arguments,
        (std::string("-ovolname=") + g_VolumeName).c_str());

    g_WorkerThread = std::thread(FuseWorker);
}

void HalalCloud::StopFuseWorker()
{
    if (!g_WorkerRunning)
    {
        return;
    }

    ::fuse_exit(g_Instance);

    if (g_WorkerThread.joinable())
    {
        g_WorkerThread.join();
    }

    ::CleanupWorkerContext();
}
