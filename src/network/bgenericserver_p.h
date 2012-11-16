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
