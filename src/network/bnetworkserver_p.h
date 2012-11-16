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

class BNetworkServerWorker : public QObject, public BBase
{
    B_DECLARE_PRIVATE(BNetworkServerWorker)
    Q_OBJECT
public:
    explicit BNetworkServerWorker(BNetworkServerPrivate *serverPrivate);
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
};

class B_NETWORK_EXPORT BNetworkServerWorkerPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BNetworkServerWorkerPrivateObject(BNetworkServerWorkerPrivate *p);
    ~BNetworkServerWorkerPrivateObject();
    //
    BNetworkServerWorkerPrivate *const _m_p;
public slots:
    void disconnected();
private:
    Q_DISABLE_COPY(BNetworkServerWorkerPrivateObject)
};

class B_NETWORK_EXPORT BNetworkServerWorkerPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BNetworkServerWorker)
public:
    explicit BNetworkServerWorkerPrivate(BNetworkServerWorker *q, BNetworkServerPrivate *sp);
    ~BNetworkServerWorkerPrivate();
    //
    void disconnected(BNetworkConnection *connection);
    //
    BNetworkServerWorkerPrivateObject *const _m_o;
    BNetworkServerPrivate *const serverPrivate;
    //
    mutable QMutex connectionsMutex;
    QList<BNetworkConnection *> connections;
private:
    Q_DISABLE_COPY(BNetworkServerWorkerPrivate)
};

class BNetworkServerThread : public QThread, public BBase
{
    B_DECLARE_PRIVATE(BNetworkServerThread)
    Q_OBJECT
public:
    explicit BNetworkServerThread(BNetworkServerPrivate *serverPrivate);
    //
    void addConnection(int socketDescriptor);
    int connectionCount() const;
protected:
    BNetworkServerThread(BNetworkServerThreadPrivate &d);
private:
    Q_DISABLE_COPY(BNetworkServerThread)
};

class B_NETWORK_EXPORT BNetworkServerPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BNetworkServerPrivateObject(BNetworkServerPrivate *p);
    ~BNetworkServerPrivateObject();
    //
    BNetworkServerPrivate *const _m_p;
public slots:
    void newConnection(int socketDescriptor);
    void finished();
private:
    Q_DISABLE_COPY(BNetworkServerPrivateObject)
};

class BNetworkServerThreadPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BNetworkServerThread)
public:
    explicit BNetworkServerThreadPrivate(BNetworkServerThread *q, BNetworkServerPrivate *serverPrivate);
    ~BNetworkServerThreadPrivate();
    //
    BNetworkServerWorker *const worker;
private:
    Q_DISABLE_COPY(BNetworkServerThreadPrivate)
};

class B_NETWORK_EXPORT BNetworkServerPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BNetworkServer)
public:
    explicit BNetworkServerPrivate(BNetworkServer *q, BGenericServer::ServerType type);
    ~BNetworkServerPrivate();
    //
    void newConnection(int socketDescriptor);
    void finished(BNetworkServerThread *t);
    BNetworkConnection *createConnection(int socketDescriptor) const;
    //
    BNetworkServerPrivateObject *const _m_o;
    //
    QPointer<BGenericServer> server;
    QList<BNetworkServerThread *> threads;
    int maxConnectionCount;
    int maxThreadCount;
private:
    Q_DISABLE_COPY(BNetworkServerPrivate)
    //
    friend class BNetworkServerWorkerPrivate;
};

#endif // BNETWORKSERVER_P_H
