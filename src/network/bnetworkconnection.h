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
class BSocketWrapper;
class BUuid;

class QString;
class QVariant;

#include "bgenericsocket.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>
#include <BeQtCore/BLogger>

#include <QAbstractSocket>
#include <QByteArray>
#include <QObject>

/*============================================================================
================================ BNetworkConnection ==========================
============================================================================*/

class B_NETWORK_EXPORT BNetworkConnection : public QObject, public BBaseObject
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
        NoopOperation = 1,
        WriteOperation,
        LogOperation
    };
public:
    typedef bool (BNetworkConnection::*InternalHandler)(BNetworkOperation *);
    typedef bool (*ExternalHandler)(BNetworkOperation *);
public:
    explicit BNetworkConnection(BGenericSocket *socket, QObject *parent = 0);
    explicit BNetworkConnection(BNetworkServer *server, BGenericSocket *socket);
    explicit BNetworkConnection(BGenericSocket::SocketType type, QObject *parent = 0);
    ~BNetworkConnection();
protected:
    explicit BNetworkConnection(BNetworkConnectionPrivate &d, QObject *parent = 0);
public:
    static QString operation(StandardOperation op);
    static InternalHandler replyHandler(StandardOperation op);
    static InternalHandler requestHandler(StandardOperation op);
public:
    bool autoDeleteSentReplies() const;
    bool closeOnCriticalBufferSize() const;
    int compressionLevel() const;
    void connectToHost(const QString &hostName, quint16 port = 0);
    bool connectToHostBlocking(const QString &hostName, quint16 port = 0, int msecs = 30 * BeQt::Second);
    qint64 criticalBufferSize() const;
    bool disconnectFromHostBlocking(int msecs = 30 * BeQt::Second);
    QAbstractSocket::SocketError error() const;
    QString errorString() const;
    void installReplyHandler(const QString &operation, InternalHandler handler);
    void installReplyHandler(const QString &operation, ExternalHandler handler);
    void installReplyHandler(StandardOperation op);
    void installRequestHandler(const QString &operation, InternalHandler handler);
    void installRequestHandler(const QString &operation, ExternalHandler handler);
    void installRequestHandler(StandardOperation op);
    void installRequestHandler(StandardOperation op, InternalHandler handler);
    void installRequestHandler(StandardOperation op, ExternalHandler handler);
    bool isConnected() const;
    bool isValid() const;
    BLogger *logger() const;
    LoggingMode loggingMode() const;
    QString peerAddress() const;
    bool sendReply(BNetworkOperation *op, const QByteArray &data = QByteArray());
    bool sendReply(BNetworkOperation *op, const QVariant &variant);
    bool sendReply(BNetworkOperation *op, int compressionLevel, const QByteArray &data = QByteArray());
    bool sendReply(BNetworkOperation *op, int compressionLevel, const QVariant &variant);
    BNetworkOperation *sendRequest(const QString &op, const QByteArray &data = QByteArray());
    BNetworkOperation *sendRequest(const QString &op, const QVariant &variant);
    BNetworkOperation *sendRequest(const QString &op, int compressionLevel, const QByteArray &data = QByteArray());
    BNetworkOperation *sendRequest(const QString &op, int compressionLevel, const QVariant &variant);
    BNetworkServer *server() const;
    void setAutoDeleteSentReplies(bool enabled);
    void setCloseOnCriticalBufferSize(bool close);
    void setCompressionLevel(int level);
    void setCriticalBufferSize(qint64 size);
    void setLogger(BLogger *l);
    void setLoggingMode(LoggingMode m);
    void setTranslationsEnabled(bool enabled);
    bool translationsEnabled() const;
    BUuid uniqueId() const;
    bool waitForConnected(int msecs = 30 * BeQt::Second);
    bool waitForDisconnected(int msecs = 30 * BeQt::Second);
public Q_SLOTS:
    void abort();
    void close();
    void disconnectFromHost();
protected:
    bool handleNoopRequest(BNetworkOperation *op);
    bool handleLogRequest(BNetworkOperation *op);
    virtual bool handleReply(BNetworkOperation *op);
    virtual bool handleRequest(BNetworkOperation *op);
    bool handleWriteRequest(BNetworkOperation *op);
    virtual void log(const QString &text, BLogger::Level lvl = BLogger::NoLevel);
    BGenericSocket *socket() const;
    BSocketWrapper *socketWrapper() const;
Q_SIGNALS:
    void connected();
    void criticalBufferSizeReached();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void incomingRequest(BNetworkOperation *operation);
    void replyReceived(BNetworkOperation *operation);
    void replySent(BNetworkOperation *operation);
    void requestReceived(BNetworkOperation *operation);
    void requestSent(BNetworkOperation *operation);
private:
    Q_DISABLE_COPY(BNetworkConnection)
};

#endif // BNETWORKCONNECTON_H
