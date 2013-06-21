#ifndef BNETWORKSERVER_P_H
#define BNETWORKSERVER_P_H

class BNetworkConnection;
class BNetworkServerWorker;
class BNetworkServerThread;
class BNetworkServerPrivate;
class BNetworkServer;
class BGenericSocket;
class BSpamNotifier;

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
public Q_SLOTS:
    void addConnection(int socketDescriptor);
    void disconnected();
Q_SIGNALS:
    void connectionAdded(QObject *obj);
    void disconnected(QObject *obj);
public:
    BNetworkServerPrivate *const ServerPrivate;
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
public Q_SLOTS:
    void connectionAdded(QObject *obj);
    void disconnected(QObject *obj);
public:
    BNetworkServerWorker *const Worker;
public:
    QList<BNetworkConnection *> connections;
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
    BNetworkConnection *createConnection(BGenericSocket *socket);
    BGenericSocket *createSocket();
    BNetworkServerThread *getOptimalThread();
    int connectionCount() const;
    void emitConnectionAdded(BNetworkConnection *connection);
    void emitConnectionAboutToBeRemoved(BNetworkConnection *connection);
public Q_SLOTS:
    void newConnection(int socketDescriptor);
    void finished();
    void spammed();
public:
    QPointer<BGenericServer> server;
    QList<BNetworkServerThread *> threads;
    int maxConnectionCount;
    int maxThreadCount;
    BSpamNotifier *spamNotifier;
    mutable QMutex connectionMutex;
private:
    Q_DISABLE_COPY(BNetworkServerPrivate)
    friend class BNetworkServerWorkerPrivate;
};

#endif // BNETWORKSERVER_P_H
