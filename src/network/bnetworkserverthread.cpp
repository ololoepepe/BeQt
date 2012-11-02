#include "bnetworkserverthread.h"
#include "bnetworkserverworker.h"

#include <QThread>
#include <QMetaObject>

BNetworkServerThread::BNetworkServerThread(BNetworkServerWorker *worker, QObject *parent) :
    QThread(parent)
{
    if (worker)
    {
        worker->setParent(0);
        worker->moveToThread(this);
        connect(worker, SIGNAL( ranOutOfConnections() ), this, SLOT( quit() ), Qt::QueuedConnection);
        _m_worker = worker;
    }
}

//

void BNetworkServerThread::addConnection(int socketDescriptor)
{
    if (!_m_worker)
        return;
    QMetaObject::invokeMethod( _m_worker, "addConnection", Qt::QueuedConnection, Q_ARG(int, socketDescriptor) );
}

int BNetworkServerThread::connectionCount() const
{
    return !_m_worker.isNull() ? _m_worker->connectionCount() : 0;
}

bool BNetworkServerThread::isValid() const
{
    return !_m_worker.isNull();
}
