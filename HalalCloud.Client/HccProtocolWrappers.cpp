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

std::string HalalCloud::PathToUtf8String(
    std::filesystem::path const& Path)
{
#ifdef _WIN32
    return Mile::ToString(CP_UTF8, Path.wstring());
#else
    return Path.string();
#endif
}

std::filesystem::path HalalCloud::EnsureDirectoryPathExists(
    std::filesystem::path const& Path)
{
    if (!std::filesystem::exists(Path))
    {
        std::filesystem::create_directories(Path);
    }
    return Path;
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
    return HalalCloud::EnsureDirectoryPathExists(CachedResult);
}

std::filesystem::path HalalCloud::GetBlocksCachePath()
{
    static std::filesystem::path CachedResult = ([]() -> std::filesystem::path
    {
        return HalalCloud::GetApplicationDataRootPath() / "BlocksCache";
    }());
    return HalalCloud::EnsureDirectoryPathExists(CachedResult);
}

std::filesystem::path HalalCloud::GetProfilesRootPath()
{
    static std::filesystem::path CachedResult = ([]() -> std::filesystem::path
    {
        return HalalCloud::GetApplicationDataRootPath() / "Profiles";
    }());
    return HalalCloud::EnsureDirectoryPathExists(CachedResult);
}

std::filesystem::path HalalCloud::GetProfilePath(
    std::string_view ProfileName)
{
    std::filesystem::path ProfilePath = HalalCloud::GetProfilesRootPath();
    ProfilePath /= ProfileName;
    return HalalCloud::EnsureDirectoryPathExists(ProfilePath);
}

////////////////////////////////////////////////////////////////////////////////

void HalalCloud::UserToken::Clear()
{
    this->AccessToken.clear();
    this->RefreshToken.clear();
}

bool HalalCloud::UserToken::Validate()
{
    bool AccessTokenValid = !this->AccessToken.empty();
    bool RefreshTokenValid = !this->RefreshToken.empty();
    if (AccessTokenValid != RefreshTokenValid)
    {
        // Invalidate the token if it's not valid.
        this->Clear();
    }
    return AccessTokenValid && RefreshTokenValid;
}

void HalalCloud::UserToken::Parse(
    std::string_view JsonString)
{
    nlohmann::json Object = nlohmann::json::parse(JsonString);
    this->AccessToken = Mile::Json::ToString(
        Mile::Json::GetSubKey(Object, "access_token"));
    this->RefreshToken = Mile::Json::ToString(
        Mile::Json::GetSubKey(Object, "refresh_token"));
    this->Validate();
}

HalalCloud::UserToken::UserToken(
    std::string_view JsonString)
{
    this->Parse(JsonString);
}

void HalalCloud::FileInformation::Parse(
    std::string_view JsonString)
{
    nlohmann::json Object = nlohmann::json::parse(JsonString);
    this->CreationTime = Mile::ToInt64(Mile::Json::ToString(
        Mile::Json::GetSubKey(Object, "create_ts")));
    this->LastWriteTime = Mile::ToInt64(Mile::Json::ToString(
        Mile::Json::GetSubKey(Object, "update_ts")));
    this->FileSize = Mile::ToInt64(Mile::Json::ToString(
        Mile::Json::GetSubKey(Object, "size")));
    this->FileAttributes.Fields.IsDirectory = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Object, "dir"));
    this->FileAttributes.Fields.IsHidden = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Object, "hidden"));
    this->FileName = Mile::Json::ToString(
        Mile::Json::GetSubKey(Object, "name"));
}

HalalCloud::FileInformation::FileInformation(
    std::string_view JsonString)
{
    this->Parse(JsonString);
}

////////////////////////////////////////////////////////////////////////////////

std::string HalalCloud::RequestWithoutToken(
    std::string_view MethodFullName,
    std::string_view RequestJson)
{
    MO_STRING RawResponseJson = nullptr;
    HCC_RPC_STATUS Status = ::HccRpcPostRequest(
        &RawResponseJson,
        "*",
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
    nlohmann::json Response = nlohmann::json::parse(
        HalalCloud::RequestWithoutToken(
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
    nlohmann::json Response = nlohmann::json::parse(
        HalalCloud::RequestWithoutToken(
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

HalalCloud::UserToken HalalCloud::GetToken(
    std::string_view Code,
    std::string_view CodeVerifier)
{
    nlohmann::json Request = nlohmann::json();
    Request["code"] = Code;
    Request["code_verifier"] = CodeVerifier;
    return HalalCloud::UserToken(HalalCloud::RequestWithoutToken(
        "/v6/oauth/get_token",
        Request.dump()));
}

HalalCloud::UserToken HalalCloud::RefreshToken(
    std::string_view RefreshToken)
{
    nlohmann::json Request = nlohmann::json();
    Request["refresh_token"] = RefreshToken;
    return HalalCloud::UserToken(HalalCloud::RequestWithoutToken(
        "/v6/oauth/refresh_token",
        Request.dump()));
}

////////////////////////////////////////////////////////////////////////////////

HalalCloud::GlobalConfigurations& HalalCloud::GetGlobalConfigurations()
{
    static HalalCloud::GlobalConfigurations CachedConfigurations = {};
    return CachedConfigurations;
}

void HalalCloud::LoadGlobalConfigurations()
{
    HalalCloud::GlobalConfigurations& Configurations =
        HalalCloud::GetGlobalConfigurations();
    std::filesystem::path CurrentProfilePath =
        HalalCloud::GetProfilePath(Configurations.CurrentProfile);
    std::filesystem::path ConfigurationsFilePath =
        CurrentProfilePath / "Profile.json";

    PMO_UINT8 ContentBuffer = nullptr;
    MO_UINT32 ContentSize = 0;
    if (MO_RESULT_SUCCESS_OK != ::HccReadAllBytesFromFile(
        &ContentBuffer,
        &ContentSize,
        HalalCloud::PathToUtf8String(ConfigurationsFilePath).c_str()))
    {
        return;
    }
    std::string Content(
        reinterpret_cast<char const*>(ContentBuffer),
        static_cast<std::size_t>(ContentSize));
    ::HccFreeMemory(ContentBuffer);

    nlohmann::json Object;
    try
    {
        Object = nlohmann::json::parse(Content);
    }
    catch (...)
    {

    }
    Configurations.CurrentToken.RefreshToken = Mile::Json::ToString(
        Mile::Json::GetSubKey(Object, "RefreshToken"));
    Configurations.MountPoint = Mile::Json::ToString(
        Mile::Json::GetSubKey(Object, "MountPoint"),
        "Z:");
}

void HalalCloud::SaveGlobalConfigurations()
{
    HalalCloud::GlobalConfigurations& Configurations =
        HalalCloud::GetGlobalConfigurations();
    std::filesystem::path CurrentProfilePath =
        HalalCloud::GetProfilePath(Configurations.CurrentProfile);
    std::filesystem::path ConfigurationsFilePath =
        CurrentProfilePath / "Profile.json";

    nlohmann::json Object = nlohmann::json::object();
    if (Configurations.CurrentToken.Validate())
    {
        Object["RefreshToken"] = Configurations.CurrentToken.RefreshToken;
    }
    Object["MountPoint"] = Configurations.MountPoint;
    std::string Content = Object.dump(2);

    ::HccWriteAllBytesToFile(
        HalalCloud::PathToUtf8String(ConfigurationsFilePath).c_str(),
        reinterpret_cast<PMO_UINT8>(const_cast<char*>(Content.data())),
        static_cast<MO_UINT32>(Content.size()));
}

////////////////////////////////////////////////////////////////////////////////

std::string HalalCloud::Request(
    UserToken& Token,
    std::string_view MethodFullName,
    std::string_view RequestJson)
{
    MO_STRING RawResponseJson = nullptr;
    HCC_RPC_STATUS Status = ::HccRpcPostRequest(
        &RawResponseJson,
        Token.AccessToken.data(),
        MethodFullName.data(),
        RequestJson.data());
    if (HCC_RPC_STATUS_UNAUTHENTICATED == Status)
    {
        // Try to refresh the token and retry.
        Token = HalalCloud::RefreshToken(Token.RefreshToken);
        // Save the new token to the local storage.
        HalalCloud::SaveGlobalConfigurations();
        // Retry the request.
        Status = ::HccRpcPostRequest(
            &RawResponseJson,
            Token.AccessToken.data(),
            MethodFullName.data(),
            RequestJson.data());
    }
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

void HalalCloud::Logoff(
    HalalCloud::UserToken& Token)
{
    if (Token.Validate())
    {
        nlohmann::json Request = nlohmann::json();
        Request["access_token"] = Token.AccessToken;
        Request["refresh_token"] = Token.RefreshToken;
        HalalCloud::Request(
            Token,
            "/v6/user/logoff",
            Request.dump());
        Token.Clear();
    }
}

HalalCloud::UserInformation HalalCloud::GetUserInformation(
    HalalCloud::UserToken& Token)
{
    nlohmann::json Response = nlohmann::json::parse(HalalCloud::Request(
        Token,
        "/v6/user/get",
        "{}"));
    HalalCloud::UserInformation Information;
    Information.Identity = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "identity"));
    Information.Name = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "name"));
    Information.Icon = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "icon"));
    return Information;
}

HalalCloud::UserStatistics HalalCloud::GetUserStatistics(
    HalalCloud::UserToken& Token)
{
    nlohmann::json Response = nlohmann::json::parse(HalalCloud::Request(
        Token,
        "/v6/user/get_statistics_and_quota",
        "{}"));
    HalalCloud::UserStatistics Statistics;
    nlohmann::json DiskStatisticsQuota =
        Mile::Json::GetSubKey(Response, "disk_statistics_quota");
    Statistics.BytesQuota = Mile::ToUInt64(Mile::Json::ToString(
        Mile::Json::GetSubKey(DiskStatisticsQuota, "bytes_quota")));
    Statistics.BytesUsed = Mile::ToUInt64(Mile::Json::ToString(
        Mile::Json::GetSubKey(DiskStatisticsQuota, "bytes_used")));
    return Statistics;
}

HalalCloud::FileInformation HalalCloud::GetFileInformation(
    HalalCloud::UserToken& Token,
    std::string_view Path)
{
    nlohmann::json Request;
    Request["path"] = Path;
    return HalalCloud::FileInformation(HalalCloud::Request(
        Token,
        "/v6/userfile/get",
        Request.dump()));
}

HalalCloud::FileList HalalCloud::GetFileList(
    HalalCloud::UserToken& Token,
    std::string_view Path)
{
    HalalCloud::FileList Result;

    nlohmann::json Request;
    Request["parent"]["path"] = Path;

    std::string NextToken;
    do
    {
        Request["list_info"]["token"] = NextToken;

        nlohmann::json Response = nlohmann::json::parse(HalalCloud::Request(
            Token,
            "/v6/userfile/list",
            Request.dump()));

        NextToken = Mile::Json::ToString(Mile::Json::GetSubKey(
            Mile::Json::GetSubKey(Response, "list_info"),
            "token"));

        for (nlohmann::json const& File
            : Mile::Json::GetSubKey(Response, "files"))
        {
            Result.emplace_back(HalalCloud::FileInformation(File.dump()));
        }
    } while (!NextToken.empty());

    return Result;
}

void HalalCloud::AppendFileList(
    HalalCloud::FileDictionary& Dictionary,
    HalalCloud::UserToken& Token,
    std::string_view Path)
{
    nlohmann::json Request;
    Request["parent"]["path"] = Path;

    std::string NextToken;
    do
    {
        Request["list_info"]["token"] = NextToken;

        nlohmann::json Response = nlohmann::json::parse(HalalCloud::Request(
            Token,
            "/v6/userfile/list",
            Request.dump()));

        NextToken = Mile::Json::ToString(Mile::Json::GetSubKey(
            Mile::Json::GetSubKey(Response, "list_info"),
            "token"));

        for (nlohmann::json const& File
            : Mile::Json::GetSubKey(Response, "files"))
        {
            HalalCloud::FileInformation FileInfo(File.dump());
            Dictionary.emplace(FileInfo.FileName, FileInfo);
        }
    } while (!NextToken.empty());
}
