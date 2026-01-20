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

#include <fstream>

#include <HccApi.h>

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

std::vector<std::uint8_t> HalalCloud::Session::AcquireBlock(
    HalalCloud::BlockStorageInformation const& BlockInfo,
    std::int64_t const& ExpectedSize)
{
    std::lock_guard<std::mutex> Lock(this->m_CachedBlocksMutex);

    std::filesystem::path BlockCachePath =
        HalalCloud::GetBlocksCachePath() / BlockInfo.Identifier;

    MO_BOOL CachedHashExisted = MO_FALSE;
    MO_UINT8 CachedHashBytes[HCC_SHA256_HASH_LENGTH] = {};
    {
        auto Iterator = this->m_CachedBlocks.find(BlockInfo.Identifier);
        if (this->m_CachedBlocks.end() != Iterator)
        {
            if (Iterator->second.HashExisted)
            {
                std::memcpy(
                    CachedHashBytes,
                    Iterator->second.HashBytes,
                    HCC_SHA256_HASH_LENGTH);
                CachedHashExisted = MO_TRUE;
            }
        }
    }

    {
        std::vector<std::uint8_t> Bytes = HalalCloud::ReadAllBytesFromFile(
            BlockCachePath.string());
        if (Bytes.size() == static_cast<std::size_t>(ExpectedSize))
        {
            if (CachedHashExisted)
            {
                MO_UINT8 ComputedHashBytes[HCC_SHA256_HASH_LENGTH] = {};
                if (MO_RESULT_SUCCESS_OK == ::HccComputeSha256(
                    ComputedHashBytes,
                    Bytes.data(),
                    static_cast<MO_UINT32>(Bytes.size())))
                {
                    if (0 == std::memcmp(
                        CachedHashBytes,
                        ComputedHashBytes,
                        HCC_SHA256_HASH_LENGTH))
                    {
                        ::HccXorBufferWithByte(
                            Bytes.data(),
                            static_cast<MO_UINT32>(Bytes.size()),
                            BlockInfo.EncryptionByte);

                        return Bytes;
                    }
                }

                CachedHashExisted = MO_FALSE;
                std::memset(CachedHashBytes, 0, HCC_SHA256_HASH_LENGTH);
                this->m_CachedBlocks.erase(BlockInfo.Identifier);
            }
            else
            {
                if (MO_RESULT_SUCCESS_OK == ::HccComputeSha256(
                    CachedHashBytes,
                    Bytes.data(),
                    static_cast<MO_UINT32>(Bytes.size())))
                {
                    ::HccXorBufferWithByte(
                        Bytes.data(),
                        static_cast<MO_UINT32>(Bytes.size()),
                        BlockInfo.EncryptionByte);

                    MO_UINT8 ContentHashBytes[HCC_SHA256_HASH_LENGTH] = {};
                    if (::HccCidGetSha256(
                        ContentHashBytes,
                        BlockInfo.Identifier.c_str()))
                    {
                        MO_UINT8 ComputedHashBytes[HCC_SHA256_HASH_LENGTH] = {};
                        if (MO_RESULT_SUCCESS_OK == ::HccComputeSha256(
                            ComputedHashBytes,
                            Bytes.data(),
                            static_cast<MO_UINT32>(Bytes.size())))
                        {
                            if (0 == std::memcmp(
                                ContentHashBytes,
                                ComputedHashBytes,
                                HCC_SHA256_HASH_LENGTH))
                            {
                                HalalCloud::CachedBlockItem CachedItem = {};
                                std::memcpy(
                                    CachedItem.HashBytes,
                                    CachedHashBytes,
                                    HCC_SHA256_HASH_LENGTH);
                                CachedItem.HashExisted = MO_TRUE;
                                this->m_CachedBlocks.emplace(
                                    BlockInfo.Identifier,
                                    CachedItem);
                                return Bytes;
                            }
                        }
                    }
                    else
                    {
                        // For legacy Baidu Object Storage, it doesn't have CID.
                        // So, only check the size.
                        HalalCloud::CachedBlockItem CachedItem = {};
                        std::memcpy(
                            CachedItem.HashBytes,
                            CachedHashBytes,
                            HCC_SHA256_HASH_LENGTH);
                        CachedItem.HashExisted = MO_TRUE;
                        this->m_CachedBlocks.emplace(
                            BlockInfo.Identifier,
                            CachedItem);
                        return Bytes;
                    }
                }
            }
        }
    }

    if (MO_RESULT_SUCCESS_OK == ::HccDownloadFile(
        BlockInfo.DownloadLink.c_str(),
        BlockCachePath.string().c_str()))
    {
        std::vector<std::uint8_t> Bytes = HalalCloud::ReadAllBytesFromFile(
            BlockCachePath.string());
        if (Bytes.size() == static_cast<std::size_t>(ExpectedSize))
        {
            if (MO_RESULT_SUCCESS_OK == ::HccComputeSha256(
                CachedHashBytes,
                Bytes.data(),
                static_cast<MO_UINT32>(Bytes.size())))
            {
                ::HccXorBufferWithByte(
                    Bytes.data(),
                    static_cast<MO_UINT32>(Bytes.size()),
                    BlockInfo.EncryptionByte);

                MO_UINT8 ContentHashBytes[HCC_SHA256_HASH_LENGTH] = {};
                if (::HccCidGetSha256(
                    ContentHashBytes,
                    BlockInfo.Identifier.c_str()))
                {
                    MO_UINT8 ComputedHashBytes[HCC_SHA256_HASH_LENGTH] = {};
                    if (MO_RESULT_SUCCESS_OK == ::HccComputeSha256(
                        ComputedHashBytes,
                        Bytes.data(),
                        static_cast<MO_UINT32>(Bytes.size())))
                    {
                        if (0 == std::memcmp(
                            ContentHashBytes,
                            ComputedHashBytes,
                            HCC_SHA256_HASH_LENGTH))
                        {
                            HalalCloud::CachedBlockItem CachedItem = {};
                            std::memcpy(
                                CachedItem.HashBytes,
                                CachedHashBytes,
                                HCC_SHA256_HASH_LENGTH);
                            CachedItem.HashExisted = MO_TRUE;
                            this->m_CachedBlocks.emplace(
                                BlockInfo.Identifier,
                                CachedItem);
                            return Bytes;
                        }
                    }
                }
                else
                {
                    // For legacy Baidu Object Storage, it doesn't have CID.
                    // So, only check the size.
                    HalalCloud::CachedBlockItem CachedItem = {};
                    std::memcpy(
                        CachedItem.HashBytes,
                        CachedHashBytes,
                        HCC_SHA256_HASH_LENGTH);
                    CachedItem.HashExisted = MO_TRUE;
                    this->m_CachedBlocks.emplace(
                        BlockInfo.Identifier,
                        CachedItem);
                    return Bytes;
                }
            }
        }
    }

    return std::vector<std::uint8_t>();
}

HalalCloud::UserToken& HalalCloud::Session::CurrentToken()
{
    return this->m_CurrentToken;
}

nlohmann::json HalalCloud::Session::Request(
    std::string_view MethodFullName,
    nlohmann::json const& Request)
{
    if (this->m_CurrentToken.AccessToken.empty())
    {
        return nlohmann::json::parse(HalalCloud::RequestWithoutToken(
            MethodFullName,
            Request.dump()));
    }
    else
    {
        return nlohmann::json::parse(HalalCloud::Request(
            this->m_CurrentToken,
            MethodFullName,
            Request.dump()));
    }
}

void HalalCloud::Session::Authenticate(
    std::function<void(std::string_view)> Callback)
{
    std::string CodeVerifier = HalalCloud::GenerateCodeVerifier();

    std::string Code;
    std::string RedirectUri;
    HalalCloud::Authorize(Code, RedirectUri, CodeVerifier);

    Callback(RedirectUri);

    while (true)
    {
        HalalCloud::AuthorizeState State =
            HalalCloud::GetAuthorizeState(Code);
        if (HalalCloud::AuthorizeState::Failed == State)
        {
            HalalCloud::ThrowException(
                "Authorization Failed",
                -1);
        }

        if (HalalCloud::AuthorizeState::TokenCreated == State)
        {
            this->m_CurrentToken = HalalCloud::GetToken(Code, CodeVerifier);
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void HalalCloud::Session::Impersonate(
    std::string_view RefreshToken)
{
    this->m_CurrentToken = HalalCloud::RefreshToken(RefreshToken);
}

void HalalCloud::Session::Logout()
{
    HalalCloud::Logoff(this->m_CurrentToken);
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
    return HalalCloud::GetFileInformation(
        this->m_CurrentToken,
        Path);
}

std::vector<HalalCloud::FileInformation> HalalCloud::Session::EnumerateFiles(
    std::string_view Path)
{
    return HalalCloud::GetFileList(
        this->m_CurrentToken,
        Path);
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
        MO_UINT8 OutputHashBytes[HCC_SHA256_HASH_LENGTH] = {};
        if (::HccCidGetSha256(
            OutputHashBytes,
            Block.Identifier.c_str()))
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
