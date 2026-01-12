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
#include "HccUxMainWindow.h"

int main(int argc, char* argv[])
{
    QApplication Application(argc, argv);

    HalalCloud::LoadGlobalConfigurations();

    HalalCloud::GlobalConfigurations& Configurations =
        HalalCloud::GetGlobalConfigurations();

    HalalCloud::UserToken& CurrentToken = Configurations.CurrentToken;

    if (!CurrentToken.RefreshToken.empty())
    {
        // Restore the user token with the refresh token.
        CurrentToken = HalalCloud::RefreshToken(CurrentToken.RefreshToken);
        HalalCloud::SaveGlobalConfigurations();
    }

    if (!CurrentToken.Validate())
    {
        HccUxNewCredentialDialog* Dialog = new HccUxNewCredentialDialog();
        Dialog->show();
        Dialog->exec();
        if (QDialog::DialogCode::Accepted != Dialog->result())
        {
            // Exit if the user cancelled the login.
            return 0;
        }
        // Get the user token if login succeeded.
        {
            CurrentToken = Dialog->GetUserToken();
            HalalCloud::SaveGlobalConfigurations();
        }
    }

    HccUxMainWindow* MainWindow = new HccUxMainWindow();
    MainWindow->show();

    return Application.exec();
}
