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

#include <Mile.Helpers.CppBase.h>
#include <HalalCloud.RpcClient.h>

#include <Mile.Json.h>

#include <functional>
#include <string_view>
#include <vector>

namespace HalalCloud
{
    [[noreturn]] void ThrowExceptionWithHResult(
        _In_ LPCSTR Checkpoint,
        _In_ HRESULT Value);

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

    struct FileStorageNode
    {
        std::int64_t Offset;
        std::int64_t Size;
        std::string Identifier;
    };

    struct FileStorageInformation
    {
        std::string Identifier;
        std::int64_t Size;
        std::string Path;
        std::uint32_t Type;
        std::vector<FileStorageNode> Nodes;
    };

    class Session : Mile::DisableCopyConstruction
    {
    private:

        HCC_RPC_SESSION m_Session = nullptr;
        nlohmann::json m_CurrentToken;

        void ApplyAccessToken(
            nlohmann::json const& Token);

    public:

        Session();

        ~Session();

        HCC_RPC_SESSION NativeHandle();

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

        std::vector<FileInformation> EnumerateFiles(
            std::string_view Path);

        void UploadFile(
            std::string_view SourceFilePath,
            std::string_view TargetFilePath);

        FileStorageInformation GetFileStorageInformation(
            std::string_view Path);
    };
}

#endif // !HALALCLOUD_CLIENT_SESSION
