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

#ifndef BREMOTELOGGER_H
#define BREMOTELOGGER_H

class BRemoteLoggerPrivate;

class BGenericSocket;
class BNetworkConnection;
class BNetworkServer;

class QString;

#include <BeQtCore/BLogger>

#include <QList>
#include <QObject>

/*============================================================================
================================ BRemoteLogger ===============================
============================================================================*/

class B_NETWORK_EXPORT BRemoteLogger : public BLogger
{
    Q_OBJECT
    Q_PROPERTY(QString hostName READ hostName STORED false)
    Q_PROPERTY(bool logToRemoteEnabled READ isLogToRemoteEnabled WRITE setLogToRemoteEnabled)
    Q_PROPERTY(quint16 port READ port STORED false)
    Q_PROPERTY(int remoteTimeout READ remoteTimeout WRITE setRemoteTimeout)
    B_DECLARE_PRIVATE(BRemoteLogger)
public:
    explicit BRemoteLogger(QObject *parent = 0);
    explicit BRemoteLogger(BGenericSocket *socket, QObject *parent = 0);
    explicit BRemoteLogger(const QString &hostName, quint16 port, QObject *parent = 0);
    explicit BRemoteLogger(BNetworkConnection *c, QObject *parent = 0);
    explicit BRemoteLogger(const QList<BNetworkConnection *> &list, QObject *parent = 0);
    explicit BRemoteLogger(BNetworkServer *server, QObject *parent = 0);
    ~BRemoteLogger();
protected:
    explicit BRemoteLogger(BRemoteLoggerPrivate &d);
public:
    QList<BNetworkConnection *> connections() const;
    QString hostName() const;
    bool isLogToRemoteEnabled() const;
    quint16 port() const;
    int remoteTimeout() const;
    BNetworkServer *server() const;
    void setLogToRemoteEnabled(bool enabled);
    void setRemote(BGenericSocket *socket);
    void setRemote(const QString &hostName, quint16 port);
    void setRemote(BNetworkConnection *c);
    void setRemote(const QList<BNetworkConnection *> &list);
    void setRemote(BNetworkServer *server);
    void setRemoteTimeout(int msecs);
    BGenericSocket *socket() const;
protected:
    void userLog(const QString &text, Level level);
private:
    Q_DISABLE_COPY(BRemoteLogger)
};

#endif // BREMOTELOGGER_H
