/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HccUxMainWindow.h
 * PURPOSE:    Definition for Main Window
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_HccUxMainWindow.h"

class HccUxMainWindow :
    public QMainWindow,
    public Ui::HccUxMainWindowClass
{
	Q_OBJECT

private:

public:

	HccUxMainWindow(
        QWidget* Parent = nullptr);

	~HccUxMainWindow();	 
};

