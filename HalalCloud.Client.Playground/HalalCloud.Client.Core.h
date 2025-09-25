/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HalalCloud.Client.Core.h
 * PURPOSE:    Definition for the HalalCloud.Client.Core
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef HALALCLOUD_CLIENT_CORE
#define HALALCLOUD_CLIENT_CORE

// Temporarily
#include <string>

#include <HccApi.h>

// The length of the CID content used in the current Halal Cloud service
// implementation should be 36 bytes long, which is calculated as follows:
// - 1 byte for CID version
// - 1 byte for multicodec type
// - 1 byte for multihash type
// - 1 byte for the length of the hash value
// - 32 bytes for the SHA-256 hash value
#define HCC_CID_CONTENT_LENGTH (1 + 1 + 1 + 1 + HCC_SHA256_HASH_LENGTH)

// The CID string used in the current Halal Cloud service implementation should
// be 59 characters long, which is calculated as follows:
// - 1 character for multibase encoding
// - 58 characters for the Base32-encoded content which the content is
//   HCC_CID_CONTENT_LENGTH bytes long.
#define HCC_CID_STRING_LENGTH 59

// The length of a CID string buffer used in the current Halal Cloud service
// implementation. The buffer contains space for the CID string and a
// null-terminator.
#define HCC_CID_STRING_BUFFER_LENGTH (HCC_CID_STRING_LENGTH + 1)

/**
 * @brief Get the SHA-256 hash value from a CID string used in Halal Cloud.
 * @param CidString The input CID string, the caller must ensure that string is
 *                  null-terminated and encoded in UTF-8.
 * @param HashBytes The output buffer to receive the SHA-256 hash value, the
 *                  caller must ensure that the buffer is at least 32 bytes.
 * @return Returns MO_TRUE on success; otherwise, returns MO_FALSE.
 * @remark This function only supports CIDs used in the current Halal Cloud
 *         service implementation, which has the following properties:
 *         - Multibase encoding: Base32
 *         - CID version: CIDv1
 *         - Multicodec type: InterPlanetary Linked Data Format - Raw binary
 *         - Multihash type: SHA2-256
 *         - CID string length: HCC_CID_STRING_LENGTH
 */
MO_BOOL HccCidGetSha256(
    _In_ MO_CONSTANT_STRING CidString,
    _Out_ MO_POINTER HashBytes);

#endif // !HALALCLOUD_CLIENT_CORE
