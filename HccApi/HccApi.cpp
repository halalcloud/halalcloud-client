/*
 * PROJECT:    Halal Cloud Client Library
 * FILE:       HccApi.cpp
 * PURPOSE:    Implementation for Halal Cloud Client Library
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "HccApi.h"

#ifdef _WIN32
#include <Mile.Helpers.CppBase.h>
#endif // _WIN32

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>
#include <string_view>

#include <curl/curl.h>
#include <mbedtls/base64.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/md.h>

#include "Multiformats.h"

EXTERN_C MO_POINTER MOAPI HccAllocateMemory(
    _In_ MO_UINTN Size)
{
#ifdef _WIN32
    return ::MileAllocateMemory(Size);
#else
    return std::calloc(1, Size);
#endif
}

EXTERN_C VOID MOAPI HccFreeMemory(
    _In_ MO_POINTER Block)
{
#ifdef _WIN32
    ::MileFreeMemory(Block);
#else
    std::free(Block);
#endif
}

EXTERN_C VOID MOAPI HccXorBufferWithByte(
    _Inout_ MO_POINTER Buffer,
    _In_ MO_UINT32 BufferSize,
    _In_ MO_UINT8 XorByte)
{
    // When XorByte is 0, the buffer will remain unchanged. So we also skip the
    // operation in this case.
    if (!Buffer || !BufferSize || !XorByte)
    {
        return;
    }

    const MO_UINT32 NativeVariableSize = sizeof(MO_UINTN);
    MO_UINT32 UnalignedSize = BufferSize % NativeVariableSize;
    MO_UINT32 AlignedSize = BufferSize - UnalignedSize;
    MO_UINT32 AlignedCount = AlignedSize / NativeVariableSize;

    MO_UINTN AlignedXorValue = 0;
    for (MO_UINT32 i = 0; i < NativeVariableSize; ++i)
    {
        AlignedXorValue |= (static_cast<MO_UINTN>(XorByte) << (i * 8));
    }

    PMO_UINTN AlignedBuffer = reinterpret_cast<PMO_UINTN>(Buffer);
    for (MO_UINT32 i = 0; i < AlignedCount; ++i)
    {
        AlignedBuffer[i] ^= AlignedXorValue;
    }

    PMO_UINT8 UnalignedBuffer = reinterpret_cast<PMO_UINT8>(
        AlignedBuffer + AlignedCount);
    for (MO_UINT32 i = 0; i < UnalignedSize; ++i)
    {
        UnalignedBuffer[i] ^= XorByte;
    }
}

EXTERN_C MO_RESULT MOAPI HccComputeSha256(
    _Out_ MO_POINTER OutputBuffer,
    _In_ MO_CONSTANT_POINTER InputBuffer,
    _In_ MO_UINT32 InputSize)
{
    if (!OutputBuffer || !InputBuffer || !InputSize)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    MO_RESULT Result = MO_RESULT_ERROR_FAIL;

    mbedtls_md_context_t Context;
    ::mbedtls_md_init(&Context);

    if (0 == ::mbedtls_md_setup(
        &Context,
        ::mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
        0))
    {
        if (0 == ::mbedtls_md_starts(&Context))
        {
            if (0 == ::mbedtls_md_update(
                &Context,
                reinterpret_cast<const unsigned char*>(InputBuffer),
                InputSize))
            {
                if (0 == ::mbedtls_md_finish(
                    &Context,
                    reinterpret_cast<unsigned char*>(OutputBuffer)))
                {
                    Result = MO_RESULT_SUCCESS_OK;
                }
            }
        }
    }

    ::mbedtls_md_free(&Context);

    return Result;
}

EXTERN_C MO_RESULT MOAPI HccComputeHmacSha256(
    _Out_ MO_POINTER OutputBuffer,
    _In_ MO_CONSTANT_POINTER KeyBuffer,
    _In_ MO_UINT32 KeySize,
    _In_ MO_CONSTANT_POINTER InputBuffer,
    _In_ MO_UINT32 InputSize)
{
    if (!OutputBuffer || !KeyBuffer || !KeySize || !InputBuffer || !InputSize)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    MO_RESULT Result = MO_RESULT_ERROR_FAIL;

    mbedtls_md_context_t Context;
    ::mbedtls_md_init(&Context);

    if (0 == ::mbedtls_md_setup(
        &Context,
        ::mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
        1))
    {
        if (0 == ::mbedtls_md_hmac_starts(
            &Context,
            reinterpret_cast<const unsigned char*>(KeyBuffer),
            KeySize))
        {
            if (0 == ::mbedtls_md_hmac_update(
                &Context,
                reinterpret_cast<const unsigned char*>(InputBuffer),
                InputSize))
            {
                if (0 == ::mbedtls_md_hmac_finish(
                    &Context,
                    reinterpret_cast<unsigned char*>(OutputBuffer)))
                {
                    Result = MO_RESULT_SUCCESS_OK;
                }
            }
        }
    }

    ::mbedtls_md_free(&Context);

    return Result;
}

EXTERN_C MO_RESULT MOAPI HccGenerateRandomBytes(
    _Out_ MO_POINTER OutputBuffer,
    _In_ MO_UINT32 OutputSize)
{
    if (!OutputBuffer || !OutputSize)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    MO_RESULT Result = MO_RESULT_ERROR_FAIL;

    mbedtls_entropy_context EntropyContext;
    ::mbedtls_entropy_init(&EntropyContext);

    mbedtls_ctr_drbg_context CtrDrbgContext;
    ::mbedtls_ctr_drbg_init(&CtrDrbgContext);
    ::mbedtls_ctr_drbg_set_prediction_resistance(
        &CtrDrbgContext,
        MBEDTLS_CTR_DRBG_PR_ON);

    if (0 == ::mbedtls_ctr_drbg_seed(
        &CtrDrbgContext,
        ::mbedtls_entropy_func,
        &EntropyContext,
        nullptr,
        0))
    {
        if (0 == ::mbedtls_ctr_drbg_random(
            &CtrDrbgContext,
            reinterpret_cast<unsigned char*>(OutputBuffer),
            OutputSize))
        {
            Result = MO_RESULT_SUCCESS_OK;
        }
    }

    ::mbedtls_ctr_drbg_free(&CtrDrbgContext);

    ::mbedtls_entropy_free(&EntropyContext);

    return Result;
}

EXTERN_C MO_RESULT MOAPI HccEncodeBase64(
    _Out_ PMO_STRING OutputString,
    _In_ MO_CONSTANT_POINTER InputBuffer,
    _In_ MO_UINT32 InputSize)
{
    if (!OutputString || !InputBuffer || !InputSize)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    std::size_t OutputSize = 0;
    if (MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL != ::mbedtls_base64_encode(
        nullptr,
        0,
        &OutputSize,
        reinterpret_cast<const unsigned char*>(InputBuffer),
        InputSize))
    {
        return MO_RESULT_ERROR_FAIL;
    }

    *OutputString = reinterpret_cast<MO_STRING>(
        ::HccAllocateMemory(OutputSize * sizeof(MO_CHAR)));
    if (!*OutputString)
    {
        return MO_RESULT_ERROR_OUT_OF_MEMORY;
    }

    if (0 != ::mbedtls_base64_encode(
        reinterpret_cast<unsigned char*>(*OutputString),
        OutputSize,
        &OutputSize,
        reinterpret_cast<const unsigned char*>(InputBuffer),
        InputSize))
    {
        ::HccFreeMemory(*OutputString);
        *OutputString = nullptr;
        return MO_RESULT_ERROR_FAIL;
    }

    (*OutputString)[OutputSize] = '\0';
    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI HccEncodeBase64UrlSafe(
    _Out_ PMO_STRING OutputString,
    _In_ MO_CONSTANT_POINTER InputBuffer,
    _In_ MO_UINT32 InputSize)
{
    MO_RESULT Result = ::HccEncodeBase64(OutputString, InputBuffer, InputSize);
    if (MO_RESULT_SUCCESS_OK != Result)
    {
        return Result;
    }

    std::size_t Length = std::strlen(*OutputString);
    for (std::size_t i = 0; i < Length; ++i)
    {
        if ('+' == (*OutputString)[i])
        {
            (*OutputString)[i] = '-';
        }
        else if ('/' == (*OutputString)[i])
        {
            (*OutputString)[i] = '_';
        }
        else if ('=' == (*OutputString)[i])
        {
            (*OutputString)[i] = '\0';
        }
    }

    return Result;
}

namespace
{
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
        std::vector<std::uint8_t> Result(HCC_SHA256_HASH_LENGTH);

        if (MO_RESULT_SUCCESS_OK != ::HccComputeSha256(
            Result.data(),
            Data.c_str(),
            static_cast<MO_UINT32>(Data.size() * sizeof(char))))
        {
            return {};
        }

        return ::BytesToHexString(Result);
    }

    std::vector<std::uint8_t> ComputeHmacSha256(
        std::vector<std::uint8_t> const& Key,
        std::string const& Data)
    {
        std::vector<std::uint8_t> Result(HCC_SHA256_HASH_LENGTH);

        if (MO_RESULT_SUCCESS_OK != ::HccComputeHmacSha256(
            Result.data(),
            Key.data(),
            static_cast<MO_UINT32>(Key.size()),
            Data.c_str(),
            static_cast<MO_UINT32>(Data.size() * sizeof(char))))
        {
            return {};
        }

        return Result;
    }

    std::string GenerateNonce()
    {
        GUID Guid = {};

        if (MO_RESULT_SUCCESS_OK != ::HccGenerateRandomBytes(
            &Guid,
            sizeof(GUID)))
        {
            return {};
        }

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
        std::tm UtcTime = {};
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
}

EXTERN_C HCC_RPC_STATUS MOAPI HccRpcPostRequest(
    _Out_opt_ PMO_STRING ResponseJsonString,
    _In_ MO_CONSTANT_STRING AccessTokenString,
    _In_ MO_CONSTANT_STRING ApiPathString,
    _In_ MO_CONSTANT_STRING RequestJsonString)
{
    if (!AccessTokenString || !ApiPathString || !RequestJsonString)
    {
        return HCC_RPC_STATUS_INVALID_ARGUMENT;
    }

    std::string AccessToken(AccessTokenString);
    std::string ApiPath(ApiPathString);
    std::string RequestJson(RequestJsonString);

    const char RequestSuffix[] = "hl6_request";
    const char SignaturePrefix[] = "HL6";
    const char DefaultSignatureAlgorithm[] = "HL6-HMAC-SHA256";

    const char SecretId[] = "puc_5_dcpqfdvu8qnt_v1";
    const char SecretKey[] = "c8303e83dc164352a38289409681df8d";

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

    std::string ResponseJson;
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

    if (ResponseJsonString)
    {
        std::size_t BufferSize = (ResponseJson.size() + 1) * sizeof(MO_CHAR);

        *ResponseJsonString = reinterpret_cast<MO_STRING>(
            ::HccAllocateMemory(BufferSize));
        if (!*ResponseJsonString)
        {
            return HCC_RPC_STATUS_INTERNAL;
        }

        std::memcpy(*ResponseJsonString, ResponseJson.c_str(), BufferSize);
    }

    return Status;
}

namespace
{
    static FILE* CrtFileOpen(
        char const* FileName,
        char const* Mode)
    {
#ifdef _WIN32
        FILE* FileStream = nullptr;
        return (
            0 == ::_wfopen_s(
                &FileStream,
                Mile::ToWideString(CP_UTF8, FileName).c_str(),
                Mile::ToWideString(CP_UTF8, Mode).c_str())) ?
            FileStream :
            nullptr;
#else
        return std::fopen(FileName, Mode);
#endif
    }
}

EXTERN_C MO_RESULT MOAPI HccDownloadFile(
    _In_ MO_CONSTANT_STRING SourceUrl,
    _In_ MO_CONSTANT_STRING TargetPath)
{
    if (!SourceUrl || !TargetPath)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    MO_RESULT Result = MO_RESULT_ERROR_FAIL;
    CURL* CurlHandle = nullptr;
    FILE* FileStream = nullptr;

    do
    {
        CurlHandle = ::curl_easy_init();
        if (!CurlHandle)
        {
            break;
        }

        if (CURLE_OK != ::curl_easy_setopt(
            CurlHandle,
            CURLOPT_FOLLOWLOCATION,
            CURLFOLLOW_ALL))
        {
            break;
        }

        if (CURLE_OK != ::curl_easy_setopt(
            CurlHandle,
            CURLOPT_SSL_VERIFYPEER,
            0L))
        {
            break;
        }

        if (CURLE_OK != ::curl_easy_setopt(
            CurlHandle,
            CURLOPT_URL,
            SourceUrl))
        {
            break;
        }

        FileStream = ::CrtFileOpen(TargetPath, "wb");
        if (!FileStream)
        {
            break;
        }

        if (CURLE_OK != ::curl_easy_setopt(
            CurlHandle,
            CURLOPT_WRITEDATA,
            FileStream))
        {
            break;
        }

        if (CURLE_OK != ::curl_easy_perform(CurlHandle))
        {
            break;
        }

        long ResponseCode = 0;
        if (CURLE_OK != ::curl_easy_getinfo(
            CurlHandle,
            CURLINFO_RESPONSE_CODE,
            &ResponseCode))
        {
            break;
        }

        if (200 != ResponseCode)
        {
            break;
        }

        Result = MO_RESULT_SUCCESS_OK;

    } while (false);

    if (FileStream)
    {
        std::fclose(FileStream);
    }

    if (CurlHandle)
    {
        ::curl_easy_cleanup(CurlHandle);
    }

    return Result;
}

namespace
{
    std::vector<std::uint8_t> DecodeBase32Rfc4648(
        std::string_view Input)
    {
        std::vector<std::uint8_t> Result;
        std::size_t ReservedSize = ((Input.size() * 5) / 8) + 1;
        Result.reserve(ReservedSize);

        std::uint64_t Buffer = 0;
        std::uint8_t BitsInBuffer = 0;

        for (char const& Character : Input)
        {
            if ('=' == Character)
            {
                // Stop processing when padding character is found.
                break;
            }

            std::uint8_t Current = 0;
            if (Character >= 'A' && Character <= 'Z')
            {
                Current = Character - 'A';
            }
            else if (Character >= 'a' && Character <= 'z')
            {
                Current = Character - 'a';
            }
            else if (Character >= '2' && Character <= '7')
            {
                Current = Character - '2' + 26;
            }
            else
            {
                // Invalid character found.
                return {};
            }

            Buffer = (Buffer << 5) | Current;
            BitsInBuffer += 5;

            if (BitsInBuffer < 8)
            {
                continue;
            }
            BitsInBuffer -= 8;
            Result.push_back(static_cast<std::uint8_t>(Buffer >> BitsInBuffer));
        }

        return Result;
    }
}

EXTERN_C MO_BOOL MOAPI HccCidGetSha256(
    _In_ MO_CONSTANT_STRING CidString,
    _Out_ MO_POINTER HashBytes)
{
    if (!CidString || !HashBytes)
    {
        return MO_FALSE;
    }

    std::string_view CidStringView(CidString);
    if (CidStringView.empty() ||
        HCC_CID_STRING_LENGTH != CidStringView.size() ||
        'b' != CidStringView.front())
    {
        return MO_FALSE;
    }

    std::vector<std::uint8_t> Content = ::DecodeBase32Rfc4648(
        CidStringView.substr(1));
    if (HCC_CID_CONTENT_LENGTH != Content.size() ||
        MULTICODEC_TYPE_CIDV1 != Content[0] ||
        MULTICODEC_TYPE_IPLD_RAW != Content[1] ||
        MULTIHASH_TYPE_SHA2_256 != Content[2] ||
        HCC_SHA256_HASH_LENGTH != Content[3])
    {
        return MO_FALSE;
    }

    // Copy the SHA-256 hash value to the output buffer. Let it crash if the
    // caller cannot ensure that the output buffer is at least
    // HCC_SHA256_HASH_LENGTH bytes.
    std::memcpy(HashBytes, Content.data() + 4, HCC_SHA256_HASH_LENGTH);

    return MO_TRUE;
}
