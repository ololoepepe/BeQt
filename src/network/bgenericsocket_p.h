#ifndef BGENERICSOCKET_P_H
#define BGENERICSOCKET_P_H

class BGenericSocketPrivate;

class QLocalSocket;

#include "bgenericsocket.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QLocalSocket>
#include <QPointer>
#include <QAbstractSocket>

class B_NETWORK_EXPORT BGenericSocketPrivateObject : public BBasePrivateObject
{
    B_DECLARE_PRIVATE_O(BGenericSocket)
    Q_OBJECT
public:
    explicit BGenericSocketPrivateObject(BGenericSocketPrivate *p);
    ~BGenericSocketPrivateObject();
public slots:
    void lsocketError(QLocalSocket::LocalSocketError socketError);
    void lsocketStateChanged(QLocalSocket::LocalSocketState socketState);
    void aboutToClose();
    void connected();
    void bytesWritten(qint64 bytes);
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void readChannelFinished();
    void readyRead();
    void stateChanged(QAbstractSocket::SocketState socketState);
private:
    Q_DISABLE_COPY(BGenericSocketPrivateObject)
};

class B_NETWORK_EXPORT BGenericSocketPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BGenericSocket)
    B_DECLARE_OBJECT(BGenericSocket)
public:
    explicit BGenericSocketPrivate(BGenericSocket *q, BGenericSocket::SocketType type);
    ~BGenericSocketPrivate();
    //
    void setSocket(QAbstractSocket *socket);
    void setSocket(QLocalSocket *socket);
    void connectIODevice();
    void disconnectIODevice();
    void lsocketError(QLocalSocket::LocalSocketError socketError);
    void lsocketStateChanged(QLocalSocket::LocalSocketState socketState);
    void aboutToClose();
    void connected();
    void bytesWritten(qint64 bytes);
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void readChannelFinished();
    void readyRead();
    void stateChanged(QAbstractSocket::SocketState socketState);
    //
    QPointer<QAbstractSocket> asocket;
    QPointer<QLocalSocket> lsocket;
protected:
    BGenericSocketPrivate(BGenericSocket &q, BGenericSocketPrivateObject &o);
private:
    Q_DISABLE_COPY(BGenericSocketPrivate)
};

#endif // BGENERICSOCKET_P_H
