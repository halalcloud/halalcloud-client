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

#include "HalalCloud.Client.Session.h"

#include <time.h>

int main()
{
    HalalCloud::Session Session;

    Session.Authenticate([](
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
        Session.CurrentToken().dump(2).c_str());

    Session.Impersonate(Mile::Json::ToString(
        Mile::Json::GetSubKey(Session.CurrentToken(), "refresh_token")));

    std::printf("Refresh Success!\n");

    std::printf(
        "Token = \"%s\"\n",
        Session.CurrentToken().dump(2).c_str());

    std::printf(
        "UserInformation = %s\n",
        Session.GetUserInformation().dump(2).c_str());

    Session.UploadFile(
        "D:\\Updates\\9p.cap",
        "/9p20240925.cap");

    Session.CreateFolder("/Test20240923");

    std::vector<HalalCloud::FileInformation> Files =
        Session.EnumerateFiles("/");
    for(HalalCloud::FileInformation const& File : Files)
    {
        std::time_t Time = File.CreationTime / 1000;
        std::tm TimeInfo = { 0 };
        ::localtime_s(&TimeInfo, &Time);
        std::printf(
            "%04d/%02d/%02d %02d:%02d\t%s\t%lld\t%s\n",
            TimeInfo.tm_year + 1900,
            TimeInfo.tm_mon + 1,
            TimeInfo.tm_mday,
            TimeInfo.tm_hour,
            TimeInfo.tm_min,
            File.FileAttributes.Fields.IsDirectory ? "<DIR>" : "",
            File.FileSize,
            File.FileName.c_str());
    }

    Session.Logout();

    std::printf("Logout Success!\n");

    std::printf(
        "================================================================\n"
        "HalalCloud.Client.Playground has been completed.\n");
    std::getchar();

    return 0;
}
