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

class BGenericServerPrivate;
class BGenericSocket;

class QTcpServer;
class QLocalServer;

#include "bgenericserver.h"

#include <BeQtCore/private/bbaseobject_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QQueue>
#include <QPointer>
#include <QTcpServer>
#include <QLocalServer>

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
    explicit BGenericServerPrivate(BGenericServer *q);
    ~BGenericServerPrivate();
public:
    void init();
public Q_SLOTS:
    void newConnection(int socketDescriptor);
public:
    QPointer<QTcpServer> tserver;
    QPointer<QLocalServer> lserver;
    QQueue<BGenericSocket *> socketQueue;
    int maxPending;
private:
    Q_DISABLE_COPY(BGenericServerPrivate)
};

#endif // BGENERICSERVER_P_H
