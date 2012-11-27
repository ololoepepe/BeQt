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
================================ BNetworkServerWorker ========================
============================================================================*/

class B_NETWORK_EXPORT BNetworkServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit BNetworkServerWorker(BNetworkServerPrivate *sp);
    ~BNetworkServerWorker();
public:
    int connectionCount() const;
public slots:
    void addConnection(int socketDescriptor);
    void disconnected();
signals:
    void ranOutOfConnections();
public:
    BNetworkServerPrivate *const serverPrivate;
public:
    mutable QMutex connectionsMutex;
    QList<BNetworkConnection *> connections;
private:
    Q_DISABLE_COPY(BNetworkServerWorker)
};

/*============================================================================
================================ BNetworkServerThread ========================
============================================================================*/

class B_NETWORK_EXPORT BNetworkServerThread : public QThread
{
    Q_OBJECT
public:
    explicit BNetworkServerThread(BNetworkServerPrivate *serverPrivate);
    ~BNetworkServerThread();
public:
    void addConnection(int socketDescriptor);
    int connectionCount() const;
public:
    BNetworkServerWorker *const worker;
private:
    Q_DISABLE_COPY(BNetworkServerThread)
};

/*============================================================================
================================ BNetworkServerPrivate =======================
============================================================================*/

class B_NETWORK_EXPORT BNetworkServerPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BNetworkServer)
    Q_OBJECT
public:
    explicit BNetworkServerPrivate(BNetworkServer *q);
    ~BNetworkServerPrivate();
public:
    void init();
    BNetworkConnection *createConnection(int socketDescriptor) const;
public slots:
    void newConnection(int socketDescriptor);
    void finished();
public:
    QPointer<BGenericServer> server;
    QList<BNetworkServerThread *> threads;
    int maxConnectionCount;
    int maxThreadCount;
private:
    Q_DISABLE_COPY(BNetworkServerPrivate)
    friend class BNetworkServerWorkerPrivate;
};

#endif // BNETWORKSERVER_P_H
