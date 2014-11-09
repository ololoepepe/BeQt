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

#ifndef BGENERICSERVER_P_H
#define BGENERICSERVER_P_H

class BGenericSocket;

#include "bgenericserver.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QLocalServer>
#include <QObject>
#include <QPointer>
#include <QQueue>
#include <QTcpServer>

/*============================================================================
================================ BLocalServer ================================
============================================================================*/

class B_NETWORK_EXPORT BLocalServer : public QLocalServer
{
    Q_OBJECT
public:
    explicit BLocalServer(QObject *parent = 0);
    ~BLocalServer();
protected:
    void incomingConnection(quintptr socketDescriptor);
Q_SIGNALS:
    void newConnection(int socketDescriptor);
};

/*============================================================================
================================ BTcpServer ==================================
============================================================================*/

class B_NETWORK_EXPORT BTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit BTcpServer(QObject *parent = 0);
    ~BTcpServer();
protected:
    void incomingConnection(int handle);
Q_SIGNALS:
    void newConnection(int socketDescriptor);
};

/*============================================================================
================================ BGenericServerPrivate =======================
============================================================================*/

class B_NETWORK_EXPORT BGenericServerPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BGenericServer)
public:
    QPointer<QLocalServer> lserver;
    int maxPending;
    BGenericServer::ServerType type;
    QQueue<BGenericSocket *> socketQueue;
    QPointer<QTcpServer> tserver;
public:
    explicit BGenericServerPrivate(BGenericServer *q);
    ~BGenericServerPrivate();
public:
    void init();
public Q_SLOTS:
    void newConnection(int socketDescriptor);
private:
    Q_DISABLE_COPY(BGenericServerPrivate)
};

#endif // BGENERICSERVER_P_H
