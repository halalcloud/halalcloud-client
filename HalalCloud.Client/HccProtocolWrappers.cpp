/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HccProtocolWrappers.cpp
 * PURPOSE:    Implementation for Halal Cloud Client Protocol Wrappers
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "HccProtocolWrappers.h"

#include <HccApi.h>
#include <Mile.Json.h>

#include <Mile.Helpers.CppBase.h>

#ifdef _WIN32
#include <ShlObj.h>
#endif

#include <stdexcept>

[[noreturn]] void HalalCloud::ThrowException(
    std::string_view Checkpoint,
    std::int32_t const& Code)
{
    throw std::runtime_error(Mile::FormatString(
        "[HalalCloud.Client] %s failed with code %d.",
        Checkpoint.data(),
        Code));
}

std::filesystem::path HalalCloud::GetApplicationDataRootPath()
{
    static std::filesystem::path CachedResult = ([]() -> std::filesystem::path
    {
#ifdef _WIN32
        std::filesystem::path Data = std::filesystem::path();
        {
            LPWSTR RawPath = nullptr;
            // KF_FLAG_FORCE_APP_DATA_REDIRECTION, when engaged, causes
            // SHGetKnownFolderPath to return the new AppModel paths
            // (Packages/xxx/RoamingState, etc.) for standard path requests.
            // Using this flag allows us to avoid
            // Windows.Storage.ApplicationData completely.
            if (SUCCEEDED(::SHGetKnownFolderPath(
                FOLDERID_LocalAppData,
                KF_FLAG_FORCE_APP_DATA_REDIRECTION,
                nullptr,
                &RawPath)))
            {
                Data = std::filesystem::path(RawPath);
                ::CoTaskMemFree(RawPath);
            }
            else
            {
                Data = std::filesystem::path(
                    std::getenv("USERPROFILE"));
                if (Data.empty() ||
                    !Data.is_absolute() ||
                    !std::filesystem::exists(Data))
                {
                    Data = std::filesystem::path(
                        std::getenv("SYSTEMDRIVE"));
                    if (Data.empty() ||
                        !Data.is_absolute() ||
                        !std::filesystem::exists(Data))
                    {
                        Data = std::filesystem::path("C:");
                        if (Data.empty() ||
                            !Data.is_absolute() ||
                            !std::filesystem::exists(Data))
                        {
                            Data = std::filesystem::path("X:");
                        }
                    }
                }
                Data /= ".local";
                Data /= "share";
            }
        }
#else
        std::filesystem::path Data = std::filesystem::path(
            std::getenv("XDG_DATA_HOME"));
        if (Data.empty() ||
            !Data.is_absolute() ||
            !std::filesystem::exists(Data))
        {
            Data = std::filesystem::path(
                std::getenv("HOME"));
            if (Data.empty() ||
                !Data.is_absolute() ||
                !std::filesystem::exists(Data))
            {
                Data = std::filesystem::path("/");
            }
            Data /= ".local";
            Data /= "share";
        }
#endif
        Data /= "HalalCloud";
        return Data;
    }());

    if (!std::filesystem::exists(CachedResult))
    {
        std::filesystem::create_directories(CachedResult);
    }
    return CachedResult;
}

std::filesystem::path HalalCloud::GetBlocksCachePath()
{
    std::filesystem::path Result = HalalCloud::GetApplicationDataRootPath();
    Result /= "BlocksCache";
    if (!std::filesystem::exists(Result))
    {
        std::filesystem::create_directories(Result);
    }
    return Result;
}

std::string HalalCloud::Request(
    std::string_view AccessToken,
    std::string_view MethodFullName,
    std::string_view RequestJson)
{
    MO_STRING RawResponseJson = nullptr;
    HCC_RPC_STATUS Status = ::HccRpcPostRequest(
        &RawResponseJson,
        AccessToken.data(),
        MethodFullName.data(),
        RequestJson.data());
    if (HCC_RPC_STATUS_OK != Status)
    {
        HalalCloud::ThrowException(
            "HalalCloud::Request!HccRpcPostRequest",
            Status);
    }
    std::string ResponseJson = std::string(RawResponseJson);
    ::HccFreeMemory(RawResponseJson);
    return ResponseJson;
}

std::string HalalCloud::GenerateCodeVerifier()
{
    std::string CodeVerifier;
    {
        MO_UINT8 CodeVerifierBytes[32] = {};
        if (MO_RESULT_SUCCESS_OK == ::HccGenerateRandomBytes(
            CodeVerifierBytes,
            sizeof(CodeVerifierBytes)))
        {
            MO_STRING RawCodeVerifier = nullptr;
            if (MO_RESULT_SUCCESS_OK == ::HccEncodeBase64UrlSafe(
                &RawCodeVerifier,
                CodeVerifierBytes,
                sizeof(CodeVerifierBytes)))
            {
                CodeVerifier = std::string(RawCodeVerifier);
                ::HccFreeMemory(RawCodeVerifier);
            }
        }
    }
    return CodeVerifier;
}

void HalalCloud::Authorize(
    std::string& Code,
    std::string& RedirectUri,
    std::string_view CodeVerifier)
{
    Code.clear();
    RedirectUri.clear();
    std::string CodeChallenge;
    {
        MO_UINT8 CodeChallengeBytes[HCC_SHA256_HASH_LENGTH] = {};
        if (MO_RESULT_SUCCESS_OK == ::HccComputeSha256(
            CodeChallengeBytes,
            CodeVerifier.data(),
            static_cast<MO_UINT32>(CodeVerifier.size())))
        {
            MO_STRING RawCodeChallenge = nullptr;
            if (MO_RESULT_SUCCESS_OK == ::HccEncodeBase64UrlSafe(
                &RawCodeChallenge,
                CodeChallengeBytes,
                sizeof(CodeChallengeBytes)))
            {
                CodeChallenge = std::string(RawCodeChallenge);
                ::HccFreeMemory(RawCodeChallenge);
            }
        }
    }
    nlohmann::json Request = nlohmann::json();
    Request["client_id"] = "puc_5_dcpqfdvu8qnt_v1";
    Request["code_challenge"] = CodeChallenge;
    Request["code_challenge_method"] = "S256";
    Request["legacy"] = true;
    nlohmann::json Response = nlohmann::json::parse(HalalCloud::Request(
        "*",
        "/v6/oauth/authorize",
        Request.dump()));
    Code = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "code"));
    RedirectUri = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "redirect_uri"));
}

HalalCloud::AuthorizeState HalalCloud::GetAuthorizeState(
    std::string_view Code)
{
    nlohmann::json Request = nlohmann::json();
    Request["code"] = Code;
    nlohmann::json Response = nlohmann::json::parse(HalalCloud::Request(
        "*",
        "/v6/oauth/get_authorize_state",
        Request.dump()));
    std::string Status = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "status"));
    if ("AUTHORIZATION_PENDING_LOGIN" == Status)
    {
        return HalalCloud::AuthorizeState::PendingLogin;
    }
    else if ("AUTHORIZATION_PENDING_CONFIRMATION" == Status)
    {
        return HalalCloud::AuthorizeState::PendingConfirmation;
    }
    else if ("AUTHORIZATION_TOKEN_CREATED" == Status)
    {
        return HalalCloud::AuthorizeState::TokenCreated;
    }
    else if ("AUTHORIZATION_SUCCESS" == Status)
    {
        return HalalCloud::AuthorizeState::Success;
    }

    return HalalCloud::AuthorizeState::Failed;
}

std::string HalalCloud::GetToken(
    std::string_view Code,
    std::string_view CodeVerifier)
{
    nlohmann::json Request = nlohmann::json();
    Request["code"] = Code;
    Request["code_verifier"] = CodeVerifier;
    return HalalCloud::Request(
        "*",
        "/v6/oauth/get_token",
        Request.dump());
}
