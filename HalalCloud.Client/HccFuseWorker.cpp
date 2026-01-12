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

#include <thread>

namespace
{
    bool g_WorkerRunning = false;
    std::thread g_WorkerThread;

    std::string g_MountPoint;
    std::string g_VolumeName;
    fuse_operations g_Operations = {};
    fuse_args g_Arguments = FUSE_ARGS_INIT(0, nullptr);

    fuse_chan* g_Channel = nullptr;
    fuse* g_Instance = nullptr;
}

static void CleanupWorkerContext()
{
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

    buf->st_mode = S_IREAD | S_IWRITE;
    buf->st_mode |= S_IFDIR;

    return 0;
}

static int ReadDirectoryCallback(
    const char* path,
    void* buf,
    fuse_fill_dir_t filler,
    FUSE_OFF_T off,
    fuse_file_info* fi)
{
    MO_UNREFERENCED_PARAMETER(path);
    MO_UNREFERENCED_PARAMETER(buf);
    MO_UNREFERENCED_PARAMETER(filler);
    MO_UNREFERENCED_PARAMETER(off);
    MO_UNREFERENCED_PARAMETER(fi);

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
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
