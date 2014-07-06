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

#ifndef BGENERICSOCKET_P_H
#define BGENERICSOCKET_P_H

class BGenericSocketPrivate;

#include "bgenericsocket.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QAbstractSocket>
#include <QLocalSocket>
#include <QObject>
#include <QPointer>

/*============================================================================
================================ BGenericSocketPrivate =======================
============================================================================*/

class B_NETWORK_EXPORT BGenericSocketPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BGenericSocket)
public:
    QPointer<QAbstractSocket> asocket;
    QPointer<QLocalSocket> lsocket;
public:
    explicit BGenericSocketPrivate(BGenericSocket *q);
    ~BGenericSocketPrivate();
public:
    void connectIODevice();
    void disconnectIODevice();
    void init();
    void setSocket(QAbstractSocket *socket);
    void setSocket(QLocalSocket *socket);
public Q_SLOTS:
    void lsocketError(QLocalSocket::LocalSocketError socketError);
    void lsocketStateChanged(QLocalSocket::LocalSocketState socketState);
private:
    Q_DISABLE_COPY(BGenericSocketPrivate)
};

#endif // BGENERICSOCKET_P_H
