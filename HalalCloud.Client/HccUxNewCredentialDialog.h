/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HccUxNewCredentialDialog.h
 * PURPOSE:    Definition for New Credential Dialog
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#pragma once

#include <QtCore/QTimer>
#include <QtWidgets/QDialog>

#include <string>
#include <thread>

#include "ui_HccUxNewCredentialDialog.h"

#include "HccProtocolWrappers.h"

class HccUxNewCredentialDialog :
    public QDialog,
    public Ui::HccUxNewCredentialDialog
{
	Q_OBJECT

private:

    HalalCloud::UserToken m_UserToken;

    std::string m_CodeVerifier;
    std::string m_Code;
    std::string m_RedirectUri;

    std::thread m_AuthenticateThread;
    bool m_AuthenticateThreadStopRequested = false;

private:

    void ClearAuthenticateState();

    void AuthenticateWorker();

signals:

    void UpdateContentSignal();

    void LoginSucceededSignal();

private slots:

    void UpdateContent();

    void LoginSucceeded();

    void WebLinkOpenButtonClick();

public:

	HccUxNewCredentialDialog(
        QDialog* Parent = nullptr);

	~HccUxNewCredentialDialog();

    virtual void closeEvent(
        QCloseEvent* event) override;

    HalalCloud::UserToken GetUserToken() const;
};
