/*
 * PROJECT:   HalalCloud.Client.Playground
 * FILE:      HalalCloud.Client.Playground.cpp
 * PURPOSE:   Implementation for Halal Cloud Client Playground
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include <clocale>
#include <cstdio>
#include <cwchar>

#include <Windows.h>

DECLARE_HANDLE(HCC_SESSION);
typedef HCC_SESSION *PHCC_SESSION;

EXTERN_C HRESULT WINAPI HccCreateSessionManager(
    _In_ PHCC_SESSION Session);

EXTERN_C HRESULT WINAPI HccCreateAuthToken(
    _In_ HCC_SESSION Session);

int main()
{
    HCC_SESSION Session = nullptr;

    HRESULT hr = ::HccCreateSessionManager(&Session);
    if (SUCCEEDED(hr))
    {
        hr = ::HccCreateAuthToken(Session);
    }

    std::printf(
        "================================================================\n"
        "HalalCloud.Client.Playground has been completed.\n");
    std::getchar();

    return 0;
}
