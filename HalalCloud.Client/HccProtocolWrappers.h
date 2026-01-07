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
#include <string>
#include <string_view>

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

    std::string Request(
        UserToken& Token,
        std::string_view MethodFullName,
        std::string_view RequestJson);

    void Logoff(
        UserToken& Token);

    UserInformation GetUserInformation(
        UserToken& Token);
}

namespace HalalCloud
{
    struct GlobalConfigurations
    {
        std::string CurrentProfile = "Default";
        UserToken CurrentToken;
    };

    GlobalConfigurations& GetGlobalConfigurations();

    void LoadGlobalConfigurations();

    void SaveGlobalConfigurations();
}

#endif // !HALALCLOUD_CLIENT_PROTOCOL_WRAPPERS
