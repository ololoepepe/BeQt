#ifndef BNETWORKSERVER_P_H
#define BNETWORKSERVER_P_H

class BNetworkConnection;
class BNetworkServerWorkerPrivate;
class BNetworkServerWorker;
class BNetworkServerThreadPrivate;
class BNetworkServerThread;
class BNetworkServerPrivate;
class BNetworkServer;

#include "bnetworkserver.h"
#include "bgenericserver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QList>
#include <QPointer>
#include <QString>
#include <QThread>
#include <QMutex>

/*============================================================================
================================ Network Server Worker
============================================================================*/

class B_NETWORK_EXPORT BNetworkServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit BNetworkServerWorker(BNetworkServerPrivate *sp);
    ~BNetworkServerWorker();
    //
    int connectionCount() const;
public slots:
    void addConnection(int socketDescriptor);
signals:
    void ranOutOfConnections();
protected:
    BNetworkServerWorker(BNetworkServerWorkerPrivate &d);
private:
    Q_DISABLE_COPY(BNetworkServerWorker)
    //
    BNetworkServerPrivate *const serverPrivate;
    //
    mutable QMutex connectionsMutex;
    QList<BNetworkConnection *> connections;
private slots:
    void disconnected();
};

/*============================================================================
================================ Network Server Thread
============================================================================*/

class B_NETWORK_EXPORT BNetworkServerThread : public QThread
{
    Q_OBJECT
public:
    explicit BNetworkServerThread(BNetworkServerPrivate *serverPrivate);
    ~BNetworkServerThread();
    //
    void addConnection(int socketDescriptor);
    int connectionCount() const;
protected:
    BNetworkServerThread(BNetworkServerThreadPrivate &d);
private:
    Q_DISABLE_COPY(BNetworkServerThread)
    //
    BNetworkServerWorker *const worker;
};

/*============================================================================
================================ Network Server Private
============================================================================*/

class B_NETWORK_EXPORT BNetworkServerPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BNetworkServer)
    Q_OBJECT
public:
    explicit BNetworkServerPrivate(BNetworkServer *q);
    ~BNetworkServerPrivate();
    //
    void init();
    BNetworkConnection *createConnection(int socketDescriptor) const;
    //
    QPointer<BGenericServer> server;
    QList<BNetworkServerThread *> threads;
    int maxConnectionCount;
    int maxThreadCount;
public slots:
    void newConnection(int socketDescriptor);
    void finished();
private:
    Q_DISABLE_COPY(BNetworkServerPrivate)
    //
    friend class BNetworkServerWorkerPrivate;
};

#endif // BNETWORKSERVER_P_H
