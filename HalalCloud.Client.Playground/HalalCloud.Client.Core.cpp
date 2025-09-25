/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HalalCloud.Client.Core.h
 * PURPOSE:    Definition for the HalalCloud.Client.Core
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

// Ensure the sal.h which is included by Windows C runtime used by
// Mile.Helpers.CppBase.h will be included before Mile.Mobility.Portable.Types.h
// which is included by HalalCloud.Client.Core.h.
#include <Mile.Helpers.CppBase.h>

#include "HalalCloud.Client.Core.h"

#include "HalalCloud.Specification.Multiformats.h"

#include <ctime>
#include <map>
#include <span>
#include <string_view>

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

MO_BOOL HccCidGetSha256(
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
