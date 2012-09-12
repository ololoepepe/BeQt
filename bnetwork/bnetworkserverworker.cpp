#include "bnetworkserverworker.h"
#include "bnetworkconnection.h"
#include "bgenericsocket.h"

#include <QObject>
#include <QList>
#include <QMutex>
#include <QMutexLocker>

BNetworkServerWorker::BNetworkServerWorker(QObject *parent) :
    QObject(parent)
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
    BNetworkConnection *connection = createConnection(socketDescriptor);
    if ( !connection || !connection->isValid() )
        return;
    QMutexLocker locker(&_m_connectionsMutex);
    _m_connections << connection;
}

//

BNetworkConnection *BNetworkServerWorker::createConnection(int socketDescriptor)
{
    BGenericSocket *socket = new BGenericSocket(BGenericSocket::TcpSocket);
    if ( !socket->setSocketDescriptor(socketDescriptor) || !socket->isValid() )
    {
        socket->deleteLater();
        return 0;
    }
    return new BNetworkConnection(socket);
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
