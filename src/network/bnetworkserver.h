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
class BNetworkConnection;
class BGenericSocket;
class BSpamNotifier;

class QString;
class QStringList;

#include "bgenericserver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBaseObject>

#include <QObject>
#include <QList>

/*============================================================================
================================ BNetworkServer ==============================
============================================================================*/

class B_NETWORK_EXPORT BNetworkServer : public QObject, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BNetworkServer)
public:
    explicit BNetworkServer(BGenericServer::ServerType type, QObject *parent = 0);
    ~BNetworkServer();
protected:
    explicit BNetworkServer(BNetworkServerPrivate &d, QObject *parent = 0);
public:
    void setMaxConnectionCount(int count);
    void setMaxThreadCount(int count);
    void lock();
    void unlock();
    bool tryLock();
    bool isValid() const;
    bool isListening() const;
    bool listen(const QString &address, quint16 port = 0);
    BGenericServer::ServerType serverType() const;
    int maxConnectionCount() const;
    int currentConnectionCount() const;
    int maxThreadCount() const;
    int currentThreadCount() const;
    QList<BNetworkConnection *> connections() const;
    BSpamNotifier *spamNotifier() const;
    QStringList banned() const;
public Q_SLOTS:
    void close();
    void ban(const QString &address);
    void ban(const QStringList &addresses);
    void unban(const QString &address);
    void unban(const QStringList &addresses);
    void clearBanList();
protected:
    virtual BNetworkConnection *createConnection(BGenericSocket *socket);
    virtual BGenericSocket *createSocket();
Q_SIGNALS:
    void connectionAdded(BNetworkConnection *connection);
    void connectionAboutToBeRemoved(BNetworkConnection *connection);
private:
    Q_DISABLE_COPY(BNetworkServer)
};

#endif // BNETWORKSERVER_H
