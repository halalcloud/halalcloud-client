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

#include <QtGui/QCloseEvent>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>

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
    QMessageBox::StandardButton UserResponse = QMessageBox::question(
        this,
        qApp->applicationDisplayName(),
        QString::fromUtf8(u8"您确定要退出账号登录并退出吗？"),
        QMessageBox::StandardButton::Yes |
        QMessageBox::StandardButton::No,
        QMessageBox::StandardButton::No);
    if (QMessageBox::StandardButton::Yes != UserResponse)
    {
        return;
    }

    HalalCloud::GlobalConfigurations& Configurations =
        HalalCloud::GetGlobalConfigurations();

    HalalCloud::UserToken& CurrentToken = Configurations.CurrentToken;
    if (CurrentToken.Validate())
    {
        HalalCloud::Logoff(CurrentToken);
        qApp->quit();
    }
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
    QWidget* Parent)
	: QMainWindow(Parent)
{
    this->setupUi(this);

    {
        std::string VersionInformation;
        VersionInformation.append(
            qApp->applicationDisplayName().toUtf8().constData());
        VersionInformation.append(
            MILE_PROJECT_VERSION_UTF8_STRING " (Build "
            MILE_PROJECT_MACRO_TO_UTF8_STRING(MILE_PROJECT_VERSION_BUILD) ")");
        this->VersionInformationLabel->setText(QString::fromUtf8(
            VersionInformation.c_str(),
            VersionInformation.size()));
    }

    {
        this->m_SystemTray = new QSystemTrayIcon();

        this->m_SystemTrayContextMenu = new QMenu(this);

        this->m_SystemTrayContextMenu->addAction(
            qApp->applicationDisplayName(),
            this,
            [&]()
        {
            if (this->isHidden())
            {
                this->show();
                this->activateWindow();
            }
            else
            {
                this->hide();
            }
        });

        this->m_SystemTrayContextMenu->addAction(
            u8"退出",
            this,
            [&]()
        {
            qApp->quit();
        });

        this->m_SystemTray->setContextMenu(
            this->m_SystemTrayContextMenu);
        this->m_SystemTray->setToolTip(
            qApp->applicationDisplayName());
        this->m_SystemTray->setIcon(
            QIcon(":/HalalCloud/SvgIcon"));
        this->connect(
            this->m_SystemTray,
            &QSystemTrayIcon::activated,
            this,
            [&](QSystemTrayIcon::ActivationReason reason)
        {
            if (reason == QSystemTrayIcon::Trigger ||
                reason == QSystemTrayIcon::DoubleClick ||
                reason == QSystemTrayIcon::MiddleClick)
            {
                if (this->isHidden())
                {
                    this->show();
                    this->activateWindow();
                }
                else
                {
                    this->hide();
                }
            }
        });
        this->m_SystemTray->show();
    }

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

void HccUxMainWindow::closeEvent(
    QCloseEvent* event)
{
    this->hide();
    event->ignore();
}
