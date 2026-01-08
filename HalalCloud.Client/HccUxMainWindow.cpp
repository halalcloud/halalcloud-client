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

HccUxMainWindow::HccUxMainWindow(
    QWidget * Parent)
	: QMainWindow(Parent)
{
    this->setupUi(this);
}

HccUxMainWindow::~HccUxMainWindow()
{

}
