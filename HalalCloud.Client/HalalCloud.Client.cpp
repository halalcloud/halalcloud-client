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

    HalalCloud::LoadGlobalConfigurations();

    HalalCloud::GlobalConfigurations& Configurations =
        HalalCloud::GetGlobalConfigurations();

    HalalCloud::UserToken& CurrentToken = Configurations.CurrentToken;

    if (!CurrentToken.RefreshToken.empty())
    {
        // Restore the user token with the refresh token.
        CurrentToken = HalalCloud::RefreshToken(CurrentToken.RefreshToken);
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

    if (CurrentToken.Validate())
    {
        HalalCloud::UserInformation Information =
            HalalCloud::GetUserInformation(CurrentToken);

        if (!Information.Icon.empty())
        {
            std::string IconFileExtension = std::string(std::strrchr(
                Information.Icon.c_str(),
                '.'));
            std::string IconFileName = "Profile";
            IconFileName.append(
                IconFileExtension.empty() ? "Icon" : IconFileExtension);

            std::filesystem::path CurrentProfilePath =
                HalalCloud::GetProfilePath(Configurations.CurrentProfile);
            std::filesystem::path IconFilePath =
                CurrentProfilePath / IconFileName;
            ::HccDownloadFile(
                Information.Icon.c_str(),
                HalalCloud::PathToUtf8String(IconFilePath).c_str());
        }

        QMessageBox::information(
            nullptr,
            Information.Name.c_str(),
            Information.Identity.c_str());
    }

    return 0;
}
