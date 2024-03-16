/*
 * PROJECT:   HalalCloud.Client.Playground
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

int main()
{
    // I can't use that because of the limitation in VC-LTL.
    // std::setlocale(LC_ALL, "zh_CN.UTF-8");

    std::setlocale(LC_ALL, "chs");

    std::wprintf(
        L"Halal Cloud Client Playground\n"
        L"================================================================\n"
        L"他喵的 MSVC 2019 工具集坑了我们价值百万美刀的项目！\n"
        L"The F@cking MSVC 2019 toolset ruined our $1M project!\n");

    return 0;
}
