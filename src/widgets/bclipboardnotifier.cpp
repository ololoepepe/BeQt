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
#include <BeQtCore/BeQtGlobal>

#include <QApplication>
#include <QObject>
#include <QClipboard>
#include <QMetaObject>

static void clipboardNotifierCleanup()
{
    delete BClipboardNotifier::instance();
}

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
    detach();
}

/*============================== Public methods ============================*/

void BClipboardNotifierPrivate::init()
{
    textDataAvailable = !QApplication::clipboard()->text().isEmpty();
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(dataChanged()));
}

void BClipboardNotifierPrivate::detach()
{
    disconnect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(dataChanged()));
}

/*============================== Public slots ==============================*/

void BClipboardNotifierPrivate::dataChanged()
{
    bool b = !QApplication::clipboard()->text().isEmpty();
    bool bb = (b != textDataAvailable);
    textDataAvailable = b;
    if (bb)
        QMetaObject::invokeMethod(q_func(), "textDataAvailableChanged", Q_ARG(bool, b));
}

/*============================================================================
================================ BClipboardNotifier ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BClipboardNotifier::BClipboardNotifier() :
    QObject(0), BBaseObject(*new BClipboardNotifierPrivate(this))
{
    bTest(!_m_self, "BClipboardNotifier", "There should be only one instance of BClipboardNotifier");
    if (_m_self)
        _m_self->d_func()->detach();
    else
        qAddPostRoutine(&clipboardNotifierCleanup);
    d_func()->init();
    _m_self = this;
}

BClipboardNotifier::~BClipboardNotifier()
{
    _m_self = 0;
}

/*============================== Static public methods =====================*/

BClipboardNotifier *BClipboardNotifier::instance()
{
    return _m_self;
}

/*============================== Public methods ============================*/

bool BClipboardNotifier::textDataAvailable() const
{
    return d_func()->textDataAvailable;
}

/*============================== Static protected variables ================*/

BClipboardNotifier *BClipboardNotifier::_m_self = 0;
