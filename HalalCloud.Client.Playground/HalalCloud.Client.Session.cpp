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

#include "HalalCloud.Client.Core.h"

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

    std::string ResponseJson;
    HCC_RPC_STATUS Status = ::HccRpcPostRequest(
        AccessToken,
        MethodFullName.data(),
        Request.dump().c_str(),
        ResponseJson);
    if (HCC_RPC_STATUS_OK != Status)
    {
        HalalCloud::ThrowException(
            "HccRpcPostRequest",
            Status);
    }
    return nlohmann::json::parse(ResponseJson);
}

void HalalCloud::Session::Authenticate(
    std::function<void(std::string_view)> Callback)
{
    nlohmann::json Request;
    {
        Request = nlohmann::json();
        Request["return_type"] = 2;
        Request["state"] = "HalalCloud.Client.Session.Authenticate";

        nlohmann::json Response = this->Request(
            "/v6/user/create_auth_token",
            Request);

        Callback(Mile::Json::ToString(
            Mile::Json::GetSubKey(Response, "return_url")));

        Request = nlohmann::json();
        Request["return_type"] = 2;
        Request["callback"] = Mile::Json::ToString(
            Mile::Json::GetSubKey(Response, "callback"));
    }

    while (true)
    {
        nlohmann::json Response = this->Request(
            "/v6/user/verify_auth_token",
            Request);
        if (6 == Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Response, "status")))
        {
            nlohmann::json Token = Mile::Json::GetSubKey(
                Mile::Json::GetSubKey(Response, "login"),
                "token");
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
        "/v6/user/refresh",
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

    std::vector<std::int64_t> Sizes;
    for (nlohmann::json const& Size : Mile::Json::GetSubKey(Response, "sizes"))
    {
        std::int64_t StartIndex = Mile::ToUInt64(Mile::Json::ToString(
            Mile::Json::GetSubKey(Size, "start_index")));
        std::int64_t EndIndex = Mile::ToUInt64(Mile::Json::ToString(
            Mile::Json::GetSubKey(Size, "end_index")));
        std::int64_t BlockSize = Mile::ToUInt64(Mile::Json::ToString(
            Mile::Json::GetSubKey(Size, "size")));
        for (std::int64_t i = StartIndex; i < EndIndex + 1; ++i)
        {
            Sizes.push_back(BlockSize);
        }
    }

    nlohmann::json RawNodes = Mile::Json::GetSubKey(Response, "raw_nodes");
    if (RawNodes.size() != Sizes.size())
    {
        throw std::runtime_error(
            "The file nodes information acquisition failed.");
    }
    std::vector<std::string> Nodes;
    for (nlohmann::json const& Node : RawNodes)
    {
        Nodes.push_back(Mile::Json::ToString(Node));
    }

    std::int64_t ReportedFileSize = Mile::ToUInt64(Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "file_size")));
    std::int64_t FileSize = 0;
    std::vector<std::int64_t> Offsets;
    for(std::int64_t const& Size : Sizes)
    {
        Offsets.push_back(FileSize);
        FileSize += Size;
    }
    if (FileSize != ReportedFileSize)
    {
        throw std::runtime_error(
            "The file size verification failed.");
    }

    HalalCloud::FileStorageInformation Result;
    Result.Identifier = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "content_identity"));
    Result.Size = ReportedFileSize;
    Result.Path = Mile::Json::ToString(
        Mile::Json::GetSubKey(Response, "path"));
    switch (Mile::Json::ToInt64(Mile::Json::GetSubKey(Response, "store_type")))
    {
    case 0:
        Result.Type = HalalCloud::FileStorageType::Ipfs;
        break;
    case 10:
        Result.Type = HalalCloud::FileStorageType::BaiduObjectStorage;
        break;
    default:
        break;
    }
    for (std::size_t i = 0; i < RawNodes.size(); ++i)
    {
        HalalCloud::FileStorageNode Current;
        Current.Offset = Offsets[i];
        Current.Size = Sizes[i];
        Current.Identifier = Nodes[i];
        Result.Nodes.push_back(Current);
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

    std::vector<std::string> Identifiers;

    HalalCloud::FileStorageInformation Information =
        this->GetFileStorageInformation(SourceFilePath);
    for (HalalCloud::FileStorageNode const& Node : Information.Nodes)
    {
        Identifiers.push_back(Node.Identifier);
    }

    std::vector<HalalCloud::BlockStorageInformation> Blocks =
        this->GetBlockStorageInformation(Identifiers);
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
