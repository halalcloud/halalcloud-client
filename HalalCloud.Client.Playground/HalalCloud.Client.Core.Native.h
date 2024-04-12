/*
 * PROJECT:   Halal Cloud Client
 * FILE:      HalalCloud.Client.Core.Native.h
 * PURPOSE:   Definition for the HalalCloud.Client.Core.Native
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef HALALCLOUD_CLIENT_CORE_NATIVE
#define HALALCLOUD_CLIENT_CORE_NATIVE

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

#endif // !HALALCLOUD_CLIENT_CORE_NATIVE
