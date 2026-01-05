/*
 * PROJECT:   Halal Cloud Client
 * FILE:      HalalCloud.Client.cpp
 * PURPOSE:   Implementation for Halal Cloud Client
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include <Windows.h>

#include <thread>
#include <QtWidgets/QApplication>

#include "HccProtocolWrappers.h"

#include "HccUxNewCredentialWidget.h"

int main(int argc, char* argv[])
{
    QApplication Application(argc, argv);

    if (!HalalCloud::GetGlobalConfigurations().CurrentToken.Validate())
    {
        std::string CodeVerifier = HalalCloud::GenerateCodeVerifier();

        std::string Code;
        std::string RedirectUri;
        HalalCloud::Authorize(Code, RedirectUri, CodeVerifier);

        HccUxNewCredentialWidget* Widget = new HccUxNewCredentialWidget();
        Widget->setWindowFlags(Qt::Dialog);
        Widget->UpdateWebLink(RedirectUri.c_str());
        Widget->show();
    }

    return Application.exec();
}
