#include "bremotelogger.h"
#include "bgenericsocket.h"

#include <BeQtCore/BLogger>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/private/blogger_p.h>
#include <BeQtCore/BeQt>

#include <QObject>
#include <QString>
#include <QPointer>
#include <QByteArray>

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
    virtual void tryLog(const QString &msg);
    void tryLogToRemote(const QString &text);
    void removeSocket();
public:
    QPointer<BGenericSocket> socket;
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

void BRemoteLoggerPrivate::tryLog(const QString &msg)
{
    BLoggerPrivate::tryLog(msg);
    tryLogToRemote(msg);
}

void BRemoteLoggerPrivate::tryLogToRemote(const QString &text)
{
    QMutexLocker locker(&remoteMutex);
    if ( !logToRemote || socket.isNull() || socket->isWritable() )
        return;
    socket->write( text.toUtf8() );
    socket->flush();
    socket->waitForBytesWritten(timeout);
}

void BRemoteLoggerPrivate::removeSocket()
{
    if ( !socket.isNull() && (!socket->parent() || socket->parent() == this) )
        socket->deleteLater();
}

/*============================================================================
================================ BRemoteLogger ===============================
============================================================================*/

/*============================== Public constructors =======================*/

BRemoteLogger::BRemoteLogger(QObject *parent) :
    BLogger(*new BLoggerPrivate(this), parent)
{
    d_func()->init();
}

BRemoteLogger::BRemoteLogger(BGenericSocket *socket, QObject *parent) :
    BLogger(*new BLoggerPrivate(this), parent)
{
    d_func()->init();
    setRemote(socket);
}

BRemoteLogger::BRemoteLogger(BGenericSocket *socket, const QString &fileName, QObject *parent) :
    BLogger(*new BLoggerPrivate(this), parent)
{
    d_func()->init();
    setFileName(fileName);
    setRemote(socket);
}

BRemoteLogger::BRemoteLogger(const QString &hostName, quint16 port, QObject *parent) :
    BLogger(*new BLoggerPrivate(this), parent)
{
    d_func()->init();
    setRemote(hostName, port);
}

BRemoteLogger::BRemoteLogger(const QString &hostName, quint16 port, const QString &fileName, QObject *parent) :
    BLogger(*new BLoggerPrivate(this), parent)
{
    d_func()->init();
    setFileName(fileName);
    setRemote(hostName, port);
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
        return setRemote(0);
    BGenericSocket *s = new BGenericSocket(BGenericSocket::TcpSocket, this);
    s->connectToHost(hostName, port);
    if ( !s->waitForConnected(d->timeout) )
    {
        s->deleteLater();
        s = 0;
    }
    setRemote(s);
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

int BRemoteLogger::remoteTimeout() const
{
    const B_D(BRemoteLogger);
    QMutexLocker locker(&d->remoteMutex);
    return d->timeout;
}
