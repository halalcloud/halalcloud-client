/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HccProtocolWrappers.h
 * PURPOSE:    Definition for Halal Cloud Client Protocol Wrappers
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef HALALCLOUD_CLIENT_PROTOCOL_WRAPPERS
#define HALALCLOUD_CLIENT_PROTOCOL_WRAPPERS

#include <HccApi.h>

#include <string_view>

namespace HalalCloud
{
    [[noreturn]] void ThrowException(
        std::string_view Checkpoint,
        std::int32_t const& Code);
}

#endif // !HALALCLOUD_CLIENT_PROTOCOL_WRAPPERS
