/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtWidgets module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef BCLIPBOARDNOTIFIER_H
#define BCLIPBOARDNOTIFIER_H

class BClipboardNotifierPrivate;

class QString;

#include <BeQtCore/BBaseObject>

#include <QObject>

/*============================================================================
================================ BClipboardNotifier ==========================
============================================================================*/

class B_WIDGETS_EXPORT BClipboardNotifier : public QObject, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BClipboardNotifier)
    B_DECLARE_PRIVATE_S(BClipboardNotifier)
protected:
    static BClipboardNotifier *_m_self;
protected:
    explicit BClipboardNotifier();
    ~BClipboardNotifier();
public:
    static void destroy();
    static BClipboardNotifier *instance();
    static bool hasColor();
    static bool hasFormat(const QString &mimeType);
    static bool hasHtml();
    static bool hasImage();
    static bool hasText();
    static bool hasUrls();
Q_SIGNALS:
    void hasColorChanged(bool available);
    void hasHtmlChanged(bool available);
    void hasImageChanged(bool available);
    void hasTextChanged(bool available);
    void hasUrlsChanged(bool available);
private:
    Q_DISABLE_COPY(BClipboardNotifier)
};

#endif // BCLIPBOARDNOTIFIER_H
