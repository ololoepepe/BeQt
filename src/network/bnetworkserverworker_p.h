#ifndef BNETWORKSERVERWORKER_P_H
#define BNETWORKSERVERWORKER_P_H

class BNetworkServerWorkerPrivate;
class BNetworkServerWorker;
class BNetworkServerPrivate;
class BNetworkConnection;

#include "bnetworkserverworker.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QList>
#include <QMutex>

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

#endif // BNETWORKSERVERWORKER_P_H
