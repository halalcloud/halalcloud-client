/*
 * PROJECT:    Halal Cloud Client Library
 * FILE:       HccApi.h
 * PURPOSE:    Definition for Halal Cloud Client Library
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef HALALCLOUD_CLIENT_API
#define HALALCLOUD_CLIENT_API

#include <Mile.Mobility.Portable.Types.h>
#ifndef MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <sal.h>
#endif // !MILE_MOBILITY_ENABLE_MINIMUM_SAL

// Workaround for Windows SDK, which defines these types in #ifndef VOID block.
// That design is terrible, but needs to have a workaround.
#ifdef VOID
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif // VOID

// The length of a SHA-256 hash value in bytes.
#define HCC_SHA256_HASH_LENGTH 32

/**
 * @brief Compute the SHA-256 hash value for the input buffer.
 * @param OutputBuffer The output buffer which must be at least
 *                     HCC_SHA256_HASH_LENGTH bytes long.
 * @param InputBuffer The input buffer.
 * @param InputSize The size of the input buffer in bytes.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI HccComputeSha256(
    _Out_ MO_POINTER OutputBuffer,
    _In_ MO_CONSTANT_POINTER InputBuffer,
    _In_ MO_UINT32 InputSize);

/**
 * @brief Compute the HMAC-SHA256 for the input buffer using the specified key.
 * @param OutputBuffer The output buffer which must be at least
 *                     HCC_SHA256_HASH_LENGTH bytes long.
 * @param KeyBuffer The key buffer.
 * @param KeySize The size of the key buffer in bytes.
 * @param InputBuffer The input buffer.
 * @param InputSize The size of the input buffer in bytes.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI HccComputeHmacSha256(
    _Out_ MO_POINTER OutputBuffer,
    _In_ MO_CONSTANT_POINTER KeyBuffer,
    _In_ MO_UINT32 KeySize,
    _In_ MO_CONSTANT_POINTER InputBuffer,
    _In_ MO_UINT32 InputSize);

/**
 * @brief Generate cryptographically secure random bytes.
 * @param OutputBuffer The output buffer.
 * @param OutputSize The size of the output buffer in bytes.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI HccGenerateRandomBytes(
    _Out_ MO_POINTER OutputBuffer,
    _In_ MO_UINT32 OutputSize);

#endif // !HALALCLOUD_CLIENT_API
