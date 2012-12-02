#include "bnetworkserver.h"
#include "bnetworkconnection.h"
#include "bnetworkserver_p.h"
#include "bgenericserver.h"

#include <QObject>
#include <QList>
#include <QThread>
#include <QString>
#include <QMetaObject>
#include <QMutex>
#include <QMutexLocker>

/*============================================================================
================================ BNetworkServerWorker ========================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkServerWorker::BNetworkServerWorker(BNetworkServerPrivate *sp) :
    QObject(0), serverPrivate(sp)
{
    //
}

BNetworkServerWorker::~BNetworkServerWorker()
{
    //
}

/*============================== Public methods ============================*/

int BNetworkServerWorker::connectionCount() const
{
    QMutexLocker locker(&connectionsMutex);
    return connections.size();
}

QList<BNetworkConnection *> BNetworkServerWorker::getConnections() const
{
    QMutexLocker locker(&connectionsMutex);
    return connections;
}

/*============================== Public slots ==============================*/

void BNetworkServerWorker::addConnection(int socketDescriptor)
{
    BGenericSocket *s = serverPrivate->createSocket();
    if ( !s || !s->setSocketDescriptor(socketDescriptor) || !s->isValid() )
        return;
    BNetworkConnection *connection = serverPrivate->createConnection(s);
    if ( !connection || !connection->isValid() )
        return;
    connect( connection, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
    QMutexLocker locker(&connectionsMutex);
    connections << connection;
}

void BNetworkServerWorker::disconnected()
{
    BNetworkConnection *c = static_cast<BNetworkConnection *>( sender() );
    if (!c)
        return;
    QMutexLocker locker(&connectionsMutex);
    connections.removeAll(c);
    c->deleteLater();
    if ( connections.isEmpty() )
        emit ranOutOfConnections();
}

/*============================================================================
================================ BNetworkServerThread ========================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkServerThread::BNetworkServerThread(BNetworkServerPrivate *serverPrivate) :
    QThread(0), worker( new BNetworkServerWorker(serverPrivate) )
{
    worker->moveToThread(this);
    QObject::connect(worker, SIGNAL( ranOutOfConnections() ), this, SLOT( quit() ), Qt::QueuedConnection);
}

BNetworkServerThread::~BNetworkServerThread()
{
    worker->deleteLater();
}

/*============================== Public methods ============================*/

void BNetworkServerThread::addConnection(int socketDescriptor)
{
    QMetaObject::invokeMethod( worker, "addConnection", Qt::QueuedConnection, Q_ARG(int, socketDescriptor) );
}

int BNetworkServerThread::connectionCount() const
{
    return worker->connectionCount();
}

/*============================================================================
================================ BNetworkServerPrivate =======================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkServerPrivate::BNetworkServerPrivate(BNetworkServer *q) :
    BBasePrivate(q)
{
    //
}

BNetworkServerPrivate::~BNetworkServerPrivate()
{
    foreach (BNetworkServerThread *t, threads)
    {
        if (!t)
            continue;
        disconnect( t, SIGNAL( finished() ), this, SLOT( finished() ) );
        t->quit();
        t->wait(1000);
    }
}

/*============================== Public methods ============================*/

void BNetworkServerPrivate::init()
{
    maxConnectionCount = 0;
    maxThreadCount = 0;
}

BNetworkConnection *BNetworkServerPrivate::createConnection(BGenericSocket *socket) const
{
    return q_func()->createConnection(socket);
}

BGenericSocket *BNetworkServerPrivate::createSocket() const
{
    return q_func()->createSocket();
}

/*============================== Public slots ==============================*/

void BNetworkServerPrivate::newConnection(int socketDescriptor)
{
    B_Q(BNetworkServer);
    if (maxConnectionCount > 0 && q->currentConnectionCount() > maxConnectionCount)
        return;
    if (maxThreadCount > 0 && q->currentThreadCount() == maxThreadCount)
    {
        int cc = threads.first()->connectionCount();
        int ind = 0;
        for (int i = 0; i < threads.size(); ++i)
        {
            int ccc = threads.at(i)->connectionCount();
            if (ccc < cc)
            {
                cc = ccc;
                ind = i;
            }
        }
        threads[ind]->addConnection(socketDescriptor);
    }
    else
    {
        BNetworkServerThread *t = new BNetworkServerThread(this);
        QObject::connect( t, SIGNAL( finished() ), this, SLOT( finished() ) );
        t->start();
        t->addConnection(socketDescriptor);
        threads << t;
    }
}

void BNetworkServerPrivate::finished()
{
    BNetworkServerThread *t = static_cast<BNetworkServerThread *>( sender() );
    if (!t)
        return;
    threads.removeAll(t);
    t->deleteLater();
}

/*============================================================================
================================ BNetworkServer ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkServer::BNetworkServer(BGenericServer::ServerType type, QObject *parent) :
    QObject(parent), BBase( *new BNetworkServerPrivate(this) )
{
    d_func()->init();
    B_D(BNetworkServer);
    if (BGenericServer::NoServer != type)
    {
        d->server = new BGenericServer(type, this);
        d->server->setMaxPendingConnections(0);
        connect( d->server.data(), SIGNAL( newConnection(int) ), d, SLOT( newConnection(int) ) );
    }
}

BNetworkServer::~BNetworkServer()
{
    //
}

/*============================== Protected constructors ====================*/

BNetworkServer::BNetworkServer(BNetworkServerPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BNetworkServer::setMaxConnectionCount(int count)
{
    d_func()->maxConnectionCount = count > 0 ? count : 0;
}

void BNetworkServer::setMaxThreadCount(int count)
{
    d_func()->maxThreadCount = count > 0 ? count : 0;
}

bool BNetworkServer::isValid() const
{
    const B_D(BNetworkServer);
    return d->server.isNull() && d->server->isServerSet();
}

bool BNetworkServer::isListening() const
{
    const B_D(BNetworkServer);
    return !d->server.isNull() && d->server->isListening();
}

bool BNetworkServer::listen(const QString &address, quint16 port)
{
    B_D(BNetworkServer);
    return !d->server.isNull() && d->server->listen(address, port);
}

void BNetworkServer::close()
{
    B_D(BNetworkServer);
    if ( d->server.isNull() )
        return;
    d->server->close();
}

BGenericServer::ServerType BNetworkServer::serverType() const
{
    const B_D(BNetworkServer);
    return !d->server.isNull() ? d->server->serverType() : BGenericServer::NoServer;
}

int BNetworkServer::maxConnectionCount() const
{
    return d_func()->maxConnectionCount;
}

int BNetworkServer::currentConnectionCount() const
{
    int count = 0;
    foreach (BNetworkServerThread *t, d_func()->threads)
        count += t->connectionCount();
    return count;
}

int BNetworkServer::maxThreadCount() const
{
    return d_func()->maxThreadCount;
}

int BNetworkServer::currentThreadCount() const
{
    return d_func()->threads.size();
}

QList<BNetworkConnection *> BNetworkServer::connections() const
{
    QList<BNetworkConnection *> list;
    foreach (BNetworkServerThread *t, d_func()->threads)
        list << t->worker->getConnections();
    return list;
}

/*============================== Protected methods =========================*/

BGenericSocket *BNetworkServer::createSocket() const
{
    return new BGenericSocket(BGenericSocket::TcpSocket);
}
