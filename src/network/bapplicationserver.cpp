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

#include "bapplicationserver.h"
#include "bapplicationserver_p.h"

#include "bgenericserver.h"
#include "bgenericsocket.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BBaseObject>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QLocalServer>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QStringList>

/*============================================================================
================================ BApplicationServerPrivate ===================
============================================================================*/

/*============================== Public constructors =======================*/

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
BApplicationServerPrivate::BApplicationServerPrivate(BApplicationServer *q, quint16 port, int timeout) :
    BBaseObjectPrivate(q), OperationTimeout(timeout), Port(port), ServerName(QString())
{
    //
}
#else
BApplicationServerPrivate::BApplicationServerPrivate(BApplicationServer *q, const QString &serverName, int timeout) :
    BBaseObjectPrivate(q), OperationTimeout(timeout), Port(0), ServerName(serverName)
{
        //
}
#endif

BApplicationServerPrivate::BApplicationServerPrivate(BApplicationServer *q, const QString &serverName, quint16 port,
                                                     int timeout) :
    BBaseObjectPrivate(q), OperationTimeout(timeout), Port(port), ServerName(serverName)
{
        //
}

BApplicationServerPrivate::~BApplicationServerPrivate()
{
    if (server->isListening())
        server->close();
    server->deleteLater();
}

/*============================== Public methods ============================*/

void BApplicationServerPrivate::init()
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    server = new BGenericServer(BGenericServer::TcpServer);
#else
    server = new BGenericServer(BGenericServer::LocalServer);
    server->localServer()->setSocketOptions(QLocalServer::WorldAccessOption);
#endif
    connect(server, SIGNAL(newPendingConnection()), this, SLOT(newPendingConnection()));
}

/*============================== Public slots ==============================*/

void BApplicationServerPrivate::newPendingConnection()
{
    QScopedPointer<BGenericSocket> s (server->nextPendingConnection());
    if (s.isNull())
        return;
    if (!s->waitForReadyRead(OperationTimeout))
        return;
    QByteArray ba = s->readAll();
    QDataStream in(ba);
    in.setVersion(BeQt::DataStreamVersion);
    bool message = false;
    QStringList args;
    in >> message;
    if (message) {
        in >> args;
        q_func()->handleMessage(args);
    } else {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(BeQt::DataStreamVersion);
        out << true;
        s->write(data);
        s->waitForBytesWritten(OperationTimeout);
    }
    s->close();
}

/*============================================================================
================================ BApplicationServer ==========================
============================================================================*/

/*============================== Public constructors =======================*/

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
BApplicationServer::BApplicationServer(quint16 port, int operationTimeout) :
    BBaseObject(*new BApplicationServerPrivate(this, port, operationTimeout))
{
    d_func()->init();
}
#else
BApplicationServer::BApplicationServer(const QString &serverName, int operationTimeout) :
    BBaseObject(*new BApplicationServerPrivate(this, serverName, operationTimeout))
{
    d_func()->init();
}
#endif

BApplicationServer::BApplicationServer(quint16 port, const QString &serverName, int operationTimeout) :
    BBaseObject(*new BApplicationServerPrivate(this, serverName, port, operationTimeout))
{
    d_func()->init();
}

BApplicationServer::~BApplicationServer()
{
    //
}

/*============================== Protected constructors ====================*/

BApplicationServer::BApplicationServer(BApplicationServerPrivate &d) :
    BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

bool BApplicationServer::isValid() const
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    return d_func()->Port;
#else
    return !d_func()->ServerName.isEmpty();
#endif
}

bool BApplicationServer::listen()
{
    if (!isValid())
        return false;
    B_D(BApplicationServer);
    if (d->server->isListening())
        return true;
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    return d->server->listen("127.0.0.1", d->Port);
#else
    return d->server->listen(d->ServerName);
#endif
}

bool BApplicationServer::testServer() const
{
    if (!isValid())
        return false;
    const B_D(BApplicationServer);
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(BeQt::DataStreamVersion);
    out << false;
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    BGenericSocket s(BGenericSocket::TcpSocket);
    s.connectToHost("127.0.0.1", d->Port);
#else
    BGenericSocket s(BGenericSocket::LocalSocket);
    s.connectToHost(d->ServerName);
#endif
    bool b = s.waitForConnected(d->OperationTimeout) && s.write(data) &&
            s.waitForBytesWritten(d->OperationTimeout) && s.waitForReadyRead(d->OperationTimeout);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (!b)
        QLocalServer::removeServer(d->ServerName);
#endif
    return b;
}

bool BApplicationServer::sendMessage(int &argc, char **argv)
{
    if (argc < 1 || !argv || !isValid())
        return false;
    QStringList args;
    for (int i = 1; i < argc; ++i)
        args << argv[i];
    return sendMessage(args);
}

bool BApplicationServer::sendMessage(const QStringList &arguments)
{
    B_D(BApplicationServer);
    if (d->server->isListening() || !isValid())
        return false;
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(BeQt::DataStreamVersion);
    out << true;
    out << arguments;
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    BGenericSocket s(BGenericSocket::TcpSocket);
    s.connectToHost("127.0.0.1", d->Port);
#else
    BGenericSocket s(BGenericSocket::LocalSocket);
    s.connectToHost(d->ServerName);
#endif
    if (!s.waitForConnected(d->OperationTimeout))
        return false;
    if (!s.write(ba))
        return false;
    return s.waitForBytesWritten(d->OperationTimeout);
}

/*============================== Protected methods =========================*/

void BApplicationServer::handleMessage(const QStringList &)
{
    //
}
