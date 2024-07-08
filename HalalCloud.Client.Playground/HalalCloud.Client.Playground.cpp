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
        hr = ::HccLoginWithAuthenticationUri(
            Session,
            [](LPSTR AuthenticationUri)
        {
            std::printf("AuthenticationUri = \"%s\"\n", AuthenticationUri);

            std::wstring ConvertedReturnUrl =
                Mile::ToWideString(CP_UTF8, AuthenticationUri);

            SHELLEXECUTEINFOW ExecInfo = { 0 };
            ExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
            ExecInfo.lpVerb = L"open";
            ExecInfo.lpFile = ConvertedReturnUrl.c_str();
            ExecInfo.nShow = SW_SHOWNORMAL;
            ::ShellExecuteExW(&ExecInfo);

            std::printf("Waiting...\n");
        });
        if (SUCCEEDED(hr))
        {
            std::printf("Login Success!\n");
            HCC_TOKEN Information = { 0 };
            hr = ::HccGetTokenInformation(Session, &Information);
            if (SUCCEEDED(hr))
            {
                std::printf(
                    "AccessToken = \"%s\"\n"
                    "RefreshToken = \"%s\"\n",
                    Information.AccessToken,
                    Information.RefreshToken);
                ::HccFreeToken(&Information);
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
