/*
 * PROJECT:   Halal Cloud Client
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

#include <Mile.Helpers.CppBase.h>

#include "HalalCloud.Client.Core.Native.h"

int main()
{
    HCC_SESSION Session = nullptr;
    HRESULT hr = ::HccCreateSessionManager(&Session);
    if (SUCCEEDED(hr))
    {
        HCC_OAUTH_TOKEN_RESPONSE Response = { 0 };
        hr = ::HccCreateAuthToken(Session, &Response);
        if (SUCCEEDED(hr))
        {
            std::printf(
                "ReturnUrl = %s\n"
                "Callback = %s\n"
                "================================================================\n",
                Response.ReturnUrl,
                Response.Callback);

            {
                std::wstring ConvertedReturnUrl =
                    Mile::ToWideString(CP_UTF8, Response.ReturnUrl);

                SHELLEXECUTEINFOW ExecInfo = { 0 };
                ExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
                ExecInfo.lpVerb = L"open";
                ExecInfo.lpFile = ConvertedReturnUrl.c_str();
                ExecInfo.nShow = SW_SHOWNORMAL;
                ::ShellExecuteExW(&ExecInfo);
            }

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
                    std::printf(
                        "AccessToken = %s\n"
                        "AccessTokenExpireTs = %lld\n"
                        "RefreshToken = %s\n"
                        "RefreshTokenExpireTs = %lld\n"
                        "================================================================\n",
                        CheckResponse.Login.Token.AccessToken,
                        CheckResponse.Login.Token.AccessTokenExpireTs,
                        CheckResponse.Login.Token.RefreshToken,
                        CheckResponse.Login.Token.RefreshTokenExpireTs);
                    break;
                }

                ::HccFreeOauthTokenCheckResponse(&CheckResponse);

                ::Sleep(200);
            }

            ::HccFreeOauthTokenResponse(&Response);
        }
        

        ::HccCloseSessionManager(Session);
    }

    std::printf(
        "================================================================\n"
        "HalalCloud.Client.Playground has been completed.\n");
    std::getchar();

    return 0;
}
