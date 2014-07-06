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

#ifndef BAPPLICATIONSERVER_P_H
#define BAPPLICATIONSERVER_P_H

class BGenericServer;

#include "bapplicationserver.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QObject>
#include <QString>

/*============================================================================
================================ BApplicationServerPrivate ===================
============================================================================*/

class B_NETWORK_EXPORT BApplicationServerPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BApplicationServer)
public:
    const int OperationTimeout;
    const quint16 Port;
    const QString ServerName;
public:
    BGenericServer *server;
public:
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    explicit BApplicationServerPrivate(BApplicationServer *q, quint16 port, int timeout);
#else
    explicit BApplicationServerPrivate(BApplicationServer *q, const QString &serverName, int timeout);
#endif
    explicit BApplicationServerPrivate(BApplicationServer *q, const QString &serverName, quint16 port, int timeout);
    ~BApplicationServerPrivate();
public:
    void init();
public Q_SLOTS:
    void newPendingConnection();
private:
    Q_DISABLE_COPY(BApplicationServerPrivate)
};

#endif // BAPPLICATIONSERVER_P_H
