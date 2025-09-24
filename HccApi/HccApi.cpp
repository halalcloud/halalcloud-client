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

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/md.h>

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
