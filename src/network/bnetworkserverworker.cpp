#include "bnetworkserverworker.h"
#include "bnetworkconnection.h"
#include "bgenericsocket.h"
#include "bnetworkserver_p.h"
#include "bnetworkserverworker_p.h"

#include <QObject>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QMetaObject>

BNetworkServerWorkerPrivateObject::BNetworkServerWorkerPrivateObject(BNetworkServerWorkerPrivate *p) :
    _m_p(p)
{
    //
}

BNetworkServerWorkerPrivateObject::~BNetworkServerWorkerPrivateObject()
{
    //
}

//

void BNetworkServerWorkerPrivateObject::disconnected()
{
    _m_p->disconnected( static_cast<BNetworkConnection *>( sender() ) );
}

//

BNetworkServerWorkerPrivate::BNetworkServerWorkerPrivate(BNetworkServerWorker *q, BNetworkServerPrivate *sp) :
    BBasePrivate(q), _m_o( new BNetworkServerWorkerPrivateObject(this) ), serverPrivate(sp)
{
    //
}

BNetworkServerWorkerPrivate::~BNetworkServerWorkerPrivate()
{
    _m_o->deleteLater();
}

//

void BNetworkServerWorkerPrivate::disconnected(BNetworkConnection *connection)
{
    if (!connection)
        return;
    QMutexLocker locker(&connectionsMutex);
    connections.removeAll(connection);
    connection->deleteLater();
    if ( connections.isEmpty() )
        QMetaObject::invokeMethod(q_func(), "ranOutOfConnections");
}

//

BNetworkServerWorker::BNetworkServerWorker(BNetworkServerPrivate *serverPrivate) :
    QObject(0), BBase( *new BNetworkServerWorkerPrivate(this, serverPrivate) )
{
    //
}

BNetworkServerWorker::~BNetworkServerWorker()
{
    //
}

//

int BNetworkServerWorker::connectionCount() const
{
    const B_D(BNetworkServerWorker);
    QMutexLocker locker(&d->connectionsMutex);
    return d->connections.size();
}

//

void BNetworkServerWorker::addConnection(int socketDescriptor)
{
    B_D(BNetworkServerWorker);
    BNetworkConnection *connection = d->serverPrivate->createConnection(socketDescriptor);
    if ( !connection || !connection->isValid() )
        return;
    connect( connection, SIGNAL( disconnected() ), d->_m_o, SLOT( disconnected() ) );
    QMutexLocker locker(&d->connectionsMutex);
    d->connections << connection;
}

//

BNetworkServerWorker::BNetworkServerWorker(BNetworkServerWorkerPrivate &d) :
    BBase(d)
{
    //
}
