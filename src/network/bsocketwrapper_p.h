/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtNetwork module of the BeQt library.
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

#ifndef BSOCKETWRAPPER_P_H
#define BSOCKETWRAPPER_P_H

class BSocketWrapperPrivate;

#include "bgenericsocket.h"
#include "bsocketwrapper.h"
#include "bnetworkoperationmetadata.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QAbstractSocket>
#include <QPointer>

/*============================================================================
================================ BSocketWrapperPrivate =======================
============================================================================*/

class B_NETWORK_EXPORT BSocketWrapperPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSocketWrapper)
public:
    explicit BSocketWrapperPrivate(BSocketWrapper *q);
    ~BSocketWrapperPrivate();
public:
    void init();
    void resetIn();
    void resetOut();
public Q_SLOTS:
    void bytesWritten(qint64 bytes);
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void readyRead();
public:
    QPointer<BGenericSocket> socket;
    qint64 bytesInTotal;
    BNetworkOperationMetaData metaIn;
    qint64 bytesOutTotal;
    qint64 bytesOutReady;
    BNetworkOperationMetaData metaOut;
    int comprLvl;
    qint64 criticalBufferSize;
    bool closeOnCriticalBufferSize;
private:
    Q_DISABLE_COPY(BSocketWrapperPrivate)
};

#endif // BSOCKETWRAPPER_P_H
