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

#include "bremotelogger.h"

#include "bgenericsocket.h"
#include "bnetworkconnection.h"
#include "bnetworkoperation.h"
#include "bnetworkserver.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BLogger>
#include <BeQtCore/private/blogger_p.h>

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QThread>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ BRemoteLoggerPrivate ========================
============================================================================*/

class BRemoteLoggerPrivate : public BLoggerPrivate
{
    B_DECLARE_PUBLIC(BRemoteLogger)
public:
    mutable QList< QPointer<BNetworkConnection> > connections;
    bool logToRemote;
    mutable QMutex remoteMutex;
    QPointer<BNetworkServer> server;
    QPointer<BGenericSocket> socket;
    int timeout;
public:
    explicit BRemoteLoggerPrivate(BRemoteLogger *q);
    ~BRemoteLoggerPrivate();
public:
    QList<BNetworkConnection *> getConnections() const;
    void init();
    void removeSocket();
    void tryLogToRemote(const QString &text, BLogger::Level lvl);
private:
    Q_DISABLE_COPY(BRemoteLoggerPrivate)
};

/*============================================================================
================================ BRemoteLoggerPrivate ========================
============================================================================*/

/*============================== Public constructors =======================*/

BRemoteLoggerPrivate::BRemoteLoggerPrivate(BRemoteLogger *q) :
    BLoggerPrivate(q)
{
    //
}

BRemoteLoggerPrivate::~BRemoteLoggerPrivate()
{
    removeSocket();
}

/*============================== Public methods ============================*/

QList<BNetworkConnection *> BRemoteLoggerPrivate::getConnections() const
{
    QList<BNetworkConnection *> list;
    for (int i = connections.size() - 1; i >= 0; --i) {
        if (connections.at(i).isNull())
            connections.removeAt(i);
        else
            list << connections.at(i).data();
    }
    return list;
}

void BRemoteLoggerPrivate::init()
{
    logToRemote = true;
    timeout = 10 * BeQt::Second;
}

void BRemoteLoggerPrivate::removeSocket()
{
    if (!socket.isNull() && (!socket->parent() || socket->parent() == this))
        socket->deleteLater();
}

void BRemoteLoggerPrivate::tryLogToRemote(const QString &text, BLogger::Level lvl)
{
    QMutexLocker locker(&remoteMutex);
    if (!logToRemote)
        return;
    if (!socket.isNull() && socket->isWritable()) {
        socket->write( text.toUtf8() );
        socket->flush();
        socket->waitForBytesWritten(timeout);
    }
    QList<BNetworkConnection *> list = getConnections();
    if (!server.isNull())
        list << server.data()->connections();
    QThread *t = thread();
    foreach (BNetworkConnection *c, list) {
        if (c->thread() != t)
            continue;
        QVariantMap m;
        m.insert("text", text);
        m.insert("level", lvl);
        BNetworkOperation *op = c->sendRequest("", m);
        op->setAutoDelete(true);
        op->setStartTimeout();
        op->setFinishTimeout();
    }
}

/*============================================================================
================================ BRemoteLogger ===============================
============================================================================*/

/*============================== Public constructors =======================*/

BRemoteLogger::BRemoteLogger(QObject *parent) :
    BLogger(*new BRemoteLoggerPrivate(this), parent)
{
    d_func()->init();
}

BRemoteLogger::BRemoteLogger(BGenericSocket *socket, QObject *parent) :
    BLogger(*new BRemoteLoggerPrivate(this), parent)
{
    d_func()->init();
    setRemote(socket);
}

BRemoteLogger::BRemoteLogger(const QString &hostName, quint16 port, QObject *parent) :
    BLogger(*new BRemoteLoggerPrivate(this), parent)
{
    d_func()->init();
    setRemote(hostName, port);
}

BRemoteLogger::BRemoteLogger(BNetworkConnection *c, QObject *parent) :
    BLogger(*new BRemoteLoggerPrivate(this), parent)
{
    d_func()->init();
    setRemote(c);
}

BRemoteLogger::BRemoteLogger(const QList<BNetworkConnection *> &list, QObject *parent) :
    BLogger(*new BRemoteLoggerPrivate(this), parent)
{
    d_func()->init();
    setRemote(list);
}

BRemoteLogger::BRemoteLogger(BNetworkServer *server, QObject *parent) :
    BLogger(*new BRemoteLoggerPrivate(this), parent)
{
    d_func()->init();
    setRemote(server);
}

BRemoteLogger::~BRemoteLogger()
{
    //
}

/*============================== Protected constructors ====================*/

BRemoteLogger::BRemoteLogger(BRemoteLoggerPrivate &d) :
    BLogger(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QList<BNetworkConnection *> BRemoteLogger::connections() const
{
    return d_func()->getConnections();
}

QString BRemoteLogger::hostName() const
{
    const B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    return !d->socket.isNull() ? d->socket->peerAddress() : QString();
}

bool BRemoteLogger::isLogToRemoteEnabled() const
{
    const B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    return d->logToRemote;
}

quint16 BRemoteLogger::port() const
{
    const B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    return !d->socket.isNull() ? d->socket->peerPort() : 0;
}

int BRemoteLogger::remoteTimeout() const
{
    const B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    return d->timeout;
}

BNetworkServer *BRemoteLogger::server() const
{
    return d_func()->server;
}

void BRemoteLogger::setLogToRemoteEnabled(bool enabled)
{
    B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    d->logToRemote = enabled;
}

void BRemoteLogger::setRemote(BGenericSocket *socket)
{
    B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    d->removeSocket();
    if (!socket || !socket->isWritable())
        return;
    d->socket = socket;
    if (!socket->parent())
        socket->setParent(this);
}

void BRemoteLogger::setRemote(const QString &hostName, quint16 port)
{
    B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    if (hostName.isEmpty() || !port)
        return setRemote((BGenericSocket *) 0);
    BGenericSocket *s = new BGenericSocket(BGenericSocket::TcpSocket, this);
    s->connectToHost(hostName, port);
    if (!s->waitForConnected(d->timeout)) {
        s->deleteLater();
        s = 0;
    }
    setRemote(s);
}

void BRemoteLogger::setRemote(BNetworkConnection *c)
{
    setRemote(QList<BNetworkConnection *>() << c);
}

void BRemoteLogger::setRemote(const QList<BNetworkConnection *> &list)
{
    B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    foreach (BNetworkConnection *c, list)
        d->connections << QPointer<BNetworkConnection>(c);
}

void BRemoteLogger::setRemote(BNetworkServer *server)
{
    B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    d->server = server;
}

void BRemoteLogger::setRemoteTimeout(int msecs)
{
    B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    if (msecs < -1)
        msecs = -1;
    d->timeout = msecs;
}

BGenericSocket *BRemoteLogger::socket() const
{
    return d_func()->socket.data();
}

/*============================== Protected methods =========================*/

void BRemoteLogger::userLog(const QString &text, Level level)
{
    d_func()->tryLogToRemote(text, level);
}
