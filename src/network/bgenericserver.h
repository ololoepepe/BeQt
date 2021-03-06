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

#ifndef BGENERICSERVER_H
#define BGENERICSERVER_H

class BGenericServerPrivate;

class BGenericSocket;

class QTcpServer;
class QLocalServer;
class QString;

#include <BeQtCore/BBaseObject>

#include <QAbstractSocket>
#include <QObject>

/*============================================================================
================================ BGenericServer ==============================
============================================================================*/

class B_NETWORK_EXPORT BGenericServer : public QObject, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BGenericServer)
public:
    enum ServerType
    {
        NoServer = 0x00,    //0 0 0 0 0 0 0 0
        TcpServer = 0x03,   //0 0 0 0 0 0 1 1
        SslServer = 0x07,   //0 0 0 0 0 1 1 1
        LocalServer = 0x10  //0 0 0 1 0 0 0 0
    };
public:
    explicit BGenericServer(ServerType type, QObject *parent = 0);
    ~BGenericServer();
protected:
    explicit BGenericServer(BGenericServerPrivate &d, QObject *parent = 0);
public:
    void close();
    QString errorString() const;
    bool hasPendingConnections() const;
    bool isListening() const;
    bool isServerSet() const;
    bool listen(const QString &address, quint16 port = 0);
    QLocalServer *localServer() const;
    int maxPendingConnections() const;
    BGenericSocket *nextPendingConnection();
    QString serverAddress() const;
    QAbstractSocket::SocketError serverError() const;
    ServerType serverType() const;
    void setMaxPendingConnections(int numConnections);
    QTcpServer *tcpServer() const;
    bool waitForNewConnection(int msec = 0, bool *timedOut = 0);
protected:
    virtual BGenericSocket *createSocket(int socketDescriptor);
Q_SIGNALS:
    void newPendingConnection();
    void newConnection(int socketDescriptor);
    void connectionOverflow();
private:
    Q_DISABLE_COPY(BGenericServer)
};

#endif // BGENERICSERVER_H
