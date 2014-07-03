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

#ifndef BNETWORKSERVER_P_H
#define BNETWORKSERVER_P_H

class BNetworkConnection;
class BNetworkServerWorker;
class BNetworkServerThread;
class BNetworkServerPrivate;
class BNetworkServer;
class BGenericSocket;
class BSpamNotifier;

#include "bnetworkserver.h"
#include "bgenericserver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbaseobject_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QList>
#include <QPointer>
#include <QString>
#include <QThread>
#include <QMutex>
#include <QSet>

/*============================================================================
================================ BNetworkServerWorker ========================
============================================================================*/

class B_NETWORK_EXPORT BNetworkServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit BNetworkServerWorker(BNetworkServerPrivate *sp, BNetworkServerThread *thread);
    ~BNetworkServerWorker();
public Q_SLOTS:
    void addConnection(int socketDescriptor);
    void disconnected();
Q_SIGNALS:
    void connectionAdded(QObject *obj);
    void disconnected(QObject *obj);
public:
    BNetworkServerPrivate *const ServerPrivate;
    BNetworkServerThread *const Thread;
private:
    Q_DISABLE_COPY(BNetworkServerWorker)
};

/*============================================================================
================================ BNetworkServerThread ========================
============================================================================*/

class B_NETWORK_EXPORT BNetworkServerThread : public QThread
{
    Q_OBJECT
public:
    explicit BNetworkServerThread(BNetworkServerPrivate *serverPrivate);
    ~BNetworkServerThread();
public:
    void addConnection(int socketDescriptor);
    int connectionCount() const;
public:
    BNetworkServerWorker *const Worker;
public:
    QList<BNetworkConnection *> connections;
private:
    Q_DISABLE_COPY(BNetworkServerThread)
};

/*============================================================================
================================ BNetworkServerPrivate =======================
============================================================================*/

class B_NETWORK_EXPORT BNetworkServerPrivate : public BBaseObjectPrivate
{
    B_DECLARE_PUBLIC(BNetworkServer)
    Q_OBJECT
public:
    explicit BNetworkServerPrivate(BNetworkServer *q);
    ~BNetworkServerPrivate();
public:
    void init();
    BNetworkConnection *createConnection(BGenericSocket *socket);
    BGenericSocket *createSocket();
    BNetworkServerThread *getOptimalThread();
    int connectionCount() const;
public Q_SLOTS:
    void emitConnectionAdded(BNetworkConnection *connection);
    void emitConnectionAboutToBeRemoved(BNetworkConnection *connection);
    void newConnection(int socketDescriptor);
    void finished();
    void spammed();
public:
    QPointer<BGenericServer> server;
    QList<BNetworkServerThread *> threads;
    int maxConnectionCount;
    int maxThreadCount;
    BSpamNotifier *spamNotifier;
    QSet<QString> banned;
    mutable QMutex *connectionMutex;
private:
    Q_DISABLE_COPY(BNetworkServerPrivate)
    friend class BNetworkServerWorkerPrivate;
};

#endif // BNETWORKSERVER_P_H
