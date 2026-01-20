/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HccProtocolWrappers.h
 * PURPOSE:    Definition for Halal Cloud Client Protocol Wrappers
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef HALALCLOUD_CLIENT_PROTOCOL_WRAPPERS
#define HALALCLOUD_CLIENT_PROTOCOL_WRAPPERS

#include <HccApi.h>

#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace HalalCloud
{
    [[noreturn]] void ThrowException(
        std::string_view Checkpoint,
        std::int32_t const& Code);

    std::string PathToUtf8String(
        std::filesystem::path const& Path);

    std::filesystem::path EnsureDirectoryPathExists(
        std::filesystem::path const& Path);

    std::filesystem::path GetApplicationDataRootPath();

    std::filesystem::path GetBlocksCachePath();

    std::filesystem::path GetProfilesRootPath();

    std::filesystem::path GetProfilePath(
        std::string_view ProfileName);
}

namespace HalalCloud
{
    enum class AuthorizeState
    {
        PendingLogin,
        PendingConfirmation,
        TokenCreated,
        Success,
        Failed
    };

    struct UserToken
    {
    public:

        std::string AccessToken;
        std::string RefreshToken;

    public:

        void Clear();

        bool Validate();

        void Parse(
            std::string_view JsonString);

    public:

        UserToken() = default;

        UserToken(
            std::string_view JsonString);
    };

    struct UserInformation
    {
        std::string Identity;
        std::string Name;
        std::string Icon;
    };

    struct UserStatistics
    {
        std::uint64_t BytesQuota = 0;
        std::uint64_t BytesUsed = 0;
    };

    struct FileInformation
    {
    public:

        std::int64_t CreationTime = 0;
        std::int64_t LastWriteTime = 0;
        std::int64_t FileSize = 0;
        union
        {
            struct
            {
                std::uint64_t IsDirectory : 1;
                std::uint64_t IsHidden : 1;
            } Fields;
            std::uint64_t Value = 0;
        } FileAttributes;
        std::string FileName;

    public:

        void Parse(
            std::string_view JsonString);

    public:

        FileInformation() = default;

        FileInformation(
            std::string_view JsonString);
    };

    using FileList = std::vector<FileInformation>;

    using FileDictionary = std::map<std::string, FileInformation>;
}

namespace HalalCloud
{
    std::string RequestWithoutToken(
        std::string_view MethodFullName,
        std::string_view RequestJson);

    std::string GenerateCodeVerifier();

    void Authorize(
        std::string& Code,
        std::string& RedirectUri,
        std::string_view CodeVerifier);

    AuthorizeState GetAuthorizeState(
        std::string_view Code);

    UserToken GetToken(
        std::string_view Code,
        std::string_view CodeVerifier);

    UserToken RefreshToken(
        std::string_view RefreshToken);
}

namespace HalalCloud
{
    struct GlobalConfigurations
    {
        std::string CurrentProfile = "Default";
        UserToken CurrentToken;
        std::string MountPoint;
    };

    GlobalConfigurations& GetGlobalConfigurations();

    void LoadGlobalConfigurations();

    void SaveGlobalConfigurations();
}

namespace HalalCloud
{
    std::string Request(
        UserToken& Token,
        std::string_view MethodFullName,
        std::string_view RequestJson);

    void Logoff(
        UserToken& Token);

    UserInformation GetUserInformation(
        UserToken& Token);

    UserStatistics GetUserStatistics(
        UserToken& Token);

    FileInformation GetFileInformation(
        UserToken& Token,
        std::string_view Path);

    FileList GetFileList(
        UserToken& Token,
        std::string_view Path);

    void AppendFileList(
        FileDictionary& Dictionary,
        UserToken& Token,
        std::string_view Path);
}

#endif // !HALALCLOUD_CLIENT_PROTOCOL_WRAPPERS
