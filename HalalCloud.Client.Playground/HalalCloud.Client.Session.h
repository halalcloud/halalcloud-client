/*
 * PROJECT:   Halal Cloud Client
 * FILE:      HalalCloud.Client.Session.h
 * PURPOSE:   Definition for the HalalCloud.Client.Session
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef HALALCLOUD_CLIENT_SESSION
#define HALALCLOUD_CLIENT_SESSION

#include <Mile.Helpers.CppBase.h>
#include <HalalCloud.RpcClient.h>

#include <Mile.Json.h>

#include <functional>
#include <string_view>

namespace HalalCloud
{
    [[noreturn]] void ThrowExceptionWithHResult(
        _In_ LPCSTR Checkpoint,
        _In_ HRESULT Value);

    class Session : Mile::DisableCopyConstruction
    {
    private:

        HCC_RPC_SESSION m_Session = nullptr;
        nlohmann::json m_CurrentToken;

        void ApplyAccessToken(
            nlohmann::json const& Token);

    public:

        Session();

        ~Session();

        HCC_RPC_SESSION NativeHandle();

        nlohmann::json CurrentToken();

        nlohmann::json Request(
            std::string_view MethodFullName,
            nlohmann::json const& Request);

        void Authenticate(
            std::function<void(std::string_view)> Callback);

        void Impersonate(
            std::string_view RefreshToken);

        void Logout();
    };
}

#endif // !HALALCLOUD_CLIENT_SESSION
