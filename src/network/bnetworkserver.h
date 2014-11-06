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

#ifndef BNETWORKSERVER_H
#define BNETWORKSERVER_H

class BNetworkServerPrivate;

class BGenericSocket;
class BNetworkConnection;

class QString;
class QStringList;

#include "bgenericserver.h"

#include <BeQtCore/BBaseObject>

#include <QList>
#include <QObject>

/*============================================================================
================================ BNetworkServer ==============================
============================================================================*/

class B_NETWORK_EXPORT BNetworkServer : public QObject, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BNetworkServer)
public:
    enum SslHandlingMode
    {
        DoNotEncript = 0,
        EncriptBlocking = 1,
        EncriptNonBlocking
    };
public:
    explicit BNetworkServer(BGenericServer::ServerType type, QObject *parent = 0);
    ~BNetworkServer();
protected:
    explicit BNetworkServer(BNetworkServerPrivate &d, QObject *parent = 0);
public:
    QStringList banList() const;
    QList<BNetworkConnection *> connections() const;
    int currentConnectionCount() const;
    int currentThreadCount() const;
    bool isListening() const;
    bool isValid() const;
    bool listen(const QString &address);
    bool listen(const QString &address, quint16 port);
    int listen(const QStringList &addresses);
    void lock();
    int maxConnectionCount() const;
    int maxThreadCount() const;
    BGenericServer::ServerType serverType() const;
    void setMaxConnectionCount(int count);
    void setMaxThreadCount(int count);
    void setSslEncriptionWaitTimeout(int msecs);
    void setSslHandlingMode(SslHandlingMode mode);
    int sslEncriptionWaitTimeout() const;
    SslHandlingMode sslHandlingMode() const;
    void unlock();
    bool tryLock();
public Q_SLOTS:
    void ban(const QString &address);
    void ban(const QStringList &addresses);
    void clearBanList();
    void close();
    void setBanList(const QStringList &addresses);
    void unban(const QString &address);
    void unban(const QStringList &addresses);
protected:
    virtual BNetworkConnection *createConnection(BGenericSocket *socket, const QString &serverAddress,
                                                 quint16 serverPort);
    virtual BGenericSocket *createSocket();
    virtual bool handleBanned(BGenericSocket *socket);
    virtual bool handleIncomingConnection(BGenericSocket *socket);
    virtual bool isBanned(const QString &address) const;
Q_SIGNALS:
    void connectionAboutToBeRemoved(BNetworkConnection *connection);
    void connectionAdded(BNetworkConnection *connection);
    void bannedUserConnectionDenied(const QString &address);
private:
    Q_DISABLE_COPY(BNetworkServer)
};

#endif // BNETWORKSERVER_H
