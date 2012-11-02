#include "bnetworkserverworker.h"
#include "bnetworkconnection.h"

#include "bgenericsocket.h"

#include "bnetworkserver_p.h"

#include <QObject>
#include <QList>
#include <QMutex>
#include <QMutexLocker>

BNetworkServerWorker::BNetworkServerWorker(BNetworkServerPrivate *serverPrivate, QObject *parent) :
    QObject(parent), _m_ServerPrivate(serverPrivate)
{
}

//

int BNetworkServerWorker::connectionCount() const
{
    QMutexLocker locker(&_m_connectionsMutex);
    return _m_connections.size();
}

//

void BNetworkServerWorker::addConnection(int socketDescriptor)
{
    BNetworkConnection *connection = _m_ServerPrivate->createConnection(socketDescriptor);
    if ( !connection || !connection->isValid() )
        return;
    QMutexLocker locker(&_m_connectionsMutex);
    _m_connections << connection;
}

//

void BNetworkServerWorker::_m_disconnected()
{
    BNetworkConnection *connection = qobject_cast<BNetworkConnection *>( sender() );
    if (!connection)
        return;
    QMutexLocker locker(&_m_connectionsMutex);
    _m_connections.removeAll(connection);
    connection->deleteLater();
    if ( _m_connections.isEmpty() )
        emit ranOutOfConnections();
}
