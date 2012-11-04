#ifndef BGENERICSOCKET_P_H
#define BGENERICSOCKET_P_H

class BGenericSocketPrivate;

class QLocalSocket;
class QAbstractSokcet;

#include "bgenericsocket.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QLocalSocket>
#include <QPointer>

class BGenericSocketPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BGenericSocketPrivateObject(BGenericSocketPrivate *p);
    ~BGenericSocketPrivateObject();
    //
    BGenericSocketPrivate *const _m_p;
public slots:
    void lsocketError(QLocalSocket::LocalSocketError socketError);
    void lsocketStateChanged(QLocalSocket::LocalSocketState socketState);
private:
    Q_DISABLE_COPY(BGenericSocketPrivateObject)
};

class BGenericSocketPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BGenericSocket)
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
    //
    BGenericSocketPrivateObject *const _m_o;
    //
    QPointer<QAbstractSocket> asocket;
    QPointer<QLocalSocket> lsocket;
private:
    Q_DISABLE_COPY(BGenericSocketPrivate)
};

#endif // BGENERICSOCKET_P_H
