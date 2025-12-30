/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HccUxNewCredentialWidget.cpp
 * PURPOSE:    Implementation for New Credential Widget
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "HccUxNewCredentialWidget.h"

#include <Mile.Helpers.CppBase.h>

#include <QtCore/QByteArray>
#include <QtGui/QDesktopServices>
#include <QtGui/QPainter>
#include <QtSvg/QSvgRenderer>

#include "qrcodegen.hpp"

namespace
{
    /**
     * @brief Returns a string of SVG code for an image depicting the given QR
     *        Code, with the given number of border modules. The string always
     *        uses Unix newlines (\n), regardless of the platform.
     * @param qr The QR code object to be converted to SVG code.
     * @param border The number of border modules to add around the QR code.
     *               Must be non-negative.
     * @return A string of SVG code for an image depicting the given QR Code.
     */
    static std::string toSvgString(
        const qrcodegen::QrCode& qr,
        int border)
    {
        if (border < 0)
            throw std::domain_error("Border must be non-negative");
        if (border > INT_MAX / 2 || border * 2 > INT_MAX - qr.getSize())
            throw std::overflow_error("Border too large");

        std::string Result;
        Result.append(
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\""
            " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");
        Result.append(Mile::FormatString(
            "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\""
            " viewBox=\"0 0 %d %d\" stroke=\"none\">\n",
            qr.getSize() + border * 2,
            qr.getSize() + border * 2));
        Result.append(
            "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n");
        Result.append("\t<path d=\"");
        for (int y = 0; y < qr.getSize(); ++y)
        {
            for (int x = 0; x < qr.getSize(); ++x)
            {
                if (qr.getModule(x, y))
                {
                    if (!Result.empty() && Result.back() != ' ')
                        Result.append(" ");
                    Result.append(Mile::FormatString(
                        "M%d,%dh1v1h-1z",
                        x + border,
                        y + border));
                }
            }
        }
        Result.append("\" fill=\"#000000\"/>\n");
        Result.append("</svg>\n");
        return Result;
    }

    /**
     * @brief Returns a UTF-8 string of SVG code for an image depicting the QR
     *        code for the given content.
     * @param Content The content you want to generate.
     * @return The UTF-8 string of SVG code for an image depicting the QR code
     *         for the given content.
     */
    std::string CreateQrCodeSvg(
        std::string const& Content)
    {
        return ::toSvgString(
            qrcodegen::QrCode::encodeText(
                Content.c_str(),
                qrcodegen::QrCode::Ecc::HIGH),
            0);
    }
}

void HccUxNewCredentialWidget::WebLinkOpenButtonClick()
{
    QDesktopServices::openUrl(QUrl(this->WebLinkPlainTextEdit->toPlainText()));  
}

HccUxNewCredentialWidget::HccUxNewCredentialWidget(
    QWidget* Parent)
	: QWidget(Parent)
{
    this->setupUi(this);

    this->connect(
        this->WebLinkOpenButton,
        SIGNAL(clicked()),
        this,
        SLOT(WebLinkOpenButtonClick()));
}

HccUxNewCredentialWidget::~HccUxNewCredentialWidget()
{

}

void HccUxNewCredentialWidget::UpdateWebLink(
    std::string const& WebLink)
{
    this->WebLinkPlainTextEdit->setPlainText(
        QString::fromUtf8(WebLink.c_str(), WebLink.size()));
    std::string SvgContent = ::CreateQrCodeSvg(WebLink);
    QSvgRenderer* SvgRender = new QSvgRenderer();
    SvgRender->load(QByteArray(SvgContent.c_str(), SvgContent.size()));
    QPixmap QrCode(this->QrCodePresenterLabel->size());
    QrCode.fill(Qt::transparent);
    QPainter Painter(&QrCode);
    SvgRender->render(&Painter);
    this->QrCodePresenterLabel->setPixmap(QrCode);
}
