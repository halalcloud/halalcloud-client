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

typedef struct _HCC_TOKEN
{
    LPSTR Identity;
    LPSTR UserIdentity;
    LPSTR Device;
    INT32 Version;
    LPSTR AccessToken;
    LPSTR RefreshToken;
    INT64 UpdateTs;
    INT64 AccessTokenExpireTs;
    INT64 RefreshTokenExpireTs;
} HCC_TOKEN, *PHCC_TOKEN;

EXTERN_C VOID WINAPI HccFreeToken(
    PHCC_TOKEN Value);

typedef struct _HCC_USER
{
    // identity for user, unique in system
    LPSTR Identity;
    // 1: user, 2: admin, 3: super admin
    INT32 Type;
    // 1: normal, 2: disabled, 3: deleted
    INT32 Status;
    INT64 UpdateTs;
    LPSTR Password;
    LPSTR Name;
    LPSTR Addon;
    INT64 CreateTs;
    LPSTR Hash;
    LPSTR Icon;
} HCC_USER, *PHCC_USER;

EXTERN_C VOID WINAPI HccFreeUser(
    PHCC_USER Value);

typedef struct _HCC_LAST_LOGIN_RESPONSE
{
    LPSTR Identity;
    INT64 LastLoginTs;
    LPSTR LastLoginIp;
    LPSTR LastLoginDevice;
} HCC_LAST_LOGIN_RESPONSE, *PHCC_LAST_LOGIN_RESPONSE;

EXTERN_C VOID WINAPI HccFreeLastLoginResponse(
    PHCC_LAST_LOGIN_RESPONSE Value);

typedef struct _HCC_LOGIN_RESPONSE
{
    HCC_TOKEN Token;
    HCC_USER User;
    HCC_LAST_LOGIN_RESPONSE LastLogin;
    LPSTR State;
} HCC_LOGIN_RESPONSE, *PHCC_LOGIN_RESPONSE;

EXTERN_C VOID WINAPI HccFreeLoginResponse(
    PHCC_LOGIN_RESPONSE Value);

typedef void(*PHCC_LOGIN_NOTIFY_AUTHENTICATION_URI_CALLBACK)(
    LPSTR AuthenticationUri);

EXTERN_C HRESULT WINAPI HccLoginWithAuthenticationUri(
    _In_ HCC_SESSION Session,
    _In_ PHCC_LOGIN_NOTIFY_AUTHENTICATION_URI_CALLBACK Callback);

EXTERN_C HRESULT WINAPI HccLoginWithRefreshToken(
    _In_ HCC_SESSION Session,
    _In_ LPSTR RefreshToken);

#endif // !HALALCLOUD_CLIENT_CORE_NATIVE
