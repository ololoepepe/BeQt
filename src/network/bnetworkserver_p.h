#ifndef BNETWORKSERVER_P_H
#define BNETWORKSERVER_P_H

class BNetworkServerPrivate;
class BNetworkServer;
class BNetworkServerThread;
class BNetworkServerWorker;
class BNetworkConnection;
class BNetworkServerWorkerPrivate;

#include "bnetworkserver.h"
#include "bgenericserver.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QList>
#include <QPointer>
#include <QString>

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
