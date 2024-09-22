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

#include <Mile.Json.h>

#include "HalalCloud.Client.Core.Native.h"

#include "HalalCloud.Client.Session.h"

int main()
{
    HalalCloud::Session Session;

    std::string Token = Session.Authenticate([](
        std::string_view AuthenticationUri)
    {
        std::printf(
            "AuthenticationUri = \"%s\"\n",
            AuthenticationUri.data());

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

    std::printf("Login Success!\n");

    std::printf(
        "Token = \"%s\"\n",
        Token.c_str());

    ///*hr = ::HccUploadFile(
    //            Session,
    //            "D:\\Updates\\9p.cap",
    //            "/9p.cap");*/

    //{
    //    HCC_TOKEN Information = { 0 };
    //    hr = ::HccGetTokenInformation(Session, &Information);
    //    if (SUCCEEDED(hr))
    //    {
    //        std::printf(
    //            "AccessToken = \"%s\"\n"
    //            "RefreshToken = \"%s\"\n",
    //            Information.AccessToken,
    //            Information.RefreshToken);
    //        hr = ::HccImpersonate(
    //            Session,
    //            Information.RefreshToken);
    //        if (SUCCEEDED(hr))
    //        {
    //            std::printf("Refresh Success!\n");
    //        }

    //        ::HccFreeToken(&Information);
    //    }
    //}

    //{
    //    HCC_USER Information = { 0 };
    //    hr = ::HccGetUserInformation(Session, &Information);
    //    if (SUCCEEDED(hr))
    //    {
    //        std::printf(
    //            "Identity = \"%s\"\n"
    //            "Name = \"%s\"\n",
    //            Information.Identity,
    //            Information.Name);
    //        ::HccFreeUser(&Information);
    //    }
    //}

    //hr = ::HccLogout(Session);
    //if (SUCCEEDED(hr))
    //{
    //    std::printf("Logout Success!\n");
    //}

    std::printf(
        "================================================================\n"
        "HalalCloud.Client.Playground has been completed.\n");
    std::getchar();

    return 0;
}
