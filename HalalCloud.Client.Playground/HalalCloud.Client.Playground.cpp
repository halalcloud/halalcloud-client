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

#include "HalalCloud.Client.Core.Native.h"

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
