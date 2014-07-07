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

#include "bclipboardnotifier.h"
#include "bclipboardnotifier_p.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QMetaObject>
#include <QMimeData>
#include <QObject>
#include <QString>

/*============================================================================
================================ BClipboardNotifierPrivate ===================
============================================================================*/

/*============================== Public constructors =======================*/

BClipboardNotifierPrivate::BClipboardNotifierPrivate(BClipboardNotifier *q) :
    BBaseObjectPrivate(q)
{
    //
}

BClipboardNotifierPrivate::~BClipboardNotifierPrivate()
{
    disconnect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(dataChanged()));
}

/*============================== Public methods ============================*/

void BClipboardNotifierPrivate::init()
{
    qAddPostRoutine(&BClipboardNotifier::destroy);
    const QMimeData *md = QApplication::clipboard()->mimeData();
    colorAvailable = md && md->hasText();
    htmlAvailable = md && md->hasHtml();
    imageAvailable = md && md->hasImage();
    textAvailable = md && md->hasText();
    urlsAvailable = md && md->hasUrls();
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(dataChanged()));
}

/*============================== Public slots ==============================*/

void BClipboardNotifierPrivate::dataChanged()
{
    const QMimeData *md = QApplication::clipboard()->mimeData();
    bool nc = md && md->hasColor();
    bool nh = md && md->hasHtml();
    bool ni = md && md->hasImage();
    bool nt = md && md->hasText();
    bool nu = md && md->hasUrls();
    if (nc != colorAvailable) {
        colorAvailable = !colorAvailable;
        QMetaObject::invokeMethod(q_func(), "hasColorChanged", Q_ARG(bool, colorAvailable));
    }
    if (nh != htmlAvailable) {
        htmlAvailable = !htmlAvailable;
        QMetaObject::invokeMethod(q_func(), "hasHtmlChanged", Q_ARG(bool, htmlAvailable));
    }
    if (ni != imageAvailable) {
        imageAvailable = !imageAvailable;
        QMetaObject::invokeMethod(q_func(), "hasImageChanged", Q_ARG(bool, imageAvailable));
    }
    if (nt != textAvailable) {
        textAvailable = !textAvailable;
        QMetaObject::invokeMethod(q_func(), "hasTextChanged", Q_ARG(bool, textAvailable));
    }
    if (nu != urlsAvailable) {
        urlsAvailable = !urlsAvailable;
        QMetaObject::invokeMethod(q_func(), "hasUrlsChanged", Q_ARG(bool, urlsAvailable));
    }
}

/*============================================================================
================================ BClipboardNotifier ==========================
============================================================================*/

/*============================== Static protected variables ================*/

BClipboardNotifier *BClipboardNotifier::_m_self = 0;

/*============================== Public constructors =======================*/

BClipboardNotifier::BClipboardNotifier() :
    QObject(0), BBaseObject(*new BClipboardNotifierPrivate(this))
{
    d_func()->init();
    _m_self = this;
}

BClipboardNotifier::~BClipboardNotifier()
{
    _m_self = 0;
}

/*============================== Static public methods =====================*/

void BClipboardNotifier::destroy()
{
    if (!_m_self)
        return;
    delete _m_self;
    _m_self = 0;
}

BClipboardNotifier *BClipboardNotifier::instance()
{
    if (!_m_self)
        _m_self = new BClipboardNotifier;
    return _m_self;
}

bool BClipboardNotifier::hasColor()
{
    instance();
    return ds_func()->colorAvailable;
}

bool BClipboardNotifier::hasFormat(const QString &mimeType)
{
    const QMimeData *md = QApplication::clipboard()->mimeData();
    return md && md->hasFormat(mimeType);
}

bool BClipboardNotifier::hasHtml()
{
    instance();
    return ds_func()->htmlAvailable;
}

bool BClipboardNotifier::hasImage()
{
    instance();
    return ds_func()->imageAvailable;
}

bool BClipboardNotifier::hasText()
{
    instance();
    return ds_func()->textAvailable;
}

bool BClipboardNotifier::hasUrls()
{
    instance();
    return ds_func()->urlsAvailable;
}
