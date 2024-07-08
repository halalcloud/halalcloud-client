/*
 * PROJECT:   Halal Cloud Client
 * FILE:      HalalCloud.Client.Core.Native.cpp
 * PURPOSE:   Implementation for the HalalCloud.Client.Core.Native
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "HalalCloud.Client.Core.Native.h"

#include <cstring>

EXTERN_C VOID WINAPI HccFreeToken(
    PHCC_TOKEN Value)
{
    if (Value)
    {
        ::CoTaskMemFree(Value->Identity);
        ::CoTaskMemFree(Value->UserIdentity);
        ::CoTaskMemFree(Value->Device);
        ::CoTaskMemFree(Value->AccessToken);
        ::CoTaskMemFree(Value->RefreshToken);
        std::memset(Value, 0, sizeof(HCC_TOKEN));
    }
}

EXTERN_C VOID WINAPI HccFreeUser(
    PHCC_USER Value)
{
    if (Value)
    {
        ::CoTaskMemFree(Value->Identity);
        ::CoTaskMemFree(Value->Password);
        ::CoTaskMemFree(Value->Name);
        ::CoTaskMemFree(Value->Addon);
        ::CoTaskMemFree(Value->Hash);
        ::CoTaskMemFree(Value->Icon);
        std::memset(Value, 0, sizeof(HCC_USER));
    }
}
