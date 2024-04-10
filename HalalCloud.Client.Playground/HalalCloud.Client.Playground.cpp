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
    _Out_ PHCC_SESSION Session);

EXTERN_C HRESULT WINAPI HccCloseSessionManager(
    _In_ HCC_SESSION Session);

typedef struct _HCC_OAUTH_TOKEN_RESPONSE
{
    LPSTR Url; // Free via CoTaskMemFree.
    LPSTR Addon; // Free via CoTaskMemFree.
    LPSTR Input; // Free via CoTaskMemFree.
    LPSTR Type; // Free via CoTaskMemFree.
    LPSTR Callback; // Free via CoTaskMemFree.
    LPSTR ReturnUrl; // Free via CoTaskMemFree.
    INT32 ReturnType;
    LPSTR Captcha; // Free via CoTaskMemFree.
    LPSTR State; // Free via CoTaskMemFree.
} HCC_OAUTH_TOKEN_RESPONSE, *PHCC_OAUTH_TOKEN_RESPONSE;

EXTERN_C HRESULT WINAPI HccCreateAuthToken(
    _In_ HCC_SESSION Session,
    _Out_ PHCC_OAUTH_TOKEN_RESPONSE Response);

int main()
{
    HCC_OAUTH_TOKEN_RESPONSE Response = { 0 };

    HCC_SESSION Session = nullptr;
    HRESULT hr = ::HccCreateSessionManager(&Session);
    if (SUCCEEDED(hr))
    {
        hr = ::HccCreateAuthToken(Session, &Response);

        std::printf(
            "ReturnUrl = %s\n"
            "Callback = %s\n",
            Response.ReturnUrl,
            Response.Callback);

        ::HccCloseSessionManager(Session);
    }

    std::printf(
        "================================================================\n"
        "HalalCloud.Client.Playground has been completed.\n");
    std::getchar();

    return 0;
}
