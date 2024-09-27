/*
 * PROJECT:   Halal Cloud Client
 * FILE:      HalalCloud.Client.Session.cpp
 * PURPOSE:   Implementation for the HalalCloud.Client.Session
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "HalalCloud.Client.Session.h"

#include <stdexcept>
#include <thread>

#include <HalalCloud.BaiduBce.h>

[[noreturn]] void HalalCloud::ThrowExceptionWithHResult(
    _In_ LPCSTR Checkpoint,
    _In_ HRESULT Value)
{
    throw std::runtime_error(Mile::FormatString(
        "[HalalCloud.Client] %s Failed. (0x%08X)",
        Checkpoint,
        Value));
}

void HalalCloud::Session::ApplyAccessToken(
    nlohmann::json const& Token)
{
    std::string AccessToken = Mile::Json::ToString(
        Mile::Json::GetSubKey(Token, "access_token"));
    HRESULT hr = ::HccRpcSetAccessToken(
        this->m_Session,
        AccessToken.c_str());
    if (FAILED(hr))
    {
        HalalCloud::ThrowExceptionWithHResult(
            "HccRpcSetAccessToken",
            hr);
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

HalalCloud::Session::Session()
{
    HRESULT hr = ::HccRpcCreateSession(&this->m_Session);
    if (FAILED(hr))
    {
        HalalCloud::ThrowExceptionWithHResult(
            "HccRpcCreateSession",
            hr);
    }
}

HalalCloud::Session::~Session()
{
    ::HccRpcCloseSession(this->m_Session);
}

HCC_RPC_SESSION HalalCloud::Session::NativeHandle()
{
    return this->m_Session;
}

nlohmann::json HalalCloud::Session::CurrentToken()
{
    return this->m_CurrentToken;
}

nlohmann::json HalalCloud::Session::Request(
    std::string_view MethodFullName,
    nlohmann::json const& Request)
{
    nlohmann::json Response;

    LPSTR ResponseJson = nullptr;

    HRESULT hr = ::HccRpcRequest(
        this->m_Session,
        MethodFullName.data(),
        Request.dump().c_str(),
        &ResponseJson);
    if (FAILED(hr))
    {
        HalalCloud::ThrowExceptionWithHResult(
            "HccRpcRequest",
            hr);
    }

    auto ResponseJsonCleanupHandler = Mile::ScopeExitTaskHandler([&]()
    {
        ::HccRpcFreeMemory(ResponseJson);
    });

    return nlohmann::json::parse(ResponseJson);
}

void HalalCloud::Session::Authenticate(
    std::function<void(std::string_view)> Callback)
{
    nlohmann::json Request;
    {
        Request = nlohmann::json();
        Request["return_type"] = 2;

        nlohmann::json Response = this->Request(
            "/v6.services.pub.PubUser/CreateAuthToken",
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
            "/v6.services.pub.PubUser/VerifyAuthToken",
            Request);
        if (6 == Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Response, "status")))
        {
            nlohmann::json Token = Mile::Json::GetSubKey(
                Mile::Json::GetSubKey(Response, "login"),
                "token");
            this->ApplyAccessToken(Token);
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
        "/v6.services.pub.PubUser/Refresh",
        Request);
    this->ApplyAccessToken(Response);
    this->m_CurrentToken = Response;
}

void HalalCloud::Session::Logout()
{
    if (!this->m_CurrentToken.empty())
    {
        this->Request(
            "/v6.services.pub.PubUser/Logoff",
            this->m_CurrentToken);
        this->m_CurrentToken = std::string();
    }
}

nlohmann::json HalalCloud::Session::GetUserInformation()
{
    return this->Request(
        "/v6.services.pub.PubUser/Get",
        nlohmann::json::object());
}

nlohmann::json HalalCloud::Session::CreateFolder(
    std::string_view Path)
{
    nlohmann::json Request;
    Request["path"] = Path;
    return this->Request(
        "/v6.services.pub.PubUserFile/Create",
        Request);
}

HalalCloud::FileInformation HalalCloud::Session::GetFileInformation(
    std::string_view Path)
{
    nlohmann::json Request;
    Request["path"] = Path;

    nlohmann::json Response = this->Request(
        "/v6.services.pub.PubUserFile/Get",
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
            "/v6.services.pub.PubUserFile/List",
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
    nlohmann::json Request;
    Request["path"] = TargetFilePath;

    nlohmann::json Response = this->Request(
        "/v6.services.pub.PubUserFile/CreateUploadToken",
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
        HalalCloud::ThrowExceptionWithHResult(
            "HccBcePutObject",
            Error);
    }
}

HalalCloud::FileStorageInformation HalalCloud::Session::GetFileStorageInformation(
    std::string_view Path)
{
    nlohmann::json Request;
    Request["path"] = Path;

    nlohmann::json Response = this->Request(
        "/v6.services.pub.PubUserFile/ParseFileSlice",
        Request);

    std::vector<std::int64_t> Sizes;
    for (nlohmann::json const& Size : Mile::Json::GetSubKey(Response, "sizes"))
    {
        std::int64_t StartIndex = Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Size, "start_index"));
        std::int64_t EndIndex = Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Size, "end_index"));
        std::int64_t BlockSize = Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Size, "size"));
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

    std::int64_t ReportedFileSize = Mile::Json::ToInt64(
        Mile::Json::GetSubKey(Response, "file_size"));
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
