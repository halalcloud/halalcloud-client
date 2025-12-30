/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HccProtocolWrappers.cpp
 * PURPOSE:    Implementation for Halal Cloud Client Protocol Wrappers
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "HccProtocolWrappers.h"

#include <Mile.Helpers.CppBase.h>

#include <stdexcept>

[[noreturn]] void HalalCloud::ThrowException(
    std::string_view Checkpoint,
    std::int32_t const& Code)
{
    throw std::runtime_error(Mile::FormatString(
        "[HalalCloud.Client] %s failed with code %d.",
        Checkpoint.data(),
        Code));
}

std::string HalalCloud::Request(
    std::string_view AccessToken,
    std::string_view MethodFullName,
    std::string_view RequestJson)
{
    MO_STRING RawResponseJson = nullptr;
    HCC_RPC_STATUS Status = ::HccRpcPostRequest(
        &RawResponseJson,
        AccessToken.data(),
        MethodFullName.data(),
        RequestJson.data());
    if (HCC_RPC_STATUS_OK != Status)
    {
        HalalCloud::ThrowException(
            "HalalCloud::Request!HccRpcPostRequest",
            Status);
    }
    std::string ResponseJson = std::string(RawResponseJson);
    ::HccFreeMemory(RawResponseJson);
    return ResponseJson;
}
