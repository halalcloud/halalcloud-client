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

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

typedef MO_INT32 HCC_RPC_STATUS;

// Not an error; returned on success.
#define HCC_RPC_STATUS_OK ((HCC_RPC_STATUS)0)

// The operation was cancelled (typically by the caller).
#define HCC_RPC_STATUS_CANCELLED ((HCC_RPC_STATUS)1)

// Unknown error.  An example of where this error may be returned is if a Status
// value received from another address space belongs to an error-space that is
// not known in this address space. Also errors raised by APIs that do not
// return enough error information may be converted to this error.
#define HCC_RPC_STATUS_UNKNOWN ((HCC_RPC_STATUS)2)

// Client specified an invalid argument.  Note that this differs from
// HCC_RPC_STATUS_FAILED_PRECONDITION. HCC_RPC_STATUS_INVALID_ARGUMENT indicates
// arguments that are problematic regardless of the state of the system (e.g., a
// malformed file name).
#define HCC_RPC_STATUS_INVALID_ARGUMENT ((HCC_RPC_STATUS)3)

// Deadline expired before operation could complete. For operations that change
// the state of the system, this error may be returned even if the operation has
// completed successfully. For example, a successful response from a server
// could have been delayed long enough for the deadline to expire.
#define HCC_RPC_STATUS_DEADLINE_EXCEEDED ((HCC_RPC_STATUS)4)

// Some requested entity (e.g., file or directory) was not found.
#define HCC_RPC_STATUS_NOT_FOUND ((HCC_RPC_STATUS)5)

// Some entity that we attempted to create (e.g., file or directory) already
// exists.
#define HCC_RPC_STATUS_ALREADY_EXISTS ((HCC_RPC_STATUS)6)

// The caller does not have permission to execute the specified operation.
// HCC_RPC_STATUS_PERMISSION_DENIED must not be used for rejections caused by
// exhausting some resource (use HCC_RPC_STATUS_RESOURCE_EXHAUSTED instead for
// those errors). HCC_RPC_STATUS_PERMISSION_DENIED must not be used if the
// caller can not be identified (use HCC_RPC_STATUS_UNAUTHENTICATED instead for
// those errors).
#define HCC_RPC_STATUS_PERMISSION_DENIED ((HCC_RPC_STATUS)7)

// Some resource has been exhausted, perhaps a per-user quota, or perhaps the
// entire file system is out of space.
#define HCC_RPC_STATUS_RESOURCE_EXHAUSTED ((HCC_RPC_STATUS)8)

// Operation was rejected because the system is not in a state required for the
// operation's execution.  For example, directory to be deleted may be
// non-empty, an rmdir operation is applied to a non-directory, etc.
#define HCC_RPC_STATUS_FAILED_PRECONDITION ((HCC_RPC_STATUS)9)

// The operation was aborted, typically due to a concurrency issue like
// sequencer check failures, transaction aborts, etc.
#define HCC_RPC_STATUS_ABORTED ((HCC_RPC_STATUS)10)

// Operation was attempted past the valid range. E.g., seeking or reading past
// end of file.
#define HCC_RPC_STATUS_OUT_OF_RANGE ((HCC_RPC_STATUS)11)

// Operation is not implemented or not supported/enabled in this service.
#define HCC_RPC_STATUS_UNIMPLEMENTED ((HCC_RPC_STATUS)12)

// Internal errors. Means some invariants expected by underlying system has been
// broken. If you see one of these errors, something is very broken.
#define HCC_RPC_STATUS_INTERNAL ((HCC_RPC_STATUS)13)

// The service is currently unavailable.  This is a most likely a transient
// condition and may be corrected by retrying with a backoff. Note that it
// is not always safe to retry non-idempotent operations.
#define HCC_RPC_STATUS_UNAVAILABLE ((HCC_RPC_STATUS)14)

// Unrecoverable data loss or corruption.
#define HCC_RPC_STATUS_DATA_LOSS ((HCC_RPC_STATUS)15)

// The request does not have valid authentication credentials for the operation.
#define HCC_RPC_STATUS_UNAUTHENTICATED ((HCC_RPC_STATUS)16)

HCC_RPC_STATUS HccRpcPostRequest(
    std::string const& AccessToken,
    std::string const& ApiPath,
    std::string const& RequestJson,
    std::string& ResponseJson);

/**
 * @brief The type of multibase encoding.
 */
typedef enum _HCC_MULTIBASE_ENCODING
{
    /**
     * @brief No base encoding
     */
    HCC_MULTIBASE_ENCODING_NONE,

    /**
     * @brief Unsupported base encoding
     */
    HCC_MULTIBASE_ENCODING_UNSUPPORTED,

    /**
     * @brief Hexadecimal (lowercase)
     */
    HCC_MULTIBASE_ENCODING_BASE16,

    /**
     * @brief Hexadecimal (uppercase)
     */
    HCC_MULTIBASE_ENCODING_BASE16_UPPER,

    /**
     * @brief RFC4648 case-insensitive - no padding (lowercase)
     */
    HCC_MULTIBASE_ENCODING_BASE32,

    /**
     * @brief RFC4648 case-insensitive - no padding (uppercase)
     */
    HCC_MULTIBASE_ENCODING_BASE32_UPPER,

    /**
     * @brief Base58 Bitcoin
     */
    HCC_MULTIBASE_ENCODING_BASE58_BTC,

    /**
     * @brief RFC4648 no padding
     */
    HCC_MULTIBASE_ENCODING_BASE64,

    /**
     * @brief RFC4648 no padding
     */
    HCC_MULTIBASE_ENCODING_BASE64_URL,

    /**
     * @brief RFC4648 with padding
     */
    HCC_MULTIBASE_ENCODING_BASE64_URL_PAD,

} HCC_MULTIBASE_ENCODING, *PHCC_MULTIBASE_ENCODING;

/**
 * @brief Information about a CID.
 */
typedef struct _HCC_CID_INFORMATION
{
    /**
     * @brief The multibase encoding of the CID.
     */
    HCC_MULTIBASE_ENCODING Encoding;

    /**
     * @brief The version of the CID.
     */
    MO_UINT64 Version;

    /**
     * @brief The content type of the CID..
     */
    MO_UINT64 ContentType;

    /**
     * @brief The hash type of the CID.
     */
    MO_UINT64 HashType;

    /**
     * @brief The hash value of the CID. Maximum length is 512 bits (64 bytes)
     *        should be enough for CID permanent hash types.
     */
    MO_UINT8 HashValue[64];

} HCC_CID_INFORMATION, *PHCC_CID_INFORMATION;

#endif // !HALALCLOUD_CLIENT_CORE
