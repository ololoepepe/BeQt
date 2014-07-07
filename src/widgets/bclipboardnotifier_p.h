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

#ifndef BHELPBROWSER_P_H
#define BHELPBROWSER_P_H

#include "bclipboardnotifier.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QObject>

/*============================================================================
================================ BClipboardNotifierPrivate ===================
============================================================================*/

class B_WIDGETS_EXPORT BClipboardNotifierPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BClipboardNotifier)
    B_DECLARE_PUBLIC_S(BClipboardNotifier)
public:
    bool colorAvailable;
    bool htmlAvailable;
    bool imageAvailable;
    bool textAvailable;
    bool urlsAvailable;
public:
    explicit BClipboardNotifierPrivate(BClipboardNotifier *q);
    ~BClipboardNotifierPrivate();
public:
    void init();
public Q_SLOTS:
    void dataChanged();
private:
    Q_DISABLE_COPY(BClipboardNotifierPrivate)
};

#endif // BHELPBROWSER_P_H
