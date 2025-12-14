/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HccUxNewCredentialWidget.h
 * PURPOSE:    Definition for New Credential Widget
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#pragma once

#include <QWidget>
#include "ui_HccUxNewCredentialWidget.h"

class HccUxNewCredentialWidget :
    public QWidget,
    public Ui::HccUxNewCredentialWidget
{
	Q_OBJECT

public:

	HccUxNewCredentialWidget(
        QWidget* Parent = nullptr);

	~HccUxNewCredentialWidget();
};
