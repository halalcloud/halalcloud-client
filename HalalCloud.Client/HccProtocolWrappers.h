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

    std::string Request(
        std::string_view AccessToken,
        std::string_view MethodFullName,
        std::string_view RequestJson);

    std::string GenerateCodeVerifier();

    void Authorize(
        std::string& Code,
        std::string& RedirectUri,
        std::string_view CodeVerifier);

    enum class AuthorizeState
    {
        PendingLogin,
        PendingConfirmation,
        TokenCreated,
        Success,
        Failed
    };

    AuthorizeState GetAuthorizeState(
        std::string_view Code);

    std::string GetToken(
        std::string_view Code,
        std::string_view CodeVerifier);
}

#endif // !HALALCLOUD_CLIENT_PROTOCOL_WRAPPERS
