#include "bapplicationserver.h"
#include "bapplicationserver_p.h"
#include "bgenericserver.h"
#include "bgenericsocket.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QLocalServer>
#include <QScopedPointer>

/*============================================================================
================================ BApplicationServerPrivate ===================
============================================================================*/

/*============================== Public constructors =======================*/

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
BApplicationServerPrivate::BApplicationServerPrivate(BApplicationServer *q, quint16 port, int timeout) :
        BBasePrivate(q), Port(port), OperationTimeout(timeout)
{
    //
}
#else
BApplicationServerPrivate::BApplicationServerPrivate(BApplicationServer *q, const QString &serverName, int timeout) :
    BBasePrivate(q), ServerName(serverName), OperationTimeout(timeout)
{
        //
}
#endif

BApplicationServerPrivate::~BApplicationServerPrivate()
{
    if ( server->isListening() )
        server->close();
    server->deleteLater();
}

/*============================== Public methods ============================*/

void BApplicationServerPrivate::init()
{
    bTest(QCoreApplication::instance(), "BApplicationServer", "There must be a QCoreApplication instance");
    server = new BGenericServer(BGenericServer::LocalServer);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    server->localServer()->setSocketOptions(QLocalServer::WorldAccessOption);
#endif
    connect( server, SIGNAL( newPendingConnection() ), this, SLOT( newPendingConnection() ) );
}

/*============================== Public slots ==============================*/

void BApplicationServerPrivate::newPendingConnection()
{
    QScopedPointer<BGenericSocket> s ( server->nextPendingConnection() );
    if ( s.isNull() )
        return;
    if ( !s->waitForReadyRead(OperationTimeout) )
        return;
    QByteArray ba = s->readAll();
    QDataStream in(ba);
    in.setVersion(BeQt::DataStreamVersion);
    bool message = false;
    QStringList args;
    in >> message;
    if (message)
    {
        in >> args;
        q_func()->handleMessage(args);
    }
    else
    {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(BeQt::DataStreamVersion);
        out << true;
        s->write(data);
        s->waitForBytesWritten(OperationTimeout);
    }
    s->close();
}

/*============================================================================
================================ BApplicationServer ==========================
============================================================================*/

/*============================== Public constructors =======================*/

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
BApplicationServer::BApplicationServer(quint16 port, int operationTimeout) :
    BBase( *new BApplicationServerPrivate(this, port, operationTimeout) )
{
    d_func()->init();
}
#else
BApplicationServer::BApplicationServer(const QString &serverName, int operationTimeout) :
    BBase( *new BApplicationServerPrivate(this, serverName, operationTimeout) )
{
    d_func()->init();
}
#endif

BApplicationServer::~BApplicationServer()
{
    //
}

/*============================== Protected constructors ====================*/

BApplicationServer::BApplicationServer(BApplicationServerPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

bool BApplicationServer::isValid() const
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    return d_func()->Port;
#else
    return !d_func()->ServerName.isEmpty();
#endif
}

bool BApplicationServer::testServer() const
{
    if (!isValid())
        return false;
    const B_D(BApplicationServer);
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(BeQt::DataStreamVersion);
    out << false;
    BGenericSocket s(BGenericSocket::LocalSocket);
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    s.connectToHost("127.0.0.1", d->Port);
#else
    s.connectToHost(d->ServerName);
#endif
    bool b = s.waitForConnected(d->OperationTimeout) && s.write(data) &&
            s.waitForBytesWritten(d->OperationTimeout) && s.waitForReadyRead(d->OperationTimeout);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (!b)
        QLocalServer::removeServer(d->ServerName);
#endif
    return b;
}

bool BApplicationServer::listen()
{
    if (!QCoreApplication::instance() || !isValid())
        return false;
    B_D(BApplicationServer);
    if ( d->server->isListening() )
        return true;
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    return d->server->listen("127.0.0.1", d->Port);
#else
    return d->server->listen(d->ServerName);
#endif
}

bool BApplicationServer::sendMessage(int &argc, char **argv)
{
    if (argc < 1 || !argv || !isValid())
        return false;
    QStringList args;
    for (int i = 1; i < argc; ++i)
        args << argv[i];
    return sendMessage(args);
}

bool BApplicationServer::sendMessage(const QStringList &arguments)
{
    B_D(BApplicationServer);
    if (d->server->isListening() || !isValid())
        return false;
    QStringList args = !arguments.isEmpty() ? arguments : QStringList( QCoreApplication::arguments().mid(1) );
    if ( args.isEmpty() )
        return false;
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(BeQt::DataStreamVersion);
    out << true;
    out << args;
    BGenericSocket s(BGenericSocket::LocalSocket);
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    s.connectToHost("127.0.0.1", d->Port);
#else
    s.connectToHost(d->ServerName);
#endif
    if ( !s.waitForConnected(d->OperationTimeout) )
        return false;
    if ( !s.write(ba) )
        return false;
    return s.waitForBytesWritten(d->OperationTimeout);
}

/*============================== Protected methods =========================*/

void BApplicationServer::handleMessage(const QStringList &)
{
    //
}
