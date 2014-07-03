/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
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

#ifndef BSIGNALDELAYPROXY_P_H
#define BSIGNALDELAYPROXY_P_H

class QTimer;

#include "bsignaldelayproxy.h"
#include "bglobal.h"
#include "bbaseobject_p.h"

#include <QObject>
#include <QVariant>

/*============================================================================
================================ BSignalDelayProxyPrivate ====================
============================================================================*/

class B_CORE_EXPORT BSignalDelayProxyPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSignalDelayProxy)
public:
    explicit BSignalDelayProxyPrivate(BSignalDelayProxy *q);
    ~BSignalDelayProxyPrivate();
public:
    void init();
    void trigger( const QVariant &data = QVariant() );
public Q_SLOTS:
    void timeout();
public:
    QTimer *tmrIntermediate;
    QTimer *tmrMaximum;
    int intermediateDelay;
    int maximumDelay;
    QVariant lastData;
private:
    Q_DISABLE_COPY(BSignalDelayProxyPrivate)
};

#endif //BSIGNALDELAYPROXY_P_H
