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

#include "bgenericserver.h"

#include "bgenericsocket.h"
#include "bgenericserver_p.h"

#include <QAbstractSocket>
#include <QDebug>
#include <QHostAddress>
#include <QLocalServer>
#include <QMetaObject>
#include <QObject>
#include <QPointer>
#include <QQueue>
#include <QSslSocket>
#include <QString>
#include <QTcpServer>

/*============================================================================
================================ BLocalServer ================================
============================================================================*/

/*============================== Public constructors =======================*/

BLocalServer::BLocalServer(QObject *parent) :
    QLocalServer(parent)
{
    //
}

BLocalServer::~BLocalServer()
{
    //
}

/*============================== Protected methods =========================*/

void BLocalServer::incomingConnection(quintptr socketDescriptor)
{
    Q_EMIT newConnection(socketDescriptor);
}

/*============================================================================
================================ BTcpServer ==================================
============================================================================*/

/*============================== Public constructors =======================*/

BTcpServer::BTcpServer(QObject *parent) :
    QTcpServer(parent)
{
    //
}

BTcpServer::~BTcpServer()
{
    //
}

/*============================== Protected methods =========================*/

void BTcpServer::incomingConnection(int handle)
{
    Q_EMIT newConnection(handle);
}

/*============================================================================
================================ BGenericServerPrivate =======================
============================================================================*/

/*============================== Public constructors =======================*/

BGenericServerPrivate::BGenericServerPrivate(BGenericServer *q) :
    BBaseObjectPrivate(q)
{
    //
}

BGenericServerPrivate::~BGenericServerPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BGenericServerPrivate::init()
{
    maxPending = 10;
    type = BGenericServer::NoServer;
}

/*============================== Public slots ==============================*/

void BGenericServerPrivate::newConnection(int socketDescriptor)
{
    B_Q(BGenericServer);
    if (maxPending) {
        BGenericSocket *socket = q->createSocket(socketDescriptor);
        if (!socket || !socket->isValid() || !(socket->socketType() & q->serverType()))
            return;
        if (maxPending > 0 && socketQueue.size() >= maxPending) {
            socket->close();
            socket->deleteLater();
            QMetaObject::invokeMethod(q, "connectionOverflow");
            return;
        }
        socket->setParent(q);
        socketQueue.enqueue(socket);
        QMetaObject::invokeMethod(q, "newPendingConnection");
    } else {
        QMetaObject::invokeMethod(q, "newConnection", Q_ARG(int, socketDescriptor));
    }
}

/*============================================================================
================================ BGenericServer ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BGenericServer::BGenericServer(ServerType type, QObject *parent) :
    QObject(parent), BBaseObject(*new BGenericServerPrivate(this))
{
    d_func()->init();
    B_D(BGenericServer);
    d->type = type;
    switch (type) {
    case LocalServer: {
        BLocalServer *server = new BLocalServer(this);
        connect(server, SIGNAL(newConnection(int)), d, SLOT(newConnection(int)));
        d->lserver = server;
        break;
    }
    case TcpServer:
    case SslServer: {
        BTcpServer *server = new BTcpServer(this);
        connect(server, SIGNAL(newConnection(int)), d, SLOT(newConnection(int)));
        d->tserver = server;
    }
    default: {
        break;
    }
    }
}

BGenericServer::~BGenericServer()
{
    //
}

/*============================== Protected constructors ====================*/

BGenericServer::BGenericServer(BGenericServerPrivate &d, QObject *parent) :
    QObject(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BGenericServer::close()
{
    B_D(BGenericServer);
    !d->tserver.isNull() ? d->tserver->close() : d->lserver->close();
}

QString BGenericServer::errorString() const
{
    if (!isServerSet())
        return "";
    const B_D(BGenericServer);
    return !d->tserver.isNull() ? d->tserver->errorString() : d->lserver->errorString();
}

bool BGenericServer::hasPendingConnections() const
{
    return !d_func()->socketQueue.isEmpty();
}

bool BGenericServer::isListening() const
{
    if (!isServerSet())
        return false;
    const B_D(BGenericServer);
    return !d->tserver.isNull() ? d->tserver->isListening() : d->lserver->isListening();
}

bool BGenericServer::isServerSet() const
{
    const B_D(BGenericServer);
    return !d->tserver.isNull() || !d->lserver.isNull();
}

bool BGenericServer::listen(const QString &address, quint16 port)
{
    if (!isServerSet())
        return false;
    const B_D(BGenericServer);
    return !d->tserver.isNull() ? d->tserver->listen(QHostAddress(address), port) : d->lserver->listen(address);
}

QLocalServer *BGenericServer::localServer() const
{
    return d_func()->lserver.data();
}

int BGenericServer::maxPendingConnections() const
{
    return d_func()->maxPending;
}

BGenericSocket *BGenericServer::nextPendingConnection()
{
    return hasPendingConnections() ? d_func()->socketQueue.dequeue() : 0;
}

QString BGenericServer::serverAddress() const
{
    if (!isServerSet())
        return "";
    const B_D(BGenericServer);
    return !d->tserver.isNull() ? d->tserver->serverAddress().toString() : d->lserver->serverName();
}

QAbstractSocket::SocketError BGenericServer::serverError() const
{
    if (!isServerSet())
        return QAbstractSocket::UnknownSocketError;
    const B_D(BGenericServer);
    return !d->tserver.isNull() ? d->tserver->serverError() : d->lserver->serverError();
}

BGenericServer::ServerType BGenericServer::serverType() const
{
    return d_func()->type;
}

void BGenericServer::setMaxPendingConnections(int numConnections)
{
    d_func()->maxPending = numConnections > 0 ? numConnections : 0;
}

QTcpServer *BGenericServer::tcpServer() const
{
    return d_func()->tserver.data();
}

bool BGenericServer::waitForNewConnection(int msec, bool *timedOut)
{
    if (!isServerSet())
        return false;
    B_D(BGenericServer);
    return !d->tserver.isNull() ? d->tserver->waitForNewConnection(msec, timedOut) :
                                  d->lserver->waitForNewConnection(msec, timedOut);
}

/*============================== Protected methods =========================*/

BGenericSocket *BGenericServer::createSocket(int socketDescriptor)
{
    BGenericSocket *socket = 0;
    switch (serverType()) {
    case TcpServer:
        socket = new BGenericSocket(BGenericSocket::TcpSocket);
        socket->setSocketDescriptor(socketDescriptor);
        break;
    case SslServer:
        socket = new BGenericSocket(BGenericSocket::SslSocket);
        socket->setSocketDescriptor(socketDescriptor);
        break;
    case LocalServer:
        socket = new BGenericSocket(BGenericSocket::LocalSocket);
        socket->setSocketDescriptor(socketDescriptor);
    default:
        break;
    }
    return socket;
}
