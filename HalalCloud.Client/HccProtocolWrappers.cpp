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
