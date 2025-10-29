/*
 * PROJECT:   Halal Cloud Client
 * FILE:      HalalCloud.Client.Session.cpp
 * PURPOSE:   Implementation for the HalalCloud.Client.Session
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "HalalCloud.Client.Session.h"

#include <stdexcept>
#include <thread>

#ifdef _WIN32
#include <ShlObj.h>
#endif

#include <fstream>

#include <HccApi.h>

[[noreturn]] void HalalCloud::ThrowException(
    std::string_view Checkpoint,
    std::int32_t const& Code)
{
    throw std::runtime_error(Mile::FormatString(
        "[HalalCloud.Client] %s Failed. (Code = %d)",
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

std::filesystem::path HalalCloud::GetUserCloudCachePath(
    std::string_view UserIdentity)
{
    std::filesystem::path Result = HalalCloud::GetApplicationDataRootPath();
    Result /= "Users";
    Result /= UserIdentity;
    Result /= "CloudCache";
    if (!std::filesystem::exists(Result))
    {
        std::filesystem::create_directories(Result);
    }
    return Result;
}

std::vector<std::uint8_t> HalalCloud::ReadAllBytesFromFile(
    std::string_view FilePath)
{
    std::vector<std::uint8_t> Result;

    std::ifstream FileStream(FilePath.data(), std::ios::binary);
    if (FileStream.is_open())
    {
        FileStream.seekg(0, std::ios::end);
        std::streampos FileSize = FileStream.tellg();
        FileStream.seekg(0, std::ios::beg);
        Result.resize(static_cast<std::size_t>(FileSize));
        FileStream.read(
            reinterpret_cast<char*>(Result.data()),
            FileSize);
    }

    return Result;
}

void HalalCloud::WriteAllBytesToFile(
    std::string_view FilePath,
    std::vector<std::uint8_t> const& Bytes)
{
    std::ofstream FileStream(FilePath.data(), std::ios::binary);
    if (FileStream.is_open())
    {
        FileStream.write(
            reinterpret_cast<char const*>(Bytes.data()),
            Bytes.size());
    }
}

bool HalalCloud::FileStorageSizeRange::IsInRange(
    std::size_t const& Index) const
{
    return Index >= this->StartBlockIndex && Index <= this->EndBlockIndex;
}

std::size_t HalalCloud::FileStorageSizeRange::GetRangeBlockCount() const
{
    return (this->EndBlockIndex - this->StartBlockIndex + 1);
}

std::int64_t HalalCloud::FileStorageSizeRange::GetRangeTotalSize() const
{
    return this->GetRangeBlockCount() * this->SingleBlockSize;
}

bool HalalCloud::FileStorageInformation::GetStartBlockIndex(
    std::size_t& StartBlockIndex,
    std::int64_t& StartBlockOffset,
    std::int64_t const& Offset) const
{
    if (Offset < 0 || Offset >= this->Size)
    {
        return false;
    }

    StartBlockOffset = Offset;
    for (HalalCloud::FileStorageSizeRange const& SizeRange : this->SizeRanges)
    {
        std::int64_t CurrentRangeSize = SizeRange.GetRangeTotalSize();
        if (StartBlockOffset >= CurrentRangeSize)
        {
            StartBlockOffset -= CurrentRangeSize;
            continue;
        }

        std::size_t BlocksToSkip = static_cast<std::size_t>(
            StartBlockOffset / SizeRange.SingleBlockSize);
        StartBlockIndex += SizeRange.StartBlockIndex + BlocksToSkip;
        StartBlockOffset = StartBlockOffset % SizeRange.SingleBlockSize;
        return true;
    }

    StartBlockIndex = static_cast<std::size_t>(-1);
    StartBlockOffset = -1;

    return false;
}

bool HalalCloud::FileStorageInformation::GetBlocks(
    std::vector<std::pair<std::string, std::int64_t>>& RequestedBlocks,
    std::int64_t& StartBlockOffset,
    std::int64_t const& Offset,
    std::uint32_t const& Length) const
{
    if (Offset < 0 || Offset + Length > this->Size)
    {
        return false;
    }
    RequestedBlocks.clear();

    std::size_t StartBlockIndex = 0;
    if (!this->GetStartBlockIndex(
        StartBlockIndex,
        StartBlockOffset,
        Offset))
    {
        return false;
    }

    std::int64_t ProceededSize = 0;
    for (HalalCloud::FileStorageSizeRange const& SizeRange : this->SizeRanges)
    {
        if (!SizeRange.IsInRange(StartBlockIndex))
        {
            continue;
        }

        for (size_t i = StartBlockIndex; i <= SizeRange.EndBlockIndex; ++i)
        {
            RequestedBlocks.push_back(std::make_pair(
                this->Blocks[i],
                SizeRange.SingleBlockSize));

            // If this is the first block, adjust the size with the offset.
            ProceededSize += (ProceededSize
                ? SizeRange.SingleBlockSize
                : (SizeRange.SingleBlockSize - StartBlockOffset));

            if (ProceededSize >= Length)
            {
                return true;
            }
        }
    }

    RequestedBlocks.clear();
    StartBlockOffset = -1;
    return false;
}

HalalCloud::FileInformation HalalCloud::Session::ToFileInformation(
    nlohmann::json const& Object)
{
    HalalCloud::FileInformation Result;

    Result.CreationTime = Mile::Json::ToInt64(
        Mile::Json::GetSubKey(Object, "create_ts"));
    Result.LastWriteTime = Mile::Json::ToInt64(
        Mile::Json::GetSubKey(Object, "update_ts"));
    Result.FileSize = Mile::Json::ToInt64(
        Mile::Json::GetSubKey(Object, "size"));
    Result.FileAttributes.Fields.IsDirectory = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Object, "dir"));
    Result.FileAttributes.Fields.IsHidden = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Object, "hidden"));
    Result.FileName = Mile::Json::ToString(
        Mile::Json::GetSubKey(Object, "name"));

    return Result;
}

HalalCloud::BlockStorageInformation HalalCloud::Session::ToBlockStorageInformation(
    nlohmann::json const& Object)
{
    HalalCloud::BlockStorageInformation Result;
    Result.Identifier = Mile::Json::ToString(
        Mile::Json::GetSubKey(Object, "identity"));
    Result.DownloadLink = Mile::Json::ToString(
        Mile::Json::GetSubKey(Object, "download_address"));
    Result.EncryptionByte = static_cast<std::uint8_t>(
        Mile::Json::ToUInt64(
            Mile::Json::GetSubKey(Object, "encrypt")));
    return Result;
}

nlohmann::json HalalCloud::Session::CurrentToken()
{
    return this->m_CurrentToken;
}

nlohmann::json HalalCloud::Session::Request(
    std::string_view MethodFullName,
    nlohmann::json const& Request)
{
    std::string AccessToken = "*";
    if (!this->m_CurrentToken.empty())
    {
        AccessToken = Mile::Json::ToString(
            Mile::Json::GetSubKey(this->m_CurrentToken, "access_token"));
    }

    MO_STRING ResponseJsonString = nullptr;
    HCC_RPC_STATUS Status = ::HccRpcPostRequest(
        &ResponseJsonString,
        AccessToken.c_str(),
        MethodFullName.data(),
        Request.dump().c_str());
    if (HCC_RPC_STATUS_OK != Status)
    {
        HalalCloud::ThrowException(
            "HccRpcPostRequest",
            Status);
    }
    nlohmann::json ResponseJson = nlohmann::json::parse(ResponseJsonString);
    ::HccFreeMemory(ResponseJsonString);
    return ResponseJson;
}

void HalalCloud::Session::Authenticate(
    std::function<void(std::string_view)> Callback)
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

    nlohmann::json Request;
    {
        Request = nlohmann::json();
        Request["client_id"] = "puc_5_dcpqfdvu8qnt_v1";
        Request["code_challenge"] = CodeChallenge;
        Request["code_challenge_method"] = "S256";
        Request["legacy"] = true;

        nlohmann::json Response = this->Request(
            "/v6/oauth/authorize",
            Request);

        Callback(Mile::Json::ToString(
            Mile::Json::GetSubKey(Response, "redirect_uri")));

        Request = nlohmann::json();
        Request["code"] = Mile::Json::ToString(
            Mile::Json::GetSubKey(Response, "code"));
    }

    while (true)
    {
        nlohmann::json Response = this->Request(
            "/v6/oauth/get_authorize_state",
            Request);
        std::string Status = Mile::Json::ToString(
            Mile::Json::GetSubKey(Response, "status"));
        if ("AUTHORIZATION_FAILED" == Status)
        {
            HalalCloud::ThrowException(
                "Authorization Failed",
                -1);
        }

        if ("AUTHORIZATION_TOKEN_CREATED" == Status)
        {
            Request["code_verifier"] = CodeVerifier;
            nlohmann::json Token = this->Request(
                "/v6/oauth/get_token",
                Request);
            this->m_CurrentToken = Token;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void HalalCloud::Session::Impersonate(
    std::string_view RefreshToken)
{
    nlohmann::json Request;
    Request["refresh_token"] = RefreshToken;
    nlohmann::json Response = this->Request(
        "/v6/oauth/refresh_token",
        Request);
    this->m_CurrentToken = Response;
}

void HalalCloud::Session::Logout()
{
    if (!this->m_CurrentToken.empty())
    {
        this->Request(
            "/v6/user/logoff",
            this->m_CurrentToken);
        this->m_CurrentToken = std::string();
    }
}

nlohmann::json HalalCloud::Session::GetUserInformation()
{
    return this->Request(
        "/v6/user/get",
        nlohmann::json::object());
}

nlohmann::json HalalCloud::Session::CreateFolder(
    std::string_view Path)
{
    nlohmann::json Request;
    Request["path"] = Path;
    return this->Request(
        "/v6/userfile/create",
        Request);
}

HalalCloud::FileInformation HalalCloud::Session::GetFileInformation(
    std::string_view Path)
{
    nlohmann::json Request;
    Request["path"] = Path;

    nlohmann::json Response = this->Request(
        "/v6/userfile/get",
        Request);

    return this->ToFileInformation(Response);
}

std::vector<HalalCloud::FileInformation> HalalCloud::Session::EnumerateFiles(
    std::string_view Path)
{
    std::vector<HalalCloud::FileInformation> Result;
    std::string NextToken;

    nlohmann::json Request;
    Request["parent"]["path"] = Path;

    do
    {
        Request["list_info"]["token"] = NextToken;

        nlohmann::json Response = this->Request(
            "/v6/userfile/list",
            Request);

        NextToken = Mile::Json::ToString(Mile::Json::GetSubKey(
            Mile::Json::GetSubKey(Response, "list_info"),
            "token"));

        for (nlohmann::json const& File
            : Mile::Json::GetSubKey(Response, "files"))
        {
            Result.push_back(this->ToFileInformation(File));
        }
    } while (!NextToken.empty());

    return Result;
}

void HalalCloud::Session::UploadFile(
    std::string_view SourceFilePath,
    std::string_view TargetFilePath)
{
    SourceFilePath;
    TargetFilePath;

    /*nlohmann::json Request;
    Request["path"] = TargetFilePath;

    nlohmann::json Response = this->Request(
        "/v6/userfile/create_upload_token",
        Request);

    std::string Endpoint = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "endpoint"));
    std::string AccessKey = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "access_key"));
    std::string SecretKey = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "secret_key"));
    std::string Token = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "token"));
    std::string Bucket = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "bucket"));
    std::string Key = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "key"));

    int Error = ::HccBcePutObject(
        SourceFilePath.data(),
        Endpoint.c_str(),
        AccessKey.c_str(),
        SecretKey.c_str(),
        Token.c_str(),
        Bucket.c_str(),
        Key.c_str());
    if (0 != Error)
    {
        HalalCloud::ThrowException(
            "HccBcePutObject",
            Error);
    }*/
}

HalalCloud::FileStorageInformation HalalCloud::Session::GetFileStorageInformation(
    std::string_view Path)
{
    nlohmann::json Request;
    Request["path"] = Path;

    nlohmann::json Response = this->Request(
        "/v6/userfile/parse_file_slice",
        Request);

    HalalCloud::FileStorageInformation Result = {};
    Result.Name = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "name"));
    Result.Path = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "path"));
    Result.Size = Mile::ToUInt64(Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "file_size")));
    switch (Mile::Json::ToInt64(Mile::Json::GetSubKey(Response, "store_type")))
    {
    case 0:
        Result.Type = HalalCloud::FileStorageType::Ipfs;
        break;
    case 10:
        Result.Type = HalalCloud::FileStorageType::BaiduObjectStorage;
        break;
    default:
        Result.Type = HalalCloud::FileStorageType::Unknown;
        break;
    }
    {
        Result.Identifier = Mile::Json::ToString(
            Mile::Json::GetSubKey(Response, "content_identity"));
    }

    nlohmann::json RawNodes = Mile::Json::GetSubKey(Response, "raw_nodes");
    nlohmann::json RawSizes = Mile::Json::GetSubKey(Response, "sizes");

    std::size_t TotalNodeCount = 0;
    std::int64_t TotalNodeSize = 0;
    for (nlohmann::json const& Size : RawSizes)
    {
        HalalCloud::FileStorageSizeRange Range = {};
        Range.StartBlockIndex = Mile::ToUInt32(Mile::Json::ToString(
            Mile::Json::GetSubKey(Size, "start_index")));
        Range.EndBlockIndex = Mile::ToUInt32(Mile::Json::ToString(
            Mile::Json::GetSubKey(Size, "end_index")));
        Range.SingleBlockSize = Mile::ToInt64(Mile::Json::ToString(
            Mile::Json::GetSubKey(Size, "size")));

        TotalNodeCount += Range.GetRangeBlockCount();
        TotalNodeSize += Range.GetRangeTotalSize();

        Result.SizeRanges.emplace_back(Range);
    }
    if (RawNodes.size() != TotalNodeCount ||
        Result.Size != TotalNodeSize)
    {
        throw std::runtime_error(
            "The file nodes information acquisition failed.");
    }

    for (nlohmann::json const& Node : RawNodes)
    {
        Result.Blocks.emplace_back(Mile::Json::ToString(Node));
    }

    return Result;
}

std::vector<HalalCloud::BlockStorageInformation> HalalCloud::Session::GetBlockStorageInformation(
    std::vector<std::string> const& Identifiers)
{
    std::vector<HalalCloud::BlockStorageInformation> Result;
    {
        std::size_t Proceeded = 0;
        nlohmann::json RequestIdentifiers = nlohmann::json::array();
        for (std::string const& Identifier : Identifiers)
        {
            RequestIdentifiers.push_back(Identifier);
            ++Proceeded;
            if (0 == Proceeded % 200)
            {
                nlohmann::json Request;
                Request["version"] = 1;
                Request["identity"] = RequestIdentifiers;
                nlohmann::json Response = this->Request(
                    "/v6/userfile/get_slice_download_address",
                    Request);
                for (nlohmann::json const& Address
                    : Mile::Json::GetSubKey(Response, "addresses"))
                {
                    Result.push_back(this->ToBlockStorageInformation(Address));
                }
                RequestIdentifiers.clear();
            }
        }
        if (!RequestIdentifiers.empty())
        {
            nlohmann::json Request;
            Request["version"] = 1;
            Request["identity"] = RequestIdentifiers;
            nlohmann::json Response = this->Request(
                "/v6/userfile/get_slice_download_address",
                Request);
            for (nlohmann::json const& Address
                : Mile::Json::GetSubKey(Response, "addresses"))
            {
                Result.push_back(this->ToBlockStorageInformation(Address));
            }
            RequestIdentifiers.clear();
        }
    }
    return Result;
}

void HalalCloud::Session::DownloadFile(
    std::string_view SourceFilePath,
    std::string_view TargetFilePath)
{
    TargetFilePath;

    HalalCloud::FileStorageInformation Information =
        this->GetFileStorageInformation(SourceFilePath);

    std::vector<HalalCloud::BlockStorageInformation> Blocks =
        this->GetBlockStorageInformation(Information.Blocks);
    for (HalalCloud::BlockStorageInformation const& Block : Blocks)
    {
        MO_UINT8 OutputHashBytes[32] = {};
        if (::HccCidGetSha256(
            Block.Identifier.c_str(),
            &OutputHashBytes))
        {
            std::string HashString;
            static const char* HexChars = "0123456789abcdef";
            for (std::uint8_t const& Byte : OutputHashBytes)
            {
                HashString.push_back(HexChars[Byte >> 4]);
                HashString.push_back(HexChars[Byte & 0xF]);
            }
            std::printf(
                "[INFO] Block %s with SHA-256 %s.\n",
                Block.Identifier.c_str(),
                HashString.c_str());
        }
        else
        {
            std::printf("[ERROR] HccCidGetSha256 failed.\n");
        }

        //std::printf("Downloading %s...\n", Block.DownloadLink.c_str());
    }
}
