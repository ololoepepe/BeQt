#ifndef BGENERICSERVER_P_H
#define BGENERICSERVER_P_H

class BGenericServerPrivate;
class BGenericSocket;

class QTcpServer;
class QLocalServer;

#include "bgenericserver.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QQueue>
#include <QPointer>

class B_NETWORK_EXPORT BGenericServerPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BGenericServerPrivateObject(BGenericServerPrivate *p);
    ~BGenericServerPrivateObject();
    //
    BGenericServerPrivate *const _m_p;
public slots:
    void newConnection(int socketDescriptor);
private:
    Q_DISABLE_COPY(BGenericServerPrivateObject)
};

class B_NETWORK_EXPORT BGenericServerPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BGenericServer)
public:
    explicit BGenericServerPrivate(BGenericServer *q, BGenericServer::ServerType type);
    ~BGenericServerPrivate();
    //
    void newConnection(int socketDescriptor);
    //
    BGenericServerPrivateObject *const _m_o;
    //
    QPointer<QTcpServer> tserver;
    QPointer<QLocalServer> lserver;
    QQueue<BGenericSocket *> socketQueue;
    int maxPending;
private:
    Q_DISABLE_COPY(BGenericServerPrivate)
};

#endif // BGENERICSERVER_P_H
