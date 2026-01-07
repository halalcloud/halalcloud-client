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
#include <QtWidgets/QMessageBox>

#include "HccProtocolWrappers.h"

#include "HccUxNewCredentialDialog.h"

int main(int argc, char* argv[])
{
    QApplication Application(argc, argv);

    if (!HalalCloud::GetGlobalConfigurations().CurrentToken.Validate())
    {
        HccUxNewCredentialDialog* Dialog = new HccUxNewCredentialDialog();
        Dialog->show();
        Dialog->exec();
        if (QDialog::DialogCode::Accepted == Dialog->result())
        {
            HalalCloud::GetGlobalConfigurations().CurrentToken =
                Dialog->GetUserToken();
        }
    }

    HalalCloud::UserToken& Token =
        HalalCloud::GetGlobalConfigurations().CurrentToken;

    if (Token.Validate())
    {
        HalalCloud::UserInformation Information =
            HalalCloud::GetUserInformation(Token);

        QMessageBox::information(
            nullptr,
            Information.Name.c_str(),
            Information.Identity.c_str());
    }

    return Application.exec();
}
