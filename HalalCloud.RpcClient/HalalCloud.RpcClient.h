/*
 * PROJECT:   Halal Cloud Client
 * FILE:      HalalCloud.RpcClient.h
 * PURPOSE:   Definition for the Halal Cloud RPC Client
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef HALALCLOUD_RPCCLIENT
#define HALALCLOUD_RPCCLIENT

#include <Windows.h>

DECLARE_HANDLE(HCC_RPC_SESSION);
typedef HCC_RPC_SESSION *PHCC_RPC_SESSION;

EXTERN_C HRESULT WINAPI HccRpcCreateSession(
    _Out_ PHCC_RPC_SESSION Instance);

EXTERN_C HRESULT WINAPI HccRpcCloseSession(
    _In_ HCC_RPC_SESSION Instance);

EXTERN_C HRESULT WINAPI HccRpcSetAccessToken(
    _In_ HCC_RPC_SESSION Instance,
    _In_ LPCSTR AccessToken);

EXTERN_C HRESULT WINAPI HccRpcRequest(
    _In_ HCC_RPC_SESSION Instance,
    _In_ LPCSTR MethodFullName,
    _In_ LPCSTR RequestJson,
    _Out_opt_ LPCSTR* ResponseJson);

#endif // !HALALCLOUD_RPCCLIENT
