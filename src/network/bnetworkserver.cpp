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
#include "bnetworkserver_p.h"

#include "bnetworkconnection.h"
#include "bgenericserver.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BSignalDelayProxy>
#include <BeQtCore/BTextTools>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QDebug>
#include <QList>
#include <QMap>
#include <QMetaObject>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTcpServer>
#include <QThread>

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
    if (!s->setSocketDescriptor(socketDescriptor) || !s->isValid()) {
        delete s;
        return;
    }
    QMutexLocker locker(ServerPrivate->connectionMutex);
    if (ServerPrivate->handleIncomingConnection(s))
        return;
    QString addr = s->peerAddress();
    if (ServerPrivate->isBanned(addr) && ServerPrivate->handleBanned(s)) {
        ServerPrivate->emitBannedUserConnectionDenied(addr);
        return;
    }
    BNetworkConnection *c = ServerPrivate->createConnection(s, serverAddress, serverPort);
    if (!c)
        return;
    if (!c->isValid()) {
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
    foreach (BNetworkServerThread *t, threads) {
        disconnect(t, SIGNAL(finished()), this, SLOT(finished()));
        t->quit();
        t->wait();
    }
    delete connectionMutex;
}

/*============================== Public methods ============================*/

void BNetworkServerPrivate::close()
{
    foreach (BGenericServer *s, servers)
        s->close();
    servers.clear();
}

int BNetworkServerPrivate::connectionCount() const
{
    QMutexLocker locker(connectionMutex);
    Q_UNUSED(locker)
    int count = 0;
    foreach (BNetworkServerThread *t, threads)
        count += t->connectionCount();
    return count;
}

BNetworkConnection *BNetworkServerPrivate::createConnection(BGenericSocket *socket, const QString &serverAddress,
                                                            quint16 serverPort)
{
    return q_func()->createConnection(socket, serverAddress, serverPort);
}

BGenericSocket *BNetworkServerPrivate::createSocket()
{
    return q_func()->createSocket();
}

void BNetworkServerPrivate::emitConnectionAboutToBeRemoved(BNetworkConnection *connection)
{
    //The signal is invoked in the connection thread, not in the server thread
    //You must connect the slots using Qt::DirectConnection
    QMetaObject::invokeMethod(q_func(), "connectionAboutToBeRemoved", Qt::DirectConnection,
                              Q_ARG(BNetworkConnection *, connection));
}

void BNetworkServerPrivate::emitConnectionAdded(BNetworkConnection *connection)
{
    //The signal is invoked in the connection thread, not in the server thread
    //You must connect the slots using Qt::DirectConnection
    QMetaObject::invokeMethod(q_func(), "connectionAdded", Qt::DirectConnection,
                              Q_ARG(BNetworkConnection *, connection));
}

void BNetworkServerPrivate::emitBannedUserConnectionDenied(const QString &address)
{
    //The signal is invoked in the connection thread, not in the server thread
    //You must connect the slots using Qt::DirectConnection
    QMetaObject::invokeMethod(q_func(), "bannedUserConnectionDenied", Qt::QueuedConnection, Q_ARG(QString, address));
}

BNetworkServerThread *BNetworkServerPrivate::getOptimalThread()
{
    QMutexLocker locker(connectionMutex);
    Q_UNUSED(locker)
    int ccount = connectionCount();
    if (maxConnectionCount > 0 && ccount == maxConnectionCount)
        return 0;
    if (maxThreadCount > 0 && ccount == maxThreadCount) {
        int cc = threads.first()->connectionCount();
        int ind = 0;
        for (int i = 0; i < threads.size(); ++i) {
            int ccc = threads.at(i)->connectionCount();
            if (ccc < cc) {
                cc = ccc;
                ind = i;
            }
        }
        return threads[ind];
    } else {
        BNetworkServerThread *t = new BNetworkServerThread(this);
        QObject::connect(t, SIGNAL(finished()), this, SLOT(finished()));
        t->start();
        threads << t;
        return t;
    }
}

bool BNetworkServerPrivate::handleBanned(BGenericSocket *socket)
{
    return q_func()->handleBanned(socket);
}

bool BNetworkServerPrivate::handleIncomingConnection(BGenericSocket *socket)
{
    return q_func()->handleIncomingConnection(socket);
}

void BNetworkServerPrivate::init()
{
    connectionMutex = new QMutex(QMutex::Recursive);
    maxConnectionCount = 0;
    maxThreadCount = 0;
    maxPendingConnections = 0;
}

bool BNetworkServerPrivate::isBanned(const QString &address) const
{
    return q_func()->isBanned(address);
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

/*============================== Public slots ==============================*/

void BNetworkServerPrivate::finished()
{
    BNetworkServerThread *t = qobject_cast<BNetworkServerThread *>(sender());
    if (!t)
        return;
    threads.removeAll(t);
    t->deleteLater();
}

void BNetworkServerPrivate::newConnection(int socketDescriptor)
{
    BGenericServer *server = qobject_cast<BGenericServer *>(sender());
    if (!server)
        return;
    BNetworkServerThread *t = getOptimalThread();
    if (t) {
        quint16 port = (server->serverType() != BGenericServer::LocalServer) ? server->tcpServer()->serverPort() : 0;
        t->addConnection(socketDescriptor, server->serverAddress(), port);
    } else {
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

QStringList BNetworkServer::banList() const
{
    QMutexLocker locker(d_func()->connectionMutex);
    return d_func()->banned;
}

QList<BNetworkConnection *> BNetworkServer::connections() const
{
    QMutexLocker locker(d_func()->connectionMutex);
    QList<BNetworkConnection *> list;
    foreach (BNetworkServerThread *t, d_func()->threads)
        list << t->connections;
    return list;
}

int BNetworkServer::currentConnectionCount() const
{
    return d_func()->connectionCount();
}

int BNetworkServer::currentThreadCount() const
{
    return d_func()->threads.size();
}

bool BNetworkServer::isListening() const
{
    return !d_func()->servers.isEmpty();
}

bool BNetworkServer::isValid() const
{
    return d_func()->Type != BGenericServer::NoServer;
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

void BNetworkServer::lock()
{
    d_func()->connectionMutex->lock();
}

int BNetworkServer::maxConnectionCount() const
{
    return d_func()->maxConnectionCount;
}

int BNetworkServer::maxThreadCount() const
{
    return d_func()->maxThreadCount;
}

BGenericServer::ServerType BNetworkServer::serverType() const
{
    return d_func()->Type;
}

void BNetworkServer::setMaxConnectionCount(int count)
{
    d_func()->maxConnectionCount = count > 0 ? count : 0;
}

void BNetworkServer::setMaxThreadCount(int count)
{
    d_func()->maxThreadCount = count > 0 ? count : 0;
}

bool BNetworkServer::tryLock()
{
    return d_func()->connectionMutex->tryLock();
}

void BNetworkServer::unlock()
{
    d_func()->connectionMutex->unlock();
}

/*============================== Public slots ==============================*/

void BNetworkServer::ban(const QString &address)
{
    ban(QStringList() << address);
}

void BNetworkServer::ban(const QStringList &addresses)
{
    QMutexLocker locker(d_func()->connectionMutex);
    foreach (const QString &s, addresses) {
        if (!d_func()->banned.contains(s))
            d_func()->banned << s;
    }
}

void BNetworkServer::clearBanList()
{
    QMutexLocker locker(d_func()->connectionMutex);
    d_func()->banned.clear();
}

void BNetworkServer::close()
{
    d_func()->close();
}

void BNetworkServer::setBanList(const QStringList &addresses)
{
    QMutexLocker locker(d_func()->connectionMutex);
    d_func()->banned = BTextTools::removeDuplicates(addresses, Qt::CaseInsensitive);
    d_func()->banned.removeAll("");
}

void BNetworkServer::unban(const QString &address)
{
    unban(QStringList() << address);
}

void BNetworkServer::unban(const QStringList &addresses)
{
    QMutexLocker locker(d_func()->connectionMutex);
    foreach (const QString &s, addresses)
        d_func()->banned.removeAll(s);
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

bool BNetworkServer::handleBanned(BGenericSocket *socket)
{
    if (!socket)
        return false;
    socket->disconnectFromHost();
    delete socket;
    return true;
}

bool BNetworkServer::handleIncomingConnection(BGenericSocket *)
{
    return false;
}

bool BNetworkServer::isBanned(const QString &address) const
{
    QMutexLocker locker(d_func()->connectionMutex);
    return d_func()->banned.contains(address);
}
