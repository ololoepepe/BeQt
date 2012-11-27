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

/*============================================================================
================================ BGenericSocketPrivate =======================
============================================================================*/

class B_NETWORK_EXPORT BGenericSocketPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BGenericSocket)

public:
    explicit BGenericSocketPrivate(BGenericSocket *q);
    ~BGenericSocketPrivate();
public:
    void init();
    void setSocket(QAbstractSocket *socket);
    void setSocket(QLocalSocket *socket);
    void connectIODevice();
    void disconnectIODevice();
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
public:
    QPointer<QAbstractSocket> asocket;
    QPointer<QLocalSocket> lsocket;
private:
    Q_DISABLE_COPY(BGenericSocketPrivate)
};

#endif // BGENERICSOCKET_P_H
