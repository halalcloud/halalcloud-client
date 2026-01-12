/*
 * PROJECT:   Halal Cloud Client
 * FILE:      HalalCloud.Client.Playground.cpp
 * PURPOSE:   Implementation for Halal Cloud Client Playground
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include <clocale>
#include <cstdio>
#include <cwchar>

#include <Windows.h>

#include <Mile.Helpers.CppBase.h>

#include <Mile.Json.h>

#include "HalalCloud.Client.Session.h"

#include <time.h>

#include <fuse.h>

int HccFuseOpenCallback(
    const char* path,
    fuse_file_info* fi)
{
    UNREFERENCED_PARAMETER(fi);

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
    {
        return -EINVAL;
    }

    HalalCloud::Session* Session =
        static_cast<HalalCloud::Session*>(Context->private_data);
    if (!Session)
    {
        return -EINVAL;
    }

    try
    {
        Session->GetFileStorageInformation(path);
    }
    catch (...)
    {
        return -EINVAL;
    }

    return 0;
}

int HccFuseReadCallback(
    const char* path,
    char* buf,
    size_t size,
    FUSE_OFF_T offset,
    fuse_file_info* fi)
{
    UNREFERENCED_PARAMETER(offset);
    UNREFERENCED_PARAMETER(fi);

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
    {
        return -EINVAL;
    }

    HalalCloud::Session* Session =
        static_cast<HalalCloud::Session*>(Context->private_data);
    if (!Session)
    {
        return -EINVAL;
    }

    std::size_t ProceededSize = 0;
    try
    {
        HalalCloud::FileStorageInformation StorageInformation =
            Session->GetFileStorageInformation(path);
        if (offset >= StorageInformation.Size)
        {
            return 0;
        }
        std::uint32_t RequestedSize = static_cast<std::uint32_t>(size);
        if (offset + RequestedSize > static_cast<std::int64_t>(StorageInformation.Size))
        {
            RequestedSize = static_cast<std::uint32_t>(
                StorageInformation.Size - offset);
        }

        std::vector<std::pair<std::string, std::int64_t>> RequestedBlocks;
        std::int64_t StartBlockOffset = 0;
        if (!StorageInformation.GetBlocks(
            RequestedBlocks,
            StartBlockOffset,
            static_cast<std::int64_t>(offset),
            RequestedSize))
        {
            return -EINVAL;
        }

        std::vector<std::string> Identifiers;
        for (auto const& Block : RequestedBlocks)
        {
            Identifiers.push_back(Block.first);
        }

        std::vector<HalalCloud::BlockStorageInformation> BlocksInfo =
            Session->GetBlockStorageInformation(Identifiers);

        for (size_t i = 0; i < RequestedBlocks.size(); ++i)
        {
            if (RequestedBlocks[i].first != BlocksInfo[i].Identifier)
            {
                return -EINVAL;
            }

            std::vector<std::uint8_t> Bytes = Session->AcquireBlock(
                BlocksInfo[i],
                RequestedBlocks[i].second);
            if (Bytes.empty())
            {
                return -EINVAL;
            }

            std::uint8_t* CopyStart = Bytes.data();
            std::size_t CopyMaximumSize = Bytes.size();
            if (!ProceededSize)
            {
                CopyStart += StartBlockOffset;
                CopyMaximumSize -= StartBlockOffset;
            }
            std::size_t CopySize = CopyMaximumSize;
            if (ProceededSize + CopySize > size)
            {
                CopySize = size - ProceededSize;
            }

            std::memcpy(
                buf + ProceededSize,
                CopyStart,
                CopySize);

            ProceededSize += CopySize;
        }
    }
    catch (...)
    {
        return -EINVAL;
    }

    return static_cast<int>(ProceededSize);
}

int HccFuseStatFsCallback(
    const char* path,
    struct statvfs* buf)
{
    UNREFERENCED_PARAMETER(path);

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
    {
        return -EINVAL;
    }

    HalalCloud::Session* Session =
        static_cast<HalalCloud::Session*>(Context->private_data);
    if (!Session)
    {
        return -EINVAL;
    }

    try
    {
        HalalCloud::UserStatistics Statistics =
            HalalCloud::GetUserStatistics(Session->CurrentToken());

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

int HccFuseGetAttributesCallback(
    const char* path,
    FUSE_STAT* buf)
{
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

    HalalCloud::Session* Session =
        static_cast<HalalCloud::Session*>(Context->private_data);
    if (!Session)
    {
        return -EINVAL;
    }

    try
    {
        HalalCloud::FileInformation File = Session->GetFileInformation(path);

        buf->st_mode = S_IREAD | S_IWRITE;
        buf->st_mode |=
            File.FileAttributes.Fields.IsDirectory ? S_IFDIR : S_IFREG;
        if (!File.FileAttributes.Fields.IsDirectory)
        {
            buf->st_size = File.FileSize;
        }
        buf->st_atim.tv_sec = File.LastWriteTime / 1000;
        buf->st_mtim.tv_sec = File.LastWriteTime / 1000;
        buf->st_ctim.tv_sec = File.LastWriteTime / 1000;
        buf->st_birthtim.tv_sec = File.CreationTime / 1000;
    }
    catch (...)
    {
        return -EINVAL;
    }

    return 0;
}

int HccFuseReadDirectoryCallback(
    const char* path,
    void* buf,
    fuse_fill_dir_t filler,
    FUSE_OFF_T off,
    fuse_file_info* fi)
{
    UNREFERENCED_PARAMETER(off);
    UNREFERENCED_PARAMETER(fi);

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
    {
        return -EINVAL;
    }

    HalalCloud::Session* Session =
        static_cast<HalalCloud::Session*>(Context->private_data);
    if (!Session)
    {
        return -EINVAL;
    }

    std::vector<HalalCloud::FileInformation> Files =
        Session->EnumerateFiles(path);
    for (HalalCloud::FileInformation const& File : Files)
    {
        FUSE_STAT stbuf = {};
        stbuf.st_mode = S_IREAD | S_IWRITE;
        stbuf.st_mode |=
            File.FileAttributes.Fields.IsDirectory ? S_IFDIR : S_IFREG;
        if (!File.FileAttributes.Fields.IsDirectory)
        {
            stbuf.st_size = File.FileSize;
        }
        stbuf.st_atim.tv_sec = File.LastWriteTime / 1000;
        stbuf.st_mtim.tv_sec = File.LastWriteTime / 1000;
        stbuf.st_ctim.tv_sec = File.LastWriteTime / 1000;
        stbuf.st_birthtim.tv_sec = File.CreationTime / 1000;
        if (filler(buf, File.FileName.c_str(), &stbuf, 0))
        {
            break;
        }
    }

    return 0;
}

void* HccFuseInitializeCallback(
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

void TextUserInterfaceMain();

int main()
{
    std::printf(
        "ApplicationDataRootPath = \"%s\"\n",
        HalalCloud::GetApplicationDataRootPath().string().data());

    HalalCloud::Session Session;

    Session.Authenticate([](
        std::string_view AuthenticationUri)
    {
        std::printf(
            "AuthenticationUri = \"%s\"\n",
            AuthenticationUri.data());

        std::wstring ConvertedReturnUrl =
            Mile::ToWideString(CP_UTF8, AuthenticationUri);

        SHELLEXECUTEINFOW ExecInfo = {};
        ExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
        ExecInfo.lpVerb = L"open";
        ExecInfo.lpFile = ConvertedReturnUrl.c_str();
        ExecInfo.nShow = SW_SHOWNORMAL;
        ::ShellExecuteExW(&ExecInfo);

        std::printf("Waiting...\n");
    });

    std::printf("Login Success!\n");

    HalalCloud::UserToken CurrentToken = Session.CurrentToken();
    std::printf(
        "Current Access Token = \"%s\"\n"
        "Current Refresh Token = \"%s\"\n",
        CurrentToken.AccessToken.c_str(),
        CurrentToken.RefreshToken.c_str());

    //Session.DownloadFile("/Civil.War.2024.2160p.WEB-DL.DDP5.1.Atmos.DV.HDR.H.265-FLUX[TGx]/Civil.War.2024.2160p.WEB-DL.DDP5.1.Atmos.DV.HDR.H.265-FLUX.mkv", "D:\\Civil.War.2024.2160p.WEB-DL.DDP5.1.Atmos.DV.HDR.H.265-FLUX.mkv");

    /*Session.Impersonate(Mile::Json::ToString(
        Mile::Json::GetSubKey(Session.CurrentToken(), "refresh_token")));

    std::printf("Refresh Success!\n");

    std::printf(
        "Token = \"%s\"\n",
        Session.CurrentToken().dump(2).c_str());

    Session.UploadFile(
        "D:\\Updates\\9p.cap",
        "/9p20240925.cap");

    Session.CreateFolder("/Test20240923");

    std::vector<HalalCloud::FileInformation> Files =
        Session.EnumerateFiles("/");
    for(HalalCloud::FileInformation const& File : Files)
    {
        std::time_t Time = File.CreationTime / 1000;
        std::tm TimeInfo = {};
        ::localtime_s(&TimeInfo, &Time);
        std::printf(
            "%04d/%02d/%02d %02d:%02d\t%s\t%lld\t%s\n",
            TimeInfo.tm_year + 1900,
            TimeInfo.tm_mon + 1,
            TimeInfo.tm_mday,
            TimeInfo.tm_hour,
            TimeInfo.tm_min,
            File.FileAttributes.Fields.IsDirectory ? "<DIR>" : "",
            File.FileSize,
            File.FileName.c_str());
    }*/

    std::string MountPoint = "z:";

    fuse_args Arguments = FUSE_ARGS_INIT(0, nullptr);
    ::fuse_opt_add_arg(&Arguments, "HalalCloud");
    ::fuse_opt_add_arg(&Arguments, "-ovolname=HalalCloud");
    ::fuse_opt_add_arg(&Arguments, "-odaemon_timeout=86400");

    fuse_operations Operations = {};
    Operations.open = ::HccFuseOpenCallback;
    Operations.read = ::HccFuseReadCallback;
    Operations.statfs = ::HccFuseStatFsCallback;
    Operations.getattr = ::HccFuseGetAttributesCallback;
    Operations.readdir = ::HccFuseReadDirectoryCallback;
    Operations.init = ::HccFuseInitializeCallback;

    fuse_chan* Channel = ::fuse_mount(MountPoint.c_str(), &Arguments);
    if (Channel)
    {
        fuse* Instance = ::fuse_new(
            Channel,
            &Arguments,
            &Operations,
            sizeof(Operations),
            &Session);
        if (Instance)
        {
            int Result = ::fuse_loop_mt(Instance);
            if (0 != Result)
            {
                std::printf("fuse_loop_mt failed!\n");
            }

            ::fuse_unmount(MountPoint.c_str(), Channel);
            ::fuse_destroy(Instance);
        }
        else
        {
            std::printf("fuse_new failed!\n");
        }
    }
    else
    {
        std::printf("fuse_mount failed!\n");
    }

    /*auto Yolo = Session.GetFileStorageInformation("/9p.cap");
    Yolo = Yolo;*/

    Session.Logout();

    std::printf("Logout Success!\n");

    std::printf(
        "================================================================\n"
        "HalalCloud.Client.Playground has been completed.\n");
    std::getchar();

    return 0;
}
