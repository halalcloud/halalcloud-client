/*
 * PROJECT:   Halal Cloud Client
 * FILE:      HalalCloud.Client.Session.cpp
 * PURPOSE:   Implementation for the HalalCloud.Client.Session
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "HalalCloud.Client.Session.h"

#include <stdexcept>
#include <thread>

[[noreturn]] void HalalCloud::ThrowExceptionWithHResult(
    _In_ LPCSTR Checkpoint,
    _In_ HRESULT Value)
{
    throw std::runtime_error(Mile::FormatString(
        "[HalalCloud.Client] %s Failed. (0x%08X)",
        Checkpoint,
        Value));
}

void HalalCloud::Session::ApplyAccessToken(
    nlohmann::json const& Token)
{
    std::string AccessToken = Mile::Json::ToString(
        Mile::Json::GetSubKey(Token, "access_token"));
    HRESULT hr = ::HccRpcSetAccessToken(
        this->m_Session,
        AccessToken.c_str());
    if (FAILED(hr))
    {
        HalalCloud::ThrowExceptionWithHResult(
            "HccRpcSetAccessToken",
            hr);
    }
}

HalalCloud::Session::Session()
{
    HRESULT hr = ::HccRpcCreateSession(&this->m_Session);
    if (FAILED(hr))
    {
        HalalCloud::ThrowExceptionWithHResult(
            "HccRpcCreateSession",
            hr);
    }
}

HalalCloud::Session::~Session()
{
    ::HccRpcCloseSession(this->m_Session);
}

HCC_RPC_SESSION HalalCloud::Session::NativeHandle()
{
    return this->m_Session;
}

nlohmann::json HalalCloud::Session::CurrentToken()
{
    return this->m_CurrentToken;
}

nlohmann::json HalalCloud::Session::Request(
    std::string_view MethodFullName,
    nlohmann::json const& Request)
{
    nlohmann::json Response;

    LPSTR ResponseJson = nullptr;

    HRESULT hr = ::HccRpcRequest(
        this->m_Session,
        MethodFullName.data(),
        Request.dump().c_str(),
        &ResponseJson);
    if (FAILED(hr))
    {
        HalalCloud::ThrowExceptionWithHResult(
            "HccRpcRequest",
            hr);
    }

    auto ResponseJsonCleanupHandler = Mile::ScopeExitTaskHandler([&]()
    {
        ::HccRpcFreeMemory(ResponseJson);
    });

    return nlohmann::json::parse(ResponseJson);
}

void HalalCloud::Session::Authenticate(
    std::function<void(std::string_view)> Callback)
{
    nlohmann::json Request;
    {
        Request = nlohmann::json();
        Request["return_type"] = 2;

        nlohmann::json Response = this->Request(
            "/v6.services.pub.PubUser/CreateAuthToken",
            Request);

        Callback(Mile::Json::ToString(
            Mile::Json::GetSubKey(Response, "return_url")));

        Request = nlohmann::json();
        Request["return_type"] = 2;
        Request["callback"] = Mile::Json::ToString(
            Mile::Json::GetSubKey(Response, "callback"));
    }

    while (true)
    {
        nlohmann::json Response = this->Request(
            "/v6.services.pub.PubUser/VerifyAuthToken",
            Request);
        if (6 == Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Response, "status")))
        {
            nlohmann::json Token = Mile::Json::GetSubKey(
                Mile::Json::GetSubKey(Response, "login"),
                "token");
            this->ApplyAccessToken(Token);
            this->m_CurrentToken = Token;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void HalalCloud::Session::Impersonate(
    std::string_view RefreshToken)
{
    nlohmann::json Request;
    Request["refresh_token"] = RefreshToken;
    nlohmann::json Response = this->Request(
        "/v6.services.pub.PubUser/Refresh",
        Request);
    this->ApplyAccessToken(Response);
    this->m_CurrentToken = Response;
}

void HalalCloud::Session::Logout()
{
    if (!this->m_CurrentToken.empty())
    {
        this->Request(
            "/v6.services.pub.PubUser/Logoff",
            this->m_CurrentToken);
        this->m_CurrentToken = std::string();
    }
}
