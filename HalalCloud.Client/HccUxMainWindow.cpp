/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HccUxMainWindow.cpp
 * PURPOSE:    Implementation for Main Window
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "HccUxMainWindow.h"

#include "HccProtocolWrappers.h"

#include <QtGui/QDesktopServices>

#include <Mile.Project.Version.h>

#define MOBILITY_MINUAP_VERSION_UTF8_STRING \
    MILE_PROJECT_VERSION_UTF8_STRING " (Build " \
    MILE_PROJECT_MACRO_TO_UTF8_STRING(MILE_PROJECT_VERSION_BUILD) ")"

namespace
{
    const char HalalCloudLink[] = "https://2dland.cn/";
    const char GitHubLink[] = "https://github.com/halalcloud/halalcloud-client";
}

void HccUxMainWindow::LogoutButtonClick()
{

}

void HccUxMainWindow::OpenDirectoryButtonClick()
{

}

void HccUxMainWindow::ViewLogButtonClick()
{

}

void HccUxMainWindow::UploadFileButtonClick()
{

}

void HccUxMainWindow::HalalCloudButtonClick()
{
    QDesktopServices::openUrl(QUrl(HalalCloudLink));
}

void HccUxMainWindow::GitHubButtonClick()
{
    QDesktopServices::openUrl(QUrl(GitHubLink));
}

HccUxMainWindow::HccUxMainWindow(
    QWidget * Parent)
	: QMainWindow(Parent)
{
    this->setupUi(this);

    std::string VersionInformation;
    VersionInformation.append(
        reinterpret_cast<const char*>(u8"清真云客户端 "));
    VersionInformation.append(
        MILE_PROJECT_VERSION_UTF8_STRING " (Build "
        MILE_PROJECT_MACRO_TO_UTF8_STRING(MILE_PROJECT_VERSION_BUILD) ")");
    this->VersionInformationLabel->setText(QString::fromUtf8(
        VersionInformation.c_str(),
        VersionInformation.size()));

    this->connect(
        this->LogoutButton,
        SIGNAL(clicked()),
        this,
        SLOT(LogoutButtonClick()));

    this->connect(
        this->OpenDirectoryButton,
        SIGNAL(clicked()),
        this,
        SLOT(OpenDirectoryButtonClick()));

    this->connect(
        this->ViewLogButton,
        SIGNAL(clicked()),
        this,
        SLOT(ViewLogButtonClick()));

    this->connect(
        this->UploadFileButton,
        SIGNAL(clicked()),
        this,
        SLOT(UploadFileButtonClick()));

    this->connect(
        this->HalalCloudButton,
        SIGNAL(clicked()),
        this,
        SLOT(HalalCloudButtonClick()));

    this->connect(
        this->GitHubButton,
        SIGNAL(clicked()),
        this,
        SLOT(GitHubButtonClick()));

    HalalCloud::GlobalConfigurations& Configurations =
        HalalCloud::GetGlobalConfigurations();

    HalalCloud::UserToken& CurrentToken = Configurations.CurrentToken;
    if (CurrentToken.Validate())
    {
        HalalCloud::UserInformation Information =
            HalalCloud::GetUserInformation(CurrentToken);

        this->ProfileNameLabel->setText(QString::fromUtf8(
            Information.Name.c_str(),
            Information.Name.size()));
        this->ProfileIdentityLabel->setText(QString::fromUtf8(
            Information.Identity.c_str(),
            Information.Identity.size()));

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

            QPixmap ProfileIconPixmap;
            ProfileIconPixmap.load(QString::fromUtf8(
                HalalCloud::PathToUtf8String(IconFilePath).c_str()));
            this->ProfileIconPresenterLabel->setPixmap(
                ProfileIconPixmap.scaled(
                    this->ProfileIconPresenterLabel->size(),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation));
        }
    }
}

HccUxMainWindow::~HccUxMainWindow()
{

}
