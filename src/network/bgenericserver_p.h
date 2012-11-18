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
#include <QTcpServer>
#include <QLocalServer>

/*============================================================================
================================ Local Server
============================================================================*/

class B_NETWORK_EXPORT BLocalServer : public QLocalServer
{
    Q_OBJECT
public:
    explicit BLocalServer(QObject *parent = 0);
signals:
    void newConnection(int socketDescriptor);
protected:
    void incomingConnection(quintptr socketDescriptor);
};

/*============================================================================
================================ Tcp Server
============================================================================*/

class B_NETWORK_EXPORT BTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit BTcpServer(QObject *parent = 0);
signals:
    void newConnection(int socketDescriptor);
protected:
    void incomingConnection(int handle);
};

/*============================================================================
================================ Generic Server Private Object
============================================================================*/

class B_NETWORK_EXPORT BGenericServerPrivateObject : public BBasePrivateObject
{
    B_DECLARE_PRIVATE_O(BGenericServer)
    Q_OBJECT
public:
    explicit BGenericServerPrivateObject(BGenericServerPrivate *p);
    ~BGenericServerPrivateObject();
public slots:
    void newConnection(int socketDescriptor);
private:
    Q_DISABLE_COPY(BGenericServerPrivateObject)
};

/*============================================================================
================================ Generic Server Private
============================================================================*/

class B_NETWORK_EXPORT BGenericServerPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BGenericServer)
    B_DECLARE_OBJECT(BGenericServer)
public:
    explicit BGenericServerPrivate(BGenericServer *q, BGenericServer::ServerType type);
    ~BGenericServerPrivate();
    //
    void newConnection(int socketDescriptor);
    //
    QPointer<QTcpServer> tserver;
    QPointer<QLocalServer> lserver;
    QQueue<BGenericSocket *> socketQueue;
    int maxPending;
protected:
    BGenericServerPrivate(BGenericServer &q, BGenericServerPrivateObject &o);
private:
    Q_DISABLE_COPY(BGenericServerPrivate)
};

#endif // BGENERICSERVER_P_H
