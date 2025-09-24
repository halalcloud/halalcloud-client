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

/**
 * @brief Allocates a block of memory from the default heap of the calling
 *        process. The allocated memory will be initialized to zero. The
 *        allocated memory is not movable.
 * @param Size The number of bytes to be allocated.
 * @return If the function succeeds, the return value is a pointer to the
 *         allocated memory block. If the function fails, the return value is
 *         nullptr.
 */
EXTERN_C MO_POINTER MOAPI HccAllocateMemory(
    _In_ MO_UINTN Size);

/**
 * @brief Frees a memory block allocated from a heap by the HccAllocateMemory
 *        function.
 * @param Block A pointer to the memory block to be freed. This pointer is
 *        returned by the Allocate or Reallocate method. If this pointer is
 *        nullptr, the behavior is undefined.
 * @return This function does not return a value.
 */
EXTERN_C VOID MOAPI HccFreeMemory(
    _In_ MO_POINTER Block);

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

#endif // !HALALCLOUD_CLIENT_API
