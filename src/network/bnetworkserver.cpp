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

#include "bnetworkserver.h"
#include "bnetworkconnection.h"
#include "bnetworkserver_p.h"
#include "bgenericserver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbaseobject_p.h>
#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BSpamNotifier>
#include <BeQtCore/BSignalDelayProxy>

#include <QObject>
#include <QList>
#include <QThread>
#include <QString>
#include <QMetaObject>
#include <QMutex>
#include <QMutexLocker>
#include <QMap>
#include <QStringList>
#include <QTcpServer>

#include <QDebug>

/*============================================================================
================================ BNetworkServerWorker ========================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkServerWorker::BNetworkServerWorker(BNetworkServerPrivate *sp, BNetworkServerThread *thread) :
    QObject(0), ServerPrivate(sp), Thread(thread)
{
    //
}

BNetworkServerWorker::~BNetworkServerWorker()
{
    //
}

/*============================== Public slots ==============================*/

void BNetworkServerWorker::addConnection(int socketDescriptor, const QString &serverAddress, quint16 serverPort)
{
    BGenericSocket *s = ServerPrivate->createSocket();
    if (!s)
        return;
    if (!s->setSocketDescriptor(socketDescriptor) || !s->isValid())
    {
        delete s;
        return;
    }
    QMutexLocker locker(ServerPrivate->connectionMutex);
    if (ServerPrivate->banned.contains(s->peerAddress()))
    {
        s->disconnectFromHost();
        delete s;
        return;
    }
    BNetworkConnection *c = ServerPrivate->createConnection(s, serverAddress, serverPort);
    if (!c)
        return;
    if (!c->isValid())
    {
        delete c;
        return;
    }
    BSignalDelayProxy *proxy = new BSignalDelayProxy(c);
    connect(c, SIGNAL(disconnected()), proxy, SLOT(trigger()));
    connect(c, SIGNAL(error(QAbstractSocket::SocketError)), proxy, SLOT(trigger()));
    connect(proxy, SIGNAL(triggered()), this, SLOT(disconnected()));
    Thread->connections << c;
    ServerPrivate->emitConnectionAdded(c);
}

void BNetworkServerWorker::disconnected()
{
    BSignalDelayProxy *proxy = qobject_cast<BSignalDelayProxy *>(sender());
    BNetworkConnection *c = proxy ? qobject_cast<BNetworkConnection *>(proxy->parent()) : 0;
    if (!c)
        return;
    disconnect(proxy, SIGNAL(triggered()), this, SLOT(disconnected()));
    c->close();
    QMutexLocker locker(ServerPrivate->connectionMutex);
    ServerPrivate->emitConnectionAboutToBeRemoved(c);
    Thread->connections.removeAll(c);
    bool b = Thread->connections.isEmpty();
    c->deleteLater();
    if (b)
        QMetaObject::invokeMethod(Thread, "quit", Qt::QueuedConnection);
}

/*============================================================================
================================ BNetworkServerThread ========================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkServerThread::BNetworkServerThread(BNetworkServerPrivate *serverPrivate) :
    QThread(0), Worker(new BNetworkServerWorker(serverPrivate, this))
{
    Worker->moveToThread(this);
}

BNetworkServerThread::~BNetworkServerThread()
{
    Worker->deleteLater();
}

/*============================== Public methods ============================*/

void BNetworkServerThread::addConnection(int socketDescriptor, const QString &serverAddress, quint16 serverPort)
{
    QMetaObject::invokeMethod(Worker, "addConnection", Qt::QueuedConnection, Q_ARG(int, socketDescriptor),
                              Q_ARG(QString, serverAddress), Q_ARG(quint16, serverPort));
}

int BNetworkServerThread::connectionCount() const
{
    return connections.size();
}

/*============================================================================
================================ BNetworkServerPrivate =======================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkServerPrivate::BNetworkServerPrivate(BNetworkServer *q, BGenericServer::ServerType type) :
    BBaseObjectPrivate(q), Type(type)
{
    //
}

BNetworkServerPrivate::~BNetworkServerPrivate()
{
    foreach (BNetworkServerThread *t, threads)
    {
        disconnect(t, SIGNAL(finished()), this, SLOT(finished()));
        t->quit();
        t->wait(1000);
    }
    delete connectionMutex;
}

/*============================== Public methods ============================*/

void BNetworkServerPrivate::init()
{
    connectionMutex = new QMutex(QMutex::Recursive);
    maxConnectionCount = 0;
    maxThreadCount = 0;
    maxPendingConnections = 0;
    spamNotifier = new BSpamNotifier(this);
    spamNotifier->setEnabled(false);
    connect(spamNotifier, SIGNAL(spammed(int)), this, SLOT(spammed()));
}

BNetworkConnection *BNetworkServerPrivate::createConnection(BGenericSocket *socket, const QString &serverAddress,
                                                            quint16 serverPort)
{
    return q_func()->createConnection(socket, serverAddress, serverPort);
}

BGenericSocket *BNetworkServerPrivate::createSocket()
{
    BGenericSocket *s = q_func()->createSocket();
    if (s)
        spamNotifier->spam();
    return s;
}

BNetworkServerThread *BNetworkServerPrivate::getOptimalThread()
{
    QMutexLocker locker(connectionMutex);
    int ccount = connectionCount();
    if (maxConnectionCount > 0 && ccount == maxConnectionCount)
        return 0;
    if (maxThreadCount > 0 && ccount == maxThreadCount)
    {
        int cc = threads.first()->connectionCount();
        int ind = 0;
        for (int i = 0; i < threads.size(); ++i)
        {
            int ccc = threads.at(i)->connectionCount();
            if (ccc < cc)
            {
                cc = ccc;
                ind = i;
            }
        }
        return threads[ind];
    }
    else
    {
        BNetworkServerThread *t = new BNetworkServerThread(this);
        QObject::connect(t, SIGNAL(finished()), this, SLOT(finished()));
        t->start();
        threads << t;
        return t;
    }
}

int BNetworkServerPrivate::connectionCount() const
{
    //Must lock
    int count = 0;
    foreach (BNetworkServerThread *t, threads)
        count += t->connectionCount();
    return count;
}

void BNetworkServerPrivate::emitConnectionAdded(BNetworkConnection *connection)
{
    //The signal is invoked in the connection thread, not in the server thread
    //You must connect the slots using Qt::DirectConnection
    QMetaObject::invokeMethod(q_func(), "connectionAdded", Qt::DirectConnection,
                              Q_ARG(BNetworkConnection *, connection));
}

void BNetworkServerPrivate::emitConnectionAboutToBeRemoved(BNetworkConnection *connection)
{
    //The signal is invoked in the connection thread, not in the server thread
    //You must connect the slots using Qt::DirectConnection
    QMetaObject::invokeMethod(q_func(), "connectionAboutToBeRemoved", Qt::DirectConnection,
                              Q_ARG(BNetworkConnection *, connection));
}

bool BNetworkServerPrivate::listen(const QString &address, quint16 port)
{
    if (BGenericServer::NoServer == Type)
        return false;
    BGenericServer *server = new BGenericServer(Type, this);
    server->setMaxPendingConnections(maxPendingConnections);
    connect(server, SIGNAL(newConnection(int)), this, SLOT(newConnection(int)));
    bool b = server->listen(address, port);
    if (!b)
        delete server;
    else
        servers << server;
    return b;
}

void BNetworkServerPrivate::close()
{
    foreach (BGenericServer *s, servers)
        s->close();
    servers.clear();
}

/*============================== Public slots ==============================*/

void BNetworkServerPrivate::newConnection(int socketDescriptor)
{
    BGenericServer *server = qobject_cast<BGenericServer *>(sender());
    if (!server)
        return;
    BNetworkServerThread *t = getOptimalThread();
    if (t)
    {
        quint16 port = (server->serverType() != BGenericServer::LocalServer) ? server->tcpServer()->serverPort() : 0;
        t->addConnection(socketDescriptor, server->serverAddress(), port);
    }
    else
    {
        //If connection limit is reached, every new connection is automatically removed.
        //No notifications are sent to clients, they are just disconnected.
        //If you want to send a notification to a client before disconnecting, set unlimited connection count,
        //and then check current connection count from within the newly connected client.
        //Then you may send some notifications and disconnect the client.
        BGenericSocket *s = new BGenericSocket(BGenericSocket::TcpSocket);
        s->setSocketDescriptor(socketDescriptor);
        s->disconnectFromHost();
        delete s;
    }
}

void BNetworkServerPrivate::finished()
{
    BNetworkServerThread *t = qobject_cast<BNetworkServerThread *>(sender());
    if (!t)
        return;
    threads.removeAll(t);
    t->deleteLater();
}

void BNetworkServerPrivate::spammed()
{
    //
}

/*============================================================================
================================ BNetworkServer ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkServer::BNetworkServer(BGenericServer::ServerType type, QObject *parent) :
    QObject(parent), BBaseObject(*new BNetworkServerPrivate(this, type))
{
    d_func()->init();
}

BNetworkServer::~BNetworkServer()
{
    //
}

/*============================== Protected constructors ====================*/

BNetworkServer::BNetworkServer(BNetworkServerPrivate &d, QObject *parent) :
    QObject(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BNetworkServer::setMaxConnectionCount(int count)
{
    d_func()->maxConnectionCount = count > 0 ? count : 0;
}

void BNetworkServer::setMaxThreadCount(int count)
{
    d_func()->maxThreadCount = count > 0 ? count : 0;
}

void BNetworkServer::lock()
{
    d_func()->connectionMutex->lock();
}

void BNetworkServer::unlock()
{
    d_func()->connectionMutex->unlock();
}

bool BNetworkServer::tryLock()
{
    return d_func()->connectionMutex->tryLock();
}

bool BNetworkServer::isValid() const
{
    return d_func()->Type != BGenericServer::NoServer;
}

bool BNetworkServer::isListening() const
{
    return !d_func()->servers.isEmpty();
}

bool BNetworkServer::listen(const QString &address)
{
    return listen(QStringList() << address) > 0;
}

bool BNetworkServer::listen(const QString &address, quint16 port)
{
    return listen(address + ":" + QString::number(port)) > 0;
}

int BNetworkServer::listen(const QStringList &addresses)
{
    if (isListening())
        return false;
    int count = 0;
    foreach (const QString &address, addresses) {
        if (address.isEmpty())
            continue;
        QStringList sl = address.split(':');
        if (sl.size() > 2)
            continue;
        if (sl.first().isEmpty())
            continue;
        quint16 port = 0;
        if (sl.size() == 2) {
            bool ok = false;
            port = sl.last().toUInt(&ok);
            if (!ok)
                continue;
        }
        if (!d_func()->listen(sl.first(), port))
            continue;
        ++count;
    }
    return count;
}

BGenericServer::ServerType BNetworkServer::serverType() const
{
    return d_func()->Type;
}

int BNetworkServer::maxConnectionCount() const
{
    return d_func()->maxConnectionCount;
}

int BNetworkServer::currentConnectionCount() const
{
    return d_func()->connectionCount();
}

int BNetworkServer::maxThreadCount() const
{
    return d_func()->maxThreadCount;
}

int BNetworkServer::currentThreadCount() const
{
    return d_func()->threads.size();
}

BSpamNotifier *BNetworkServer::spamNotifier() const
{
    return d_func()->spamNotifier;
}

QList<BNetworkConnection *> BNetworkServer::connections() const
{
    //Must lock
    QList<BNetworkConnection *> list;
    foreach (BNetworkServerThread *t, d_func()->threads)
        list << t->connections;
    return list;
}

QStringList BNetworkServer::banned() const
{
    //Must lock
    QStringList list = d_func()->banned.toList();
    return list;
}

/*============================== Public slots ==============================*/

void BNetworkServer::close()
{
    d_func()->close();
}

void BNetworkServer::ban(const QString &address)
{
    ban(QStringList() << address);
}

void BNetworkServer::ban(const QStringList &addresses)
{
    lock();
    foreach (const QString &s, addresses)
        d_func()->banned.insert(s);
    unlock();
}

void BNetworkServer::unban(const QString &address)
{
    unban(QStringList() << address);
}

void BNetworkServer::unban(const QStringList &addresses)
{
    lock();
    foreach (const QString &s, addresses)
        d_func()->banned.remove(s);
    unlock();
}

void BNetworkServer::clearBanList()
{
    lock();
    d_func()->banned.clear();
    unlock();
}

/*============================== Protected methods =========================*/

BNetworkConnection *BNetworkServer::createConnection(BGenericSocket *socket, const QString &, quint16)
{
    return new BNetworkConnection(this, socket);
}

BGenericSocket *BNetworkServer::createSocket()
{
    return new BGenericSocket(BGenericSocket::TcpSocket);
}
