#include "bremotelogger.h"
#include "bgenericsocket.h"
#include "bnetworkconnection.h"
#include "bnetworkserver.h"
#include "bnetworkoperation.h"

#include <BeQtCore/BLogger>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/private/blogger_p.h>
#include <BeQtCore/BeQt>

#include <QObject>
#include <QString>
#include <QPointer>
#include <QByteArray>
#include <QList>
#include <QThread>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ BRemoteLoggerPrivate ========================
============================================================================*/

class BRemoteLoggerPrivate : public BLoggerPrivate
{
    B_DECLARE_PUBLIC(BRemoteLogger)
public:
    explicit BRemoteLoggerPrivate(BRemoteLogger *q);
    ~BRemoteLoggerPrivate();
public:
    void init();
    void tryLogToRemote(const QString &text, BLogger::Level lvl);
    void removeSocket();
    QList<BNetworkConnection *> getConnections() const;
public:
    QPointer<BGenericSocket> socket;
    mutable QList< QPointer<BNetworkConnection> > connections;
    QPointer<BNetworkServer> server;
    bool logToRemote;
    int timeout;
    mutable QMutex remoteMutex;
private:
    Q_DISABLE_COPY(BRemoteLoggerPrivate)
};

/*============================================================================
================================ BRemoteLoggerPrivate ========================
============================================================================*/

/*============================== Public constructors =======================*/

BRemoteLoggerPrivate::BRemoteLoggerPrivate(BRemoteLogger *q) :
    BLoggerPrivate(q)
{
    //
}

BRemoteLoggerPrivate::~BRemoteLoggerPrivate()
{
    removeSocket();
}

/*============================== Public methods ============================*/

void BRemoteLoggerPrivate::init()
{
    logToRemote = true;
    timeout = 10 * BeQt::Second;
}

void BRemoteLoggerPrivate::tryLogToRemote(const QString &text, BLogger::Level lvl)
{
    QMutexLocker locker(&remoteMutex);
    if (!logToRemote)
        return;
    if (!socket.isNull() && socket->isWritable())
    {
        socket->write( text.toUtf8() );
        socket->flush();
        socket->waitForBytesWritten(timeout);
    }
    QList<BNetworkConnection *> list = getConnections();
    if (!server.isNull())
        list << server.data()->connections();
    QThread *t = thread();
    foreach (BNetworkConnection *c, list)
    {
        if (c->thread() != t)
            continue;
        QVariantMap m;
        m.insert("text", text);
        m.insert("level", lvl);
        BNetworkOperation *op = c->sendRequest("", m);
        op->setAutoDelete(true);
        op->setStartTimeout();
        op->setFinishTimeout();
    }
}

void BRemoteLoggerPrivate::removeSocket()
{
    if ( !socket.isNull() && (!socket->parent() || socket->parent() == this) )
        socket->deleteLater();
}

QList<BNetworkConnection *> BRemoteLoggerPrivate::getConnections() const
{
    QList<BNetworkConnection *> list;
    for (int i = connections.size() - 1; i >= 0; --i)
    {
        if (connections.at(i).isNull())
            connections.removeAt(i);
        else
            list << connections.at(i).data();
    }
    return list;
}

/*============================================================================
================================ BRemoteLogger ===============================
============================================================================*/

/*============================== Public constructors =======================*/

BRemoteLogger::BRemoteLogger(QObject *parent) :
    BLogger(*new BRemoteLoggerPrivate(this), parent)
{
    d_func()->init();
}

BRemoteLogger::BRemoteLogger(BGenericSocket *socket, QObject *parent) :
    BLogger(*new BRemoteLoggerPrivate(this), parent)
{
    d_func()->init();
    setRemote(socket);
}

BRemoteLogger::BRemoteLogger(const QString &hostName, quint16 port, QObject *parent) :
    BLogger(*new BRemoteLoggerPrivate(this), parent)
{
    d_func()->init();
    setRemote(hostName, port);
}

BRemoteLogger::BRemoteLogger(BNetworkConnection *c, QObject *parent) :
    BLogger(*new BRemoteLoggerPrivate(this), parent)
{
    d_func()->init();
    setRemote(c);
}

BRemoteLogger::BRemoteLogger(const QList<BNetworkConnection *> &list, QObject *parent) :
    BLogger(*new BRemoteLoggerPrivate(this), parent)
{
    d_func()->init();
    setRemote(list);
}

BRemoteLogger::BRemoteLogger(BNetworkServer *server, QObject *parent) :
    BLogger(*new BRemoteLoggerPrivate(this), parent)
{
    d_func()->init();
    setRemote(server);
}

BRemoteLogger::~BRemoteLogger()
{
    //
}

/*============================== Protected constructors ====================*/

BRemoteLogger::BRemoteLogger(BRemoteLoggerPrivate &d) :
    BLogger(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BRemoteLogger::setLogToRemoteEnabled(bool enabled)
{
    B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    d->logToRemote = enabled;
}

void BRemoteLogger::setRemote(BGenericSocket *socket)
{
    B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    d->removeSocket();
    if ( !socket || !socket->isWritable() )
        return;
    d->socket = socket;
    if ( !socket->parent() )
        socket->setParent(this);
}

void BRemoteLogger::setRemote(const QString &hostName, quint16 port)
{
    B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    if (hostName.isEmpty() || !port)
        return setRemote((BGenericSocket *) 0);
    BGenericSocket *s = new BGenericSocket(BGenericSocket::TcpSocket, this);
    s->connectToHost(hostName, port);
    if ( !s->waitForConnected(d->timeout) )
    {
        s->deleteLater();
        s = 0;
    }
    setRemote(s);
}

void BRemoteLogger::setRemote(BNetworkConnection *c)
{
    setRemote(QList<BNetworkConnection *>() << c);
}

void BRemoteLogger::setRemote(const QList<BNetworkConnection *> &list)
{
    B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    foreach (BNetworkConnection *c, list)
        d->connections << QPointer<BNetworkConnection>(c);
}

void BRemoteLogger::setRemote(BNetworkServer *server)
{
    B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    d->server = server;
}

void BRemoteLogger::setRemoteTimeout(int msecs)
{
    B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    if (msecs < -1)
        msecs = -1;
    d->timeout = msecs;
}

bool BRemoteLogger::isLogToRemoteEnabled() const
{
    const B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    return d->logToRemote;
}

BGenericSocket *BRemoteLogger::socket() const
{
    return d_func()->socket.data();
}

QString BRemoteLogger::hostName() const
{
    const B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    return !d->socket.isNull() ? d->socket->peerAddress() : QString();
}

quint16 BRemoteLogger::port() const
{
    const B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    return !d->socket.isNull() ? d->socket->peerPort() : 0;
}

QList<BNetworkConnection *> BRemoteLogger::connections() const
{
    return d_func()->getConnections();
}

BNetworkServer *BRemoteLogger::server() const
{
    return d_func()->server;
}

int BRemoteLogger::remoteTimeout() const
{
    const B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    return d->timeout;
}

/*============================== Protected methods =========================*/

void BRemoteLogger::userLog(const QString &text, Level level)
{
    d_func()->tryLogToRemote(text, level);
}
