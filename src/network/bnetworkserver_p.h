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
class BNetworkServerThread;
class BNetworkServerPrivate;
class BGenericSocket;

#include "bnetworkserver.h"

#include "bgenericserver.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QList>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QThread>

/*============================================================================
================================ BNetworkServerWorker ========================
============================================================================*/

class B_NETWORK_EXPORT BNetworkServerWorker : public QObject
{
    Q_OBJECT
public:
    BNetworkServerPrivate * const ServerPrivate;
    BNetworkServerThread * const Thread;
public:
    explicit BNetworkServerWorker(BNetworkServerPrivate *sp, BNetworkServerThread *thread);
    ~BNetworkServerWorker();
public Q_SLOTS:
    void addConnection(int socketDescriptor, const QString &serverAddress, quint16 serverPort);
    void disconnected();
Q_SIGNALS:
    void connectionAdded(QObject *obj);
    void disconnected(QObject *obj);
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
    BNetworkServerWorker * const Worker;
public:
    QList<BNetworkConnection *> connections;
public:
    explicit BNetworkServerThread(BNetworkServerPrivate *serverPrivate);
    ~BNetworkServerThread();
public:
    void addConnection(int socketDescriptor, const QString &serverAddress, quint16 serverPort);
    int connectionCount() const;
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
    const BGenericServer::ServerType Type;
public:
    QStringList banned;
    mutable QMutex *connectionMutex;
    int maxConnectionCount;
    int maxPendingConnections;
    int maxThreadCount;
    QList<BGenericServer *> servers;
    QList<BNetworkServerThread *> threads;
public:
    explicit BNetworkServerPrivate(BNetworkServer *q, BGenericServer::ServerType type);
    ~BNetworkServerPrivate();
public:
    void close();
    int connectionCount() const;
    BNetworkConnection *createConnection(BGenericSocket *socket, const QString &serverAddress, quint16 serverPort);
    BGenericSocket *createSocket();
    void emitConnectionAboutToBeRemoved(BNetworkConnection *connection);
    void emitConnectionAdded(BNetworkConnection *connection);
    void emitBannedUserConnectionDenied(const QString &address);
    BNetworkServerThread *getOptimalThread();
    bool handleBanned(BGenericSocket *socket);
    bool handleIncomingConnection(BGenericSocket *socket);
    void init();
    bool isBanned(const QString &address) const;
    bool listen(const QString &address, quint16 port = 0);
public Q_SLOTS:
    void finished();
    void newConnection(int socketDescriptor);
private:
    Q_DISABLE_COPY(BNetworkServerPrivate)
    friend class BNetworkServerWorkerPrivate;
};

#endif // BNETWORKSERVER_P_H
