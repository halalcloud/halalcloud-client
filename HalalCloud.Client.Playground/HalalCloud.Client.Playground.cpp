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

typedef struct _HCC_TOKEN
{
    LPSTR Identity; // Free via CoTaskMemFree.
    LPSTR UserIdentity; // Free via CoTaskMemFree.
    LPSTR Device; // Free via CoTaskMemFree.
    INT32 Version;
    LPSTR AccessToken; // Free via CoTaskMemFree.
    LPSTR RefreshToken; // Free via CoTaskMemFree.
    INT64 UpdateTs;
    INT64 AccessTokenExpireTs;
    INT64 RefreshTokenExpireTs;
} HCC_TOKEN, *PHCC_TOKEN;

typedef struct _HCC_USER
{
    // identity for user, unique in system
    // Free via CoTaskMemFree.
    LPSTR Identity;
    // 1: user, 2: admin, 3: super admin
    INT32 Type;
    // 1: normal, 2: disabled, 3: deleted
    INT32 Status;
    INT64 UpdateTs;
    LPSTR Password; // Free via CoTaskMemFree.
    LPSTR Name; // Free via CoTaskMemFree.
    LPSTR Addon; // Free via CoTaskMemFree.
    INT64 CreateTs;
    LPSTR Hash; // Free via CoTaskMemFree.
    LPSTR Icon; // Free via CoTaskMemFree.
} HCC_USER, *PHCC_USER;

typedef struct _HCC_LAST_LOGIN_RESPONSE
{
    LPSTR Identity; // Free via CoTaskMemFree.
    INT64 LastLoginTs;
    LPSTR LastLoginIp; // Free via CoTaskMemFree.
    LPSTR LastLoginDevice; // Free via CoTaskMemFree.
} HCC_LAST_LOGIN_RESPONSE, *PHCC_LAST_LOGIN_RESPONSE;

typedef struct _HCC_LOGIN_RESPONSE
{
    HCC_TOKEN Token;
    HCC_USER User;
    HCC_LAST_LOGIN_RESPONSE LastLogin;
    LPSTR State; // Free via CoTaskMemFree.
} HCC_LOGIN_RESPONSE, *PHCC_LOGIN_RESPONSE;

typedef struct _HCC_OAUTH_TOKEN_CHECK_RESPONSE
{
    HCC_LOGIN_RESPONSE Login;
    HCC_OAUTH_TOKEN_RESPONSE Oauth;
    INT32 Status;
    LPSTR Message; // Free via CoTaskMemFree.
} HCC_OAUTH_TOKEN_CHECK_RESPONSE, *PHCC_OAUTH_TOKEN_CHECK_RESPONSE;

EXTERN_C HRESULT WINAPI HccVerifyAuthToken(
    _In_ HCC_SESSION Session,
    _In_ LPSTR Callback,
    _Out_ PHCC_OAUTH_TOKEN_CHECK_RESPONSE Response);

int main()
{
    HCC_OAUTH_TOKEN_RESPONSE Response = { 0 };

    HCC_SESSION Session = nullptr;
    HRESULT hr = ::HccCreateSessionManager(&Session);
    if (SUCCEEDED(hr))
    {
        hr = ::HccCreateAuthToken(Session, &Response);
        if (SUCCEEDED(hr))
        {
            std::printf(
                "ReturnUrl = %s\n"
                "Callback = %s\n",
                Response.ReturnUrl,
                Response.Callback);

            std::printf("Waiting");
            for (;;)
            {
                std::printf(".");

                HCC_OAUTH_TOKEN_CHECK_RESPONSE CheckResponse = { 0 };
                hr = ::HccVerifyAuthToken(
                    Session,
                    Response.Callback,
                    &CheckResponse);
                if (FAILED(hr))
                {
                    std::printf("Failed!\n");
                    break;
                }

                if (CheckResponse.Status == 6)
                {
                    std::printf("\n");
                    std::printf("%s\n", CheckResponse.Login.Token.AccessToken);
                    break;
                }

                ::Sleep(200);
            }

        }
        

        ::HccCloseSessionManager(Session);
    }

    std::printf(
        "================================================================\n"
        "HalalCloud.Client.Playground has been completed.\n");
    std::getchar();

    return 0;
}
