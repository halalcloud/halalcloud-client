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

    QTimer m_StatusTimer;

private:

    void UpdateContent();

private slots:

    void WebLinkOpenButtonClick();

    void UpdateStatus();

public:

	HccUxNewCredentialDialog(
        QDialog* Parent = nullptr);

	~HccUxNewCredentialDialog();

    HalalCloud::UserToken GetUserToken() const;
};
