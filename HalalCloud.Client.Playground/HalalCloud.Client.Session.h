/*
 * PROJECT:   Halal Cloud Client
 * FILE:      HalalCloud.Client.Session.h
 * PURPOSE:   Definition for the HalalCloud.Client.Session
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef HALALCLOUD_CLIENT_SESSION
#define HALALCLOUD_CLIENT_SESSION

#include <HccApi.h>
#include "../HalalCloud.Client/HccProtocolWrappers.h"

#include <Mile.Helpers.CppBase.h>

#include <Mile.Json.h>

#include "HalalCloud.Portable.Helpers.h"

#include <functional>
#include <vector>

namespace HalalCloud
{
    std::filesystem::path GetUserCloudCachePath(
        std::string_view UserIdentity);

    std::vector<std::uint8_t> ReadAllBytesFromFile(
        std::string_view FilePath);

    void WriteAllBytesToFile(
        std::string_view FilePath,
        std::vector<std::uint8_t> const& Bytes);

    struct FileInformation
    {
        std::int64_t CreationTime;
        std::int64_t LastWriteTime;
        std::int64_t FileSize;
        union
        {
            struct
            {
                std::uint64_t IsDirectory : 1;
                std::uint64_t IsHidden : 1;
            } Fields;
            std::uint64_t Value;
        } FileAttributes;
        std::string FileName;
    };

    namespace FileStorageType
    {
        enum
        {
            Unknown,
            Ipfs,
            BaiduObjectStorage
        };
    }

    struct FileStorageSizeRange
    {
    public:

        std::size_t StartBlockIndex;
        std::size_t EndBlockIndex;
        std::int64_t SingleBlockSize;

    public:

        bool IsInRange(
            std::size_t const& Index) const;

        std::size_t GetRangeBlockCount() const;

        std::int64_t GetRangeTotalSize() const;
    };

    struct FileStorageInformation
    {
    public:

        std::string Name;
        std::string Path;
        std::int64_t Size;
        std::uint32_t Type;
        std::string Identifier;
        std::vector<FileStorageSizeRange> SizeRanges;
        std::vector<std::string> Blocks;

    public:

        bool GetStartBlockIndex(
            std::size_t& StartBlockIndex,
            std::int64_t& StartBlockOffset,
            std::int64_t const& Offset) const;

        bool GetBlocks(
            std::vector<std::pair<std::string, std::int64_t>>& RequestedBlocks,
            std::int64_t& StartBlockOffset,
            std::int64_t const& Offset,
            std::uint32_t const& Length) const;
    };

    struct BlockStorageInformation
    {
        std::string Identifier;
        std::string DownloadLink;
        std::uint8_t EncryptionByte;
    };

    struct CachedBlockItem
    {
        MO_UINT8 HashBytes[HCC_SHA256_HASH_LENGTH];
        MO_BOOL HashExisted;
        MO_UINT32 DataSize; // Not implemented
        PMO_UINT8 DataBytes; // Not implemented
    };

    class Session : Mile::DisableCopyConstruction
    {
    private:

        FileInformation ToFileInformation(
            nlohmann::json const& Object);

        BlockStorageInformation ToBlockStorageInformation(
            nlohmann::json const& Object);

    private:

        std::mutex m_CachedBlocksMutex;
        std::map<std::string, CachedBlockItem> m_CachedBlocks;

    public:

        std::vector<std::uint8_t> AcquireBlock(
            BlockStorageInformation const& BlockInfo,
            std::int64_t const& ExpectedSize);

    private:

        nlohmann::json m_CurrentToken;

    public:

        HalalCloud::DownloadManager m_DownloadManager;

        nlohmann::json CurrentToken();

        nlohmann::json Request(
            std::string_view MethodFullName,
            nlohmann::json const& Request);

        void Authenticate(
            std::function<void(std::string_view)> Callback);

        void Impersonate(
            std::string_view RefreshToken);

        void Logout();

        nlohmann::json GetUserInformation();

        nlohmann::json CreateFolder(
            std::string_view Path);

        FileInformation GetFileInformation(
            std::string_view Path);

        std::vector<FileInformation> EnumerateFiles(
            std::string_view Path);

        void UploadFile(
            std::string_view SourceFilePath,
            std::string_view TargetFilePath);

        FileStorageInformation GetFileStorageInformation(
            std::string_view Path);

		std::vector<BlockStorageInformation> GetBlockStorageInformation(
			std::vector<std::string> const& Identifiers);

        void DownloadFile(
            std::string_view SourceFilePath,
            std::string_view TargetFilePath);
    };
}

#endif // !HALALCLOUD_CLIENT_SESSION
