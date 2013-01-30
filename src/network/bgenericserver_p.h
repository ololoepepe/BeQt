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
================================ BLocalServer ================================
============================================================================*/

class B_NETWORK_EXPORT BLocalServer : public QLocalServer
{
    Q_OBJECT
public:
    explicit BLocalServer(QObject *parent = 0);
    ~BLocalServer();
protected:
    void incomingConnection(quintptr socketDescriptor);
signals:
    void newConnection(int socketDescriptor);
};

/*============================================================================
================================ BTcpServer ==================================
============================================================================*/

class B_NETWORK_EXPORT BTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit BTcpServer(QObject *parent = 0);
    ~BTcpServer();
protected:
    void incomingConnection(int handle);
signals:
    void newConnection(int socketDescriptor);
};

/*============================================================================
================================ BGenericServerPrivate =======================
============================================================================*/

class B_NETWORK_EXPORT BGenericServerPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BGenericServer)
public:
    explicit BGenericServerPrivate(BGenericServer *q);
    ~BGenericServerPrivate();
public:
    void init();
public slots:
    void newConnection(int socketDescriptor);
public:
    QPointer<QTcpServer> tserver;
    QPointer<QLocalServer> lserver;
    QQueue<BGenericSocket *> socketQueue;
    int maxPending;
private:
    Q_DISABLE_COPY(BGenericServerPrivate)
};

#endif // BGENERICSERVER_P_H
