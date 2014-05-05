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

#ifndef BNETWORKCONNECTON_H
#define BNETWORKCONNECTON_H

class BNetworkConnectionPrivate;
class BNetworkOperation;
class BNetworkServer;

class QString;
class QUuid;
class QByteArray;
class QVariant;

#include "bgenericsocket.h"
#include "bsocketwrapper.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/BeQt>
#include <BeQtCore/BLogger>

#include <QObject>
#include <QByteArray>
#include <QAbstractSocket>

/*============================================================================
================================ BNetworkConnection ==========================
============================================================================*/

class B_NETWORK_EXPORT BNetworkConnection : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BNetworkConnection)
public:
    enum LoggingMode
    {
        NoLogging = 0,
        NormalLogging = 10,
        DetailedLogging = 100
    };
    enum StandardOperation
    {
        NoopOperation,
        WriteOperation,
        LogOperation
    };
public:
    typedef bool (BNetworkConnection::*InternalHandler)(BNetworkOperation *);
    typedef bool (*ExternalHandler)(BNetworkOperation *);
public:
    static QString operation(StandardOperation op);
    static InternalHandler replyHandler(StandardOperation op);
    static InternalHandler requestHandler(StandardOperation op);
public:
    explicit BNetworkConnection(BGenericSocket *socket, QObject *parent = 0);
    explicit BNetworkConnection(BNetworkServer *server, BGenericSocket *socket);
    explicit BNetworkConnection(BGenericSocket::SocketType type, QObject *parent = 0);
    ~BNetworkConnection();
protected:
    explicit BNetworkConnection(BNetworkConnectionPrivate &d, QObject *parent = 0);
public:
    void setCompressionLevel(int level);
    void setCriticalBufferSize(qint64 size);
    void setCloseOnCriticalBufferSize(bool close);
    void setLoggingMode(LoggingMode m);
    void setAutoDeleteSentReplies(bool enabled);
    void setLogger(BLogger *l);
    void setTranslationsEnabled(bool enabled);
    void connectToHost(const QString &hostName, quint16 port = 0);
    bool connectToHostBlocking(const QString &hostName, quint16 port = 0, int msecs = 30 * BeQt::Second);
    bool disconnectFromHostBlocking(int msecs = 30 * BeQt::Second);
    bool waitForConnected(int msecs = 30 * BeQt::Second);
    bool waitForDisconnected(int msecs = 30 * BeQt::Second);
    void installReplyHandler(const QString &operation, InternalHandler handler);
    void installReplyHandler(const QString &operation, ExternalHandler handler);
    void installReplyHandler(StandardOperation op);
    void installRequestHandler(const QString &operation, InternalHandler handler);
    void installRequestHandler(const QString &operation, ExternalHandler handler);
    void installRequestHandler(StandardOperation op);
    void installRequestHandler(StandardOperation op, InternalHandler handler);
    void installRequestHandler(StandardOperation op, ExternalHandler handler);
    bool isValid() const;
    bool isConnected() const;
    const QUuid uniqueId() const;
    BNetworkServer *server() const;
    QAbstractSocket::SocketError error() const;
    QString errorString() const;
    int compressionLevel() const;
    qint64 criticalBufferSize() const;
    bool closeOnCriticalBufferSize() const;
    LoggingMode loggingMode() const;
    bool autoDeleteSentReplies() const;
    BLogger *logger() const;
    bool translationsEnabled() const;
    QString peerAddress() const;
    BNetworkOperation *sendRequest(const QString &op, const QByteArray &data = QByteArray());
    BNetworkOperation *sendRequest(const QString &op, const QVariant &variant);
    bool sendReply(BNetworkOperation *op, const QByteArray &data = QByteArray());
    bool sendReply(BNetworkOperation *op, const QVariant &variant);
public Q_SLOTS:
    void disconnectFromHost();
    void close();
    void abort();
protected:
    virtual bool handleReply(BNetworkOperation *op);
    virtual bool handleRequest(BNetworkOperation *op);
    virtual void log(const QString &text, BLogger::Level lvl = BLogger::NoLevel);
    BGenericSocket *socket() const;
    BSocketWrapper *socketWrapper() const;
    bool handleNoopRequest(BNetworkOperation *op);
    bool handleWriteRequest(BNetworkOperation *op);
    bool handleLogRequest(BNetworkOperation *op);
Q_SIGNALS:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void requestSent(BNetworkOperation *operation);
    void replyReceived(BNetworkOperation *operation);
    void incomingRequest(BNetworkOperation *operation);
    void requestReceived(BNetworkOperation *operation);
    void replySent(BNetworkOperation *operation);
    void criticalBufferSizeReached();
private:
    Q_DISABLE_COPY(BNetworkConnection)
};

#endif // BNETWORKCONNECTON_H
