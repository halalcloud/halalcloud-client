/*
 * PROJECT:   Halal Cloud Client
 * FILE:      HalalCloud.Client.Playground.cpp
 * PURPOSE:   Implementation for Halal Cloud Client Playground
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include <clocale>
#include <cstdio>
#include <cwchar>

#include <Windows.h>

#include <Mile.Helpers.CppBase.h>

#include <Mile.Json.h>

#include "HalalCloud.Client.Session.h"

#include <time.h>

#include <fuse.h>

int HccFuseOpenCallback(
    const char* path,
    fuse_file_info* fi)
{
    UNREFERENCED_PARAMETER(fi);

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
    {
        return -EINVAL;
    }

    HalalCloud::Session* Session =
        static_cast<HalalCloud::Session*>(Context->private_data);
    if (!Session)
    {
        return -EINVAL;
    }

    try
    {
        Session->GetFileStorageInformation(path);
    }
    catch (...)
    {
        return -EINVAL;
    }

    return 0;
}

int HccFuseReadCallback(
    const char* path,
    char* buf,
    size_t size,
    FUSE_OFF_T offset,
    fuse_file_info* fi)
{
    UNREFERENCED_PARAMETER(offset);
    UNREFERENCED_PARAMETER(fi);

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
    {
        return -EINVAL;
    }

    HalalCloud::Session* Session =
        static_cast<HalalCloud::Session*>(Context->private_data);
    if (!Session)
    {
        return -EINVAL;
    }

    try
    {
        HalalCloud::FileStorageInformation Information =
            Session->GetFileStorageInformation(path);
        for (HalalCloud::FileStorageNode const& Node : Information.Nodes)
        {
            if (offset < Node.Offset || offset > Node.Offset + Node.Size)
            {
                continue;
            }

            std::memset(buf, 0, size);
            std::memcpy(buf, Node.Identifier.c_str(), Node.Identifier.size());
            break;
        }
    }
    catch (...)
    {
        return -EINVAL;
    }

    return (int)size;
}

int HccFuseStatFsCallback(
    const char* path,
    struct statvfs* buf)
{
    UNREFERENCED_PARAMETER(path);

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
    {
        return -EINVAL;
    }

    HalalCloud::Session* Session =
        static_cast<HalalCloud::Session*>(Context->private_data);
    if (!Session)
    {
        return -EINVAL;
    }

    try
    {
        nlohmann::json Response = Session->Request(
            "/v6.services.pub.PubUser/GetStatisticsAndQuota",
            nlohmann::json::object());

        std::printf(
            "Response = %s\n",
            Response.dump(2).c_str());

        std::memset(buf, 0, sizeof(statvfs));
        buf->f_bsize = 512;
        buf->f_blocks = Mile::Json::ToUInt64(Mile::Json::GetSubKey(
            Mile::Json::GetSubKey(Response, "disk_statistics_quota"),
            "bytes_quota")) / buf->f_bsize;
        buf->f_bfree = Mile::Json::ToUInt64(Mile::Json::GetSubKey(
            Mile::Json::GetSubKey(Response, "disk_statistics_quota"),
            "bytes_used")) / buf->f_bsize;
        buf->f_bavail = buf->f_blocks - buf->f_bfree;
    }
    catch (...)
    {
        return -EINVAL;
    }

    return 0;
}

int HccFuseGetAttributesCallback(
    const char* path,
    FUSE_STAT* buf)
{
    if (!buf)
    {
        return -EINVAL;
    }
    std::memset(buf, 0, sizeof(FUSE_STAT));

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
    {
        return -EINVAL;
    }

    HalalCloud::Session* Session =
        static_cast<HalalCloud::Session*>(Context->private_data);
    if (!Session)
    {
        return -EINVAL;
    }

    try
    {
        HalalCloud::FileInformation File = Session->GetFileInformation(path);

        buf->st_mode = S_IREAD | S_IWRITE;
        buf->st_mode |=
            File.FileAttributes.Fields.IsDirectory ? S_IFDIR : S_IFREG;
        if (!File.FileAttributes.Fields.IsDirectory)
        {
            buf->st_size = File.FileSize;
        }
        buf->st_atim.tv_sec = File.LastWriteTime / 1000;
        buf->st_mtim.tv_sec = File.LastWriteTime / 1000;
        buf->st_ctim.tv_sec = File.LastWriteTime / 1000;
        buf->st_birthtim.tv_sec = File.CreationTime / 1000;
    }
    catch (...)
    {
        return -EINVAL;
    }

    return 0;
}

int HccFuseReadDirectoryCallback(
    const char* path,
    void* buf,
    fuse_fill_dir_t filler,
    FUSE_OFF_T off,
    fuse_file_info* fi)
{
    UNREFERENCED_PARAMETER(off);
    UNREFERENCED_PARAMETER(fi);

    fuse_context* Context = ::fuse_get_context();
    if (!Context)
    {
        return -EINVAL;
    }

    HalalCloud::Session* Session =
        static_cast<HalalCloud::Session*>(Context->private_data);
    if (!Session)
    {
        return -EINVAL;
    }

    std::vector<HalalCloud::FileInformation> Files =
        Session->EnumerateFiles(path);
    for (HalalCloud::FileInformation const& File : Files)
    {
        FUSE_STAT stbuf = { 0 };
        stbuf.st_mode = S_IREAD | S_IWRITE;
        stbuf.st_mode |=
            File.FileAttributes.Fields.IsDirectory ? S_IFDIR : S_IFREG;
        if (!File.FileAttributes.Fields.IsDirectory)
        {
            stbuf.st_size = File.FileSize;
        }
        stbuf.st_atim.tv_sec = File.LastWriteTime / 1000;
        stbuf.st_mtim.tv_sec = File.LastWriteTime / 1000;
        stbuf.st_ctim.tv_sec = File.LastWriteTime / 1000;
        stbuf.st_birthtim.tv_sec = File.CreationTime / 1000;
        if (filler(buf, File.FileName.c_str(), &stbuf, 0))
        {
            break;
        }
    }

    return 0;
}

void* HccFuseInitializeCallback(
    fuse_conn_info* conn)
{
    UNREFERENCED_PARAMETER(conn);

    fuse_context* Context = ::fuse_get_context();
    if (Context)
    {
        return Context->private_data;
    }

    return nullptr;
}

#include <curl/curl.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/md.h>

std::string BytesToHexString(
    std::vector<uint8_t> const& Bytes)
{
    static const char* HexChars = "0123456789abcdef";
    std::string Result;
    Result.reserve(Bytes.size() * 2);
    for (uint8_t const& Byte : Bytes)
    {
        Result.push_back(HexChars[Byte >> 4]);
        Result.push_back(HexChars[Byte & 0xF]);
    }
    return Result;
}

std::string ComputeSha256(
    std::string const& Data)
{
    std::vector<std::uint8_t> Result;

    mbedtls_md_context_t Context;
    ::mbedtls_md_init(&Context);

    do
    {
        const mbedtls_md_info_t* Algorithm =
            ::mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
        if (!Algorithm)
        {
            break;
        }

        if (0 != ::mbedtls_md_setup(&Context, Algorithm, 0))
        {
            break;
        }
        
        if (0 != ::mbedtls_md_starts(&Context))
        {
            break;
        }

        if (0 != ::mbedtls_md_update(
            &Context,
            reinterpret_cast<const unsigned char*>(Data.c_str()),
            Data.size() * sizeof(char)))
        {
            break;
        }

        Result.resize(::mbedtls_md_get_size(Algorithm));

        if (0 != ::mbedtls_md_finish(&Context, Result.data()))
        {
            Result.clear();
            break;
        }

    } while (false);

    ::mbedtls_md_free(&Context);

    return ::BytesToHexString(Result);
}

std::vector<std::uint8_t> ComputeHmacSha256(
    std::vector<std::uint8_t> const& Key,
    std::string const& Data)
{
    std::vector<std::uint8_t> Result;

    mbedtls_md_context_t Context;
    ::mbedtls_md_init(&Context);

    do
    {
        const mbedtls_md_info_t* Algorithm =
            ::mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
        if (!Algorithm)
        {
            break;
        }

        if (0 != ::mbedtls_md_setup(&Context, Algorithm, 1))
        {
            break;
        }

        if (0 != mbedtls_md_hmac_starts(
            &Context,
            Key.data(),
            Key.size()))
        {
            break;
        }

        if (0 != mbedtls_md_hmac_update(
            &Context,
            reinterpret_cast<const unsigned char*>(Data.c_str()),
            Data.size() * sizeof(char)))
        {
            break;
        }

        Result.resize(::mbedtls_md_get_size(Algorithm));

        if (0 != ::mbedtls_md_hmac_finish(&Context, Result.data()))
        {
            Result.clear();
            break;
        }

    } while (false);

    ::mbedtls_md_free(&Context);

    return Result;
}

std::string GenerateNonce()
{
    GUID Guid = { 0 };

    mbedtls_entropy_context EntropyContext;
    ::mbedtls_entropy_init(&EntropyContext);

    mbedtls_ctr_drbg_context CtrDrbgContext;
    ::mbedtls_ctr_drbg_init(&CtrDrbgContext);
    ::mbedtls_ctr_drbg_set_prediction_resistance(
        &CtrDrbgContext,
        MBEDTLS_CTR_DRBG_PR_ON);

    do
    {
        if (0 != ::mbedtls_ctr_drbg_seed(
            &CtrDrbgContext,
            ::mbedtls_entropy_func,
            &EntropyContext,
            nullptr,
            0))
        {
            break;
        }

        
        if (0 != ::mbedtls_ctr_drbg_random(
            &CtrDrbgContext,
            reinterpret_cast<unsigned char*>(&Guid),
            sizeof(Guid)))
        {
            break;
        }

    } while (false);

    ::mbedtls_ctr_drbg_free(&CtrDrbgContext);

    ::mbedtls_entropy_free(&EntropyContext);
    
    return Mile::FormatString(
        "%08x-%04hx-%04hx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
        Guid.Data1,
        Guid.Data2,
        Guid.Data3,
        Guid.Data4[0],
        Guid.Data4[1],
        Guid.Data4[2],
        Guid.Data4[3],
        Guid.Data4[4],
        Guid.Data4[5],
        Guid.Data4[6],
        Guid.Data4[7]);
}

std::time_t GetCurrentPosixTime()
{
    return std::time(nullptr);
}

std::tm ToUtcTime(
    std::time_t const& PosixTime)
{
    std::tm UtcTime = { 0 };
#ifdef _MSC_VER
    ::gmtime_s(&UtcTime, &PosixTime);
#else
    ::gmtime_r(&PosixTime, &UtcTime);
#endif
    return UtcTime;
}

std::string ToIso8601UtcDate(
    std::tm const& UtcTime)
{
    return Mile::FormatString(
        "%04d-%02d-%02d",
        UtcTime.tm_year + 1900,
        UtcTime.tm_mon + 1,
        UtcTime.tm_mday);
}

std::string ToIso8601UtcTime(
    std::tm const& UtcTime)
{
    return Mile::FormatString(
        "%02d:%02d:%02d",
        UtcTime.tm_hour,
        UtcTime.tm_min,
        UtcTime.tm_sec);
}

std::string ToIso8601UtcTimestamp(
    std::tm const& UtcTime)
{
    return Mile::FormatString(
        "%04d-%02d-%02dT%02d:%02d:%02dZ",
        UtcTime.tm_year + 1900,
        UtcTime.tm_mon + 1,
        UtcTime.tm_mday,
        UtcTime.tm_hour,
        UtcTime.tm_min,
        UtcTime.tm_sec);
}

std::size_t HccRpcCurlWriteCallback(
    char* data,
    std::size_t size,
    std::size_t nmemb,
    void* clientp)
{
    std::size_t realsize = size * nmemb;

    std::string* OutputString = reinterpret_cast<std::string*>(clientp);
    if (OutputString)
    {
        OutputString->append(std::string(data, realsize));
    }

    return realsize;
}

HCC_RPC_STATUS HccRpcPostRequest(
    std::string const& AccessToken,
    std::string const& ApiPath,
    std::string const& RequestJson,
    std::string& ResponseJson)
{
    const char RequestSuffix[] = "hl6_request";
    const char SignaturePrefix[] = "HL6";
    const char DefaultSignatureAlgorithm[] = "HL6-HMAC-SHA256";

    const char SecretId[] = "APPK_5f02f0889301fd7be1ac972c11bf3e7d";
    const char SecretKey[] = "AppSecretForTest_KSMWNDBAJ2hs__AS";

    std::time_t RequestPosixTime = ::GetCurrentPosixTime();

    std::tm RequestUtcTime = ::ToUtcTime(RequestPosixTime);

    std::string UtcDateString = ::ToIso8601UtcDate(RequestUtcTime);

    std::map<std::string, std::string> RequestHeaders;
    RequestHeaders.emplace(
        "host",
        "openapi.2dland.cn");
    RequestHeaders.emplace(
        "content-type",
        "application/json; charset=utf-8");
    RequestHeaders.emplace(
        "x-hl-nonce",
        ::GenerateNonce());
    RequestHeaders.emplace(
        "x-hl-timestamp",
        ::ToIso8601UtcTimestamp(RequestUtcTime));

    std::string FullUrl;
    FullUrl.append("https://");
    FullUrl.append(RequestHeaders["host"]);
    FullUrl.append(ApiPath);

    std::string SignedHeaders;
    for (auto const& RequestHeaderItem : RequestHeaders)
    {
        SignedHeaders.append(RequestHeaderItem.first);
        SignedHeaders.push_back(';');
    }
    SignedHeaders.pop_back();

    std::string CanonicalRequest;
    CanonicalRequest.append("POST");
    CanonicalRequest.push_back('\n');
    CanonicalRequest.append(ApiPath);
    CanonicalRequest.push_back('\n');
    CanonicalRequest.append(""); // All POST requests don't need query string.
    CanonicalRequest.push_back('\n');
    for (auto const& RequestHeaderItem : RequestHeaders)
    {
        CanonicalRequest.append(RequestHeaderItem.first);
        CanonicalRequest.push_back(':');
        CanonicalRequest.append(RequestHeaderItem.second);
        CanonicalRequest.push_back('\n');
    }
    CanonicalRequest.push_back('\n');
    CanonicalRequest.append(SignedHeaders);
    CanonicalRequest.push_back('\n');
    CanonicalRequest.append(::ComputeSha256(RequestJson));

    std::string CredentialScope;
    CredentialScope.append(UtcDateString);
    CredentialScope.push_back('/');
    CredentialScope.append(AccessToken);
    CredentialScope.push_back('/');
    CredentialScope.append(RequestSuffix);

    std::string StringToSign;
    StringToSign.append(DefaultSignatureAlgorithm);
    StringToSign.push_back('\n');
    StringToSign.append(RequestHeaders["x-hl-timestamp"]);
    StringToSign.push_back('\n');
    StringToSign.append(CredentialScope);
    StringToSign.push_back('\n');
    StringToSign.append(::ComputeSha256(CanonicalRequest));

    std::string Signature;
    Signature.append(SignaturePrefix);
    Signature.append(SecretKey);
    std::vector<std::uint8_t> InitialSignatureKey;
    for (auto const& Character : Signature)
    {
        InitialSignatureKey.push_back(Character);
    }
    Signature = ::BytesToHexString(
        ::ComputeHmacSha256(
            ::ComputeHmacSha256(
                ::ComputeHmacSha256(
                    ::ComputeHmacSha256(
                        InitialSignatureKey,
                        UtcDateString),
                    AccessToken),
                RequestSuffix),
            StringToSign));

    std::string Authorization;
    Authorization.append(DefaultSignatureAlgorithm);
    Authorization.push_back(' ');
    Authorization.append("Credential=");
    Authorization.append(SecretId);
    Authorization.push_back('/');
    Authorization.append(CredentialScope);
    Authorization.push_back(',');
    Authorization.append("SignedHeaders=");
    Authorization.append(SignedHeaders);
    Authorization.push_back(',');
    Authorization.append("Signature=");
    Authorization.append(Signature);

    RequestHeaders.emplace("authorization", Authorization);
    RequestHeaders.erase("host");

    CURL* CurlHandle = ::curl_easy_init();
    if (!CurlHandle)
    {
        return CURLE_FAILED_INIT;
    }
    auto CurlHandleCleanupHandler = Mile::ScopeExitTaskHandler([&]()
    {
        if (CurlHandle)
        {
            ::curl_easy_cleanup(CurlHandle);
        }
    });

    curl_slist* CurlRequestHeaders = nullptr;
    for (auto const& RequestHeaderItem : RequestHeaders)
    {
        std::string Current;
        Current.append(RequestHeaderItem.first);
        Current.push_back(':');
        Current.append(RequestHeaderItem.second);
        CurlRequestHeaders = ::curl_slist_append(
            CurlRequestHeaders,
            Current.c_str());
    }
    auto CurlRequestHeadersCleanupHandler = Mile::ScopeExitTaskHandler([&]()
    {
        if (CurlRequestHeaders)
        {
            ::curl_slist_free_all(CurlRequestHeaders);
        }
    });

    if (CURLE_OK != ::curl_easy_setopt(
        CurlHandle,
        CURLOPT_URL,
        FullUrl.c_str()))
    {
        return HCC_RPC_STATUS_INTERNAL;
    }

    if (CURLE_OK != ::curl_easy_setopt(CurlHandle, CURLOPT_POST, 1L))
    {
        return HCC_RPC_STATUS_INTERNAL;
    }

    if (CURLE_OK != ::curl_easy_setopt(
        CurlHandle,
        CURLOPT_HTTPHEADER,
        CurlRequestHeaders))
    {
        return HCC_RPC_STATUS_INTERNAL;
    }

    if (CURLE_OK != ::curl_easy_setopt(
        CurlHandle,
        CURLOPT_POSTFIELDS,
        RequestJson.c_str()))
    {
        return HCC_RPC_STATUS_INTERNAL;
    }

    if (CURLE_OK != ::curl_easy_setopt(
        CurlHandle,
        CURLOPT_POSTFIELDSIZE,
        RequestJson.size()))
    {
        return HCC_RPC_STATUS_INTERNAL;
    }

    if (CURLE_OK != ::curl_easy_setopt(CurlHandle, CURLOPT_SSL_VERIFYPEER, 0L))
    {
        return HCC_RPC_STATUS_INTERNAL;
    }

    if (CURLE_OK != ::curl_easy_setopt(
        CurlHandle,
        CURLOPT_WRITEFUNCTION,
        ::HccRpcCurlWriteCallback))
    {
        return HCC_RPC_STATUS_INTERNAL;
    }

    if (CURLE_OK != ::curl_easy_setopt(
        CurlHandle,
        CURLOPT_WRITEDATA,
        &ResponseJson))
    {
        return HCC_RPC_STATUS_INTERNAL;
    }

    if (CURLE_OK != ::curl_easy_perform(CurlHandle))
    {
        return HCC_RPC_STATUS_INTERNAL;
    }

    long ResponseCode = 0;
    if (CURLE_OK != ::curl_easy_getinfo(
        CurlHandle,
        CURLINFO_RESPONSE_CODE,
        &ResponseCode))
    {
        return HCC_RPC_STATUS_INTERNAL;
    }

    HCC_RPC_STATUS Status = HCC_RPC_STATUS_INTERNAL;
    switch (ResponseCode)
    {
    case 200:
        Status = HCC_RPC_STATUS_OK;
        break;
    case 499:
        Status = HCC_RPC_STATUS_CANCELLED;
        break;
    case 500:
        // Candidate: HCC_RPC_STATUS_INTERNAL
        // Candidate: HCC_RPC_STATUS_DATA_LOSS
        // I think we should use HCC_RPC_STATUS_UNKNOWN because maybe we will
        // receive other HTTP status codes which is more suitable to use
        // HCC_RPC_STATUS_INTERNAL.
        Status = HCC_RPC_STATUS_UNKNOWN;
        break;
    case 400:
        // Candidate: HCC_RPC_STATUS_FAILED_PRECONDITION
        // Candidate: HCC_RPC_STATUS_OUT_OF_RANGE
        // Due to these candidates is caused by invalid arguments in the most
        // cases, use HCC_RPC_STATUS_INVALID_ARGUMENT should be suitable.
        Status = HCC_RPC_STATUS_INVALID_ARGUMENT;
        break;
    case 504:
        Status = HCC_RPC_STATUS_DEADLINE_EXCEEDED;
        break;
    case 404:
        Status = HCC_RPC_STATUS_NOT_FOUND;
        break;
    case 409:
        // Candidate: HCC_RPC_STATUS_ABORTED
        // Maybe HTTP 409 more like HCC_RPC_STATUS_ALREADY_EXISTS.
        Status = HCC_RPC_STATUS_ALREADY_EXISTS;
        break;
    case 403:
        Status = HCC_RPC_STATUS_PERMISSION_DENIED;
        break;
    case 429:
        Status = HCC_RPC_STATUS_RESOURCE_EXHAUSTED;
        break;
    case 401:
        Status = HCC_RPC_STATUS_UNAUTHENTICATED;
        break;
    case 501:
        Status = HCC_RPC_STATUS_UNIMPLEMENTED;
        break;
    case 503:
        Status = HCC_RPC_STATUS_UNAVAILABLE;
        break;
    default:
        break;
    }

    return Status;
}

nlohmann::json HccRpcPostRequestWrapper(
    std::string const& AccessToken,
    std::string const& ApiPath,
    nlohmann::json const& Request)
{
    std::string ResponseJson;
    HCC_RPC_STATUS Status = ::HccRpcPostRequest(
        AccessToken,
        ApiPath,
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

int main()
{
    std::printf(
        "ApplicationDataRootPath = \"%s\"\n",
        HalalCloud::GetApplicationDataRootPath().string().data());

    HalalCloud::Session Session;

    Session.Authenticate([](
        std::string_view AuthenticationUri)
    {
        std::printf(
            "AuthenticationUri = \"%s\"\n",
            AuthenticationUri.data());

        std::wstring ConvertedReturnUrl =
            Mile::ToWideString(CP_UTF8, AuthenticationUri);

        SHELLEXECUTEINFOW ExecInfo = { 0 };
        ExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
        ExecInfo.lpVerb = L"open";
        ExecInfo.lpFile = ConvertedReturnUrl.c_str();
        ExecInfo.nShow = SW_SHOWNORMAL;
        ::ShellExecuteExW(&ExecInfo);

        std::printf("Waiting...\n");
    });

    std::printf("Login Success!\n");

    std::printf(
        "Token = \"%s\"\n",
        Session.CurrentToken().dump(2).c_str());

    Session.DownloadFile("/9p.cap", "D:\\9p.cap");

    /*Session.Impersonate(Mile::Json::ToString(
        Mile::Json::GetSubKey(Session.CurrentToken(), "refresh_token")));

    std::printf("Refresh Success!\n");

    std::printf(
        "Token = \"%s\"\n",
        Session.CurrentToken().dump(2).c_str());

    std::printf(
        "UserInformation = %s\n",
        Session.GetUserInformation().dump(2).c_str());

    Session.UploadFile(
        "D:\\Updates\\9p.cap",
        "/9p20240925.cap");

    Session.CreateFolder("/Test20240923");

    std::vector<HalalCloud::FileInformation> Files =
        Session.EnumerateFiles("/");
    for(HalalCloud::FileInformation const& File : Files)
    {
        std::time_t Time = File.CreationTime / 1000;
        std::tm TimeInfo = { 0 };
        ::localtime_s(&TimeInfo, &Time);
        std::printf(
            "%04d/%02d/%02d %02d:%02d\t%s\t%lld\t%s\n",
            TimeInfo.tm_year + 1900,
            TimeInfo.tm_mon + 1,
            TimeInfo.tm_mday,
            TimeInfo.tm_hour,
            TimeInfo.tm_min,
            File.FileAttributes.Fields.IsDirectory ? "<DIR>" : "",
            File.FileSize,
            File.FileName.c_str());
    }*/

    std::string MountPoint = "z:";

    fuse_args Arguments = FUSE_ARGS_INIT(0, nullptr);
    ::fuse_opt_add_arg(&Arguments, "HalalCloud");
    ::fuse_opt_add_arg(&Arguments, "-ovolname=HalalCloud");

    fuse_operations Operations = { 0 };
    Operations.open = ::HccFuseOpenCallback;
    Operations.read = ::HccFuseReadCallback;
    Operations.statfs = ::HccFuseStatFsCallback;
    Operations.getattr = ::HccFuseGetAttributesCallback;
    Operations.readdir = ::HccFuseReadDirectoryCallback;
    Operations.init = ::HccFuseInitializeCallback;

    fuse_chan* Channel = ::fuse_mount(MountPoint.c_str(), &Arguments);
    if (Channel)
    {
        fuse* Instance = ::fuse_new(
            Channel,
            &Arguments,
            &Operations,
            sizeof(Operations),
            &Session);
        if (Instance)
        {
            int Result = ::fuse_loop_mt(Instance);
            if (0 != Result)
            {
                std::printf("fuse_loop_mt failed!\n");
            }

            ::fuse_unmount(MountPoint.c_str(), Channel);
            ::fuse_destroy(Instance);
        }
        else
        {
            std::printf("fuse_new failed!\n");
        }
    }
    else
    {
        std::printf("fuse_mount failed!\n");
    }

    /*auto Yolo = Session.GetFileStorageInformation("/9p.cap");
    Yolo = Yolo;*/

    Session.Logout();

    std::printf("Logout Success!\n");

    std::printf(
        "================================================================\n"
        "HalalCloud.Client.Playground has been completed.\n");
    std::getchar();

    return 0;
}
