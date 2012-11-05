#include "bnetworkserverthread.h"
#include "bnetworkserverworker.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QThread>
#include <QMetaObject>

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

//

BNetworkServerThreadPrivate::BNetworkServerThreadPrivate(BNetworkServerThread *q,
                                                         BNetworkServerPrivate *serverPrivate) :
    BBasePrivate(q), worker( new BNetworkServerWorker(serverPrivate) )
{
    worker->moveToThread(q);
    QObject::connect(worker, SIGNAL( ranOutOfConnections() ), q, SLOT( quit() ), Qt::QueuedConnection);
}

BNetworkServerThreadPrivate::~BNetworkServerThreadPrivate()
{
    worker->deleteLater();
}

//

BNetworkServerThread::BNetworkServerThread(BNetworkServerPrivate *serverPrivate) :
    QThread(0), BBase( *new BNetworkServerThreadPrivate(this, serverPrivate) )
{
    //
}

//

void BNetworkServerThread::addConnection(int socketDescriptor)
{
    B_D(BNetworkServerThread);
    if (!d->worker)
        return;
    QMetaObject::invokeMethod( d->worker, "addConnection", Qt::QueuedConnection, Q_ARG(int, socketDescriptor) );
}

int BNetworkServerThread::connectionCount() const
{
    return !d_func()->worker->connectionCount();
}
