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

#include <QtWidgets/QApplication>

#include "HccUxNewCredentialWidget.h"

int main(int argc, char* argv[])
{
    QApplication Application(argc, argv);

    HccUxNewCredentialWidget* Widget = new HccUxNewCredentialWidget();
    Widget->setWindowFlags(Qt::Dialog);
    Widget->UpdateWebLink("https://www.bing.com");
    Widget->show();

    return Application.exec();
}
