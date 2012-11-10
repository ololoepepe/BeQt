#include "bgenericsocket.h"
#include "bgenericsocket_p.h"

#include <QObject>
#include <QPointer>
#include <QAbstractSocket>
#include <QLocalSocket>
#include <QIODevice>
#include <QHostAddress>
#include <QString>
#include <QNetworkProxy>
#include <QVariant>
#include <QTcpSocket>
#include <QSslSocket>
#include <QUdpSocket>
#include <QAuthenticator>
#include <QThread>
#include <QMetaType>
#include <QMetaObject>

#include <QDebug>

BGenericSocketPrivateObject::BGenericSocketPrivateObject(BGenericSocketPrivate *p) :
    QObject(0), _m_p(p)
{
    //
}

BGenericSocketPrivateObject::~BGenericSocketPrivateObject()
{
    //
}

//

void BGenericSocketPrivateObject::lsocketError(QLocalSocket::LocalSocketError socketError)
{
    _m_p->lsocketError(socketError);
}

void BGenericSocketPrivateObject::lsocketStateChanged(QLocalSocket::LocalSocketState socketState)
{
    _m_p->lsocketStateChanged(socketState);
}

void BGenericSocketPrivateObject::aboutToClose()
{
    _m_p->aboutToClose();
}

void BGenericSocketPrivateObject::connected()
{
    _m_p->connected();
}

void BGenericSocketPrivateObject::bytesWritten(qint64 bytes)
{
    _m_p->bytesWritten(bytes);
}

void BGenericSocketPrivateObject::disconnected()
{
    _m_p->disconnected();
}

void BGenericSocketPrivateObject::error(QAbstractSocket::SocketError socketError)
{
    _m_p->error(socketError);
}

void BGenericSocketPrivateObject::readChannelFinished()
{
    _m_p->readChannelFinished();
}

void BGenericSocketPrivateObject::readyRead()
{
    _m_p->readyRead();
}

void BGenericSocketPrivateObject::stateChanged(QAbstractSocket::SocketState socketState)
{
    _m_p->stateChanged(socketState);
}

//

BGenericSocketPrivate::BGenericSocketPrivate(BGenericSocket *q, BGenericSocket::SocketType type) :
    BBasePrivate(q), _m_o( new BGenericSocketPrivateObject(this) )
{
    switch (type)
    {
    case BGenericSocket::LocalSocket:
        setSocket(new QLocalSocket);
        break;
    case BGenericSocket::SslSocket:
        setSocket(new QSslSocket);
        break;
    case BGenericSocket::TcpSocket:
        setSocket(new QTcpSocket);
        break;
    case BGenericSocket::UdpSocket:
        setSocket(new QUdpSocket);
        break;
    default:
        break;
    }
}

BGenericSocketPrivate::~BGenericSocketPrivate()
{
    q_func()->close();
    _m_o->deleteLater();
}

//

void BGenericSocketPrivate::setSocket(QAbstractSocket *socket)
{
    B_Q(BGenericSocket);
    if (!asocket.isNull() || !lsocket.isNull() || !socket)
        return;
    socket->setParent(q);
    asocket = socket;
    connectIODevice();
    QObject::connect( socket, SIGNAL( connected() ), _m_o, SLOT( connected() ) );
    QObject::connect( socket, SIGNAL( disconnected() ), _m_o, SLOT( disconnected() ) );
    QObject::connect( socket, SIGNAL( error(QAbstractSocket::SocketError) ),
                      _m_o, SLOT( error(QAbstractSocket::SocketError) ) );
    QObject::connect( socket, SIGNAL( stateChanged(QAbstractSocket::SocketState) ),
                      _m_o, SLOT( stateChanged(QAbstractSocket::SocketState) ) );
}

void BGenericSocketPrivate::setSocket(QLocalSocket *socket)
{
    B_Q(BGenericSocket);
    if (!asocket.isNull() || !lsocket.isNull() || !socket)
        return;
    socket->setParent(q);
    lsocket = socket;
    connectIODevice();
    QObject::connect( socket, SIGNAL( connected() ), _m_o, SLOT( connected() ) );
    QObject::connect( socket, SIGNAL( disconnected() ), _m_o, SLOT( disconnected() ) );
    QObject::connect( socket, SIGNAL( error(QLocalSocket::LocalSocketError) ),
                      _m_o, SLOT( lsocketError(QLocalSocket::LocalSocketError) ) );
    QObject::connect( socket, SIGNAL( stateChanged(QLocalSocket::LocalSocketState) ),
                      _m_o, SLOT( lsocketStateChanged(QLocalSocket::LocalSocketState) ) );
}

void BGenericSocketPrivate::connectIODevice()
{
    QIODevice *device = !asocket.isNull() ? (QIODevice *) asocket.data() : (QIODevice *) lsocket.data();
    if (!device)
        return;
    QObject::connect( device, SIGNAL( aboutToClose() ), _m_o, SLOT( aboutToClose() ) );
    QObject::connect( device, SIGNAL( bytesWritten(qint64) ), _m_o, SLOT( bytesWritten(qint64) ) );
    QObject::connect( device, SIGNAL( readChannelFinished() ), _m_o, SLOT( readChannelFinished() ) );
    QObject::connect( device, SIGNAL( readyRead() ), _m_o, SLOT( readyRead() ) );
}

void BGenericSocketPrivate::disconnectIODevice()
{
    B_Q(BGenericSocket);
    QIODevice *device = q->ioDevice();
    if (!device)
        return;
    QObject::disconnect( device, SIGNAL( aboutToClose() ), _m_o, SLOT( aboutToClose() ) );
    QObject::disconnect( device, SIGNAL( bytesWritten(qint64) ), _m_o, SLOT( bytesWritten(qint64) ) );
    QObject::disconnect( device, SIGNAL( readChannelFinished() ), _m_o, SLOT( readChannelFinished() ) );
    QObject::disconnect( device, SIGNAL( readyRead() ), _m_o, SLOT( readyRead() ) );
}

void BGenericSocketPrivate::lsocketError(QLocalSocket::LocalSocketError socketError)
{
    QMetaObject::invokeMethod( q_func(), "error", Q_ARG( QAbstractSocket::SocketError,
                                                         static_cast<QAbstractSocket::SocketError>(socketError) ) );
}

void BGenericSocketPrivate::lsocketStateChanged(QLocalSocket::LocalSocketState socketState)
{
    QMetaObject::invokeMethod( q_func(), "stateChanged",
                               Q_ARG( QAbstractSocket::SocketState,
                                      static_cast<QAbstractSocket::SocketState>(socketState) ) );
}

void BGenericSocketPrivate::aboutToClose()
{
    QMetaObject::invokeMethod(q_func(), "aboutToClose");
}

void BGenericSocketPrivate::connected()
{
    QMetaObject::invokeMethod(q_func(), "connected");
}

void BGenericSocketPrivate::bytesWritten(qint64 bytes)
{
    QMetaObject::invokeMethod( q_func(), "bytesWritten", Q_ARG(qint64, bytes) );
}

void BGenericSocketPrivate::disconnected()
{
    QMetaObject::invokeMethod(q_func(), "disconnected");
}

void BGenericSocketPrivate::error(QAbstractSocket::SocketError socketError)
{
    QMetaObject::invokeMethod( q_func(), "error", Q_ARG(QAbstractSocket::SocketError, socketError) );
}

void BGenericSocketPrivate::readChannelFinished()
{
    QMetaObject::invokeMethod(q_func(), "readChannelFinished");
}

void BGenericSocketPrivate::readyRead()
{
    QMetaObject::invokeMethod(q_func(), "readyRead");
}

void BGenericSocketPrivate::stateChanged(QAbstractSocket::SocketState socketState)
{
    QMetaObject::invokeMethod( q_func(), "stateChanged", Q_ARG(QAbstractSocket::SocketState, socketState) );
}

//

BGenericSocket::BGenericSocket(SocketType type, QObject *parent) :
    QObject(parent), BBase( *new BGenericSocketPrivate(this, type) )
{
    //
}

//

QIODevice *BGenericSocket::ioDevice() const
{
    const B_D(BGenericSocket);
    return !d->asocket.isNull() ? static_cast<QIODevice *>( d->asocket.data() ) :
                                  static_cast<QIODevice *>( d->lsocket.data() );
}

QAbstractSocket *BGenericSocket::abstractSocket() const
{
    return d_func()->asocket.data();
}

QTcpSocket *BGenericSocket::tcpSocket() const
{
    return static_cast<QTcpSocket *>( d_func()->asocket.data() );
}

QSslSocket *BGenericSocket::sslSocket() const
{
    return static_cast<QSslSocket *>( d_func()->asocket.data() );
}

QUdpSocket *BGenericSocket::udpSocket() const
{
    return static_cast<QUdpSocket *>( d_func()->asocket.data() );
}

QLocalSocket *BGenericSocket::localSocket() const
{
    return d_func()->lsocket.data();
}

void BGenericSocket::abort()
{
    if ( !isSocketSet() )
        return;
    B_D(BGenericSocket);
    !d->asocket.isNull() ? d->asocket->abort() : d->lsocket->abort();
}

qint64 BGenericSocket::bytesAvailable() const
{
    return isSocketSet() ? ioDevice()->bytesAvailable() : 0;
}

qint64 BGenericSocket::bytesToWrite() const
{
    return isSocketSet() ? ioDevice()->bytesToWrite() : 0;
}

bool BGenericSocket::canReadLine() const
{
    return isSocketSet() && ioDevice()->canReadLine();
}

void BGenericSocket::close()
{
    if ( !isSocketSet() )
        return;
    ioDevice()->close();
}

void BGenericSocket::connectToHost(const QString &hostName, quint16 port, QIODevice::OpenMode openMode)
{
    if ( !isSocketSet() )
        return;
    B_D(BGenericSocket);
    !d->asocket.isNull() ? d->asocket->connectToHost(hostName, port, openMode) :
                           d->lsocket->connectToServer(hostName, openMode);
}

void BGenericSocket::disconnectFromHost()
{
    if ( !isSocketSet() )
        return;
    B_D(BGenericSocket);
    !d->asocket.isNull() ? d->asocket->disconnectFromHost() : d->lsocket->disconnectFromServer();
}

QAbstractSocket::SocketError BGenericSocket::error() const
{
    if ( !isSocketSet() )
        return QAbstractSocket::UnknownSocketError;
    const B_D(BGenericSocket);
    return !d->asocket.isNull() ? d->asocket->error() :
                                  static_cast<QAbstractSocket::SocketError>( d->lsocket->error() );
}

QString BGenericSocket::errorString() const
{
    return isSocketSet() ? ioDevice()->errorString() : "";
}

bool BGenericSocket::flush()
{
    if ( !isSocketSet() )
        return false;
    B_D(BGenericSocket);
    return !d->asocket.isNull() ? d->asocket->flush() : d->lsocket->flush();
}

bool BGenericSocket::isOpen() const
{
    return isSocketSet() && ioDevice()->isOpen();
}

bool BGenericSocket::isReadable() const
{
    return isSocketSet() && ioDevice()->isReadable();
}

bool BGenericSocket::isSequential() const
{
    return isSocketSet() && ioDevice()->isSequential();
}

bool BGenericSocket::isSocketSet() const
{
    const B_D(BGenericSocket);
    return !d->asocket.isNull() || !d->lsocket.isNull();
}

bool BGenericSocket::isTextModeEnabled() const
{
    return isSocketSet() && ioDevice()->isTextModeEnabled();
}

bool BGenericSocket::isValid() const
{
    if ( !isSocketSet() )
        return false;
    const B_D(BGenericSocket);
    return !d->asocket.isNull() ? d->asocket->isValid() : d->lsocket->isValid();
}

bool BGenericSocket::isWritable() const
{
    return isSocketSet() && ioDevice()->isWritable();
}

QIODevice::OpenMode BGenericSocket::openMode() const
{
    return isSocketSet() ? ioDevice()->openMode() : QIODevice::NotOpen;
}

QString BGenericSocket::peerAddress() const
{
    if ( !isSocketSet() )
        return "";
    const B_D(BGenericSocket);
    return !d->asocket.isNull() ? d->asocket->peerAddress().toString() : d->lsocket->serverName();
}

QByteArray BGenericSocket::read(qint64 maxSize)
{
    return isSocketSet() ? ioDevice()->read(maxSize) : QByteArray();
}

QByteArray BGenericSocket::readAll()
{
    return isSocketSet() ? ioDevice()->readAll() : QByteArray();
}

qint64 BGenericSocket::readBufferSize() const
{
    if ( !isSocketSet() )
        return 0;
    const B_D(BGenericSocket);
    return !d->asocket.isNull() ? d->asocket->readBufferSize() : d->lsocket->readBufferSize();
}

QByteArray BGenericSocket::readLine(qint64 maxSize)
{
    return isSocketSet() ? ioDevice()->readLine(maxSize) : QByteArray();
}

void BGenericSocket::setReadBufferSize(qint64 size)
{
    if ( !isSocketSet() )
        return;
    B_D(BGenericSocket);
    return !d->asocket.isNull() ? d->asocket->setReadBufferSize(size) : d->lsocket->setReadBufferSize(size);
}

bool BGenericSocket::setSocketDescriptor(int socketDescriptor, QIODevice::OpenMode openMode)
{
    if ( socketDescriptor < 0 || !isSocketSet() )
        return false;
    B_D(BGenericSocket);
    return !d->asocket.isNull() ?
                d->asocket->setSocketDescriptor(socketDescriptor, QAbstractSocket::ConnectedState, openMode) :
                d->lsocket->setSocketDescriptor(socketDescriptor, QLocalSocket::ConnectedState, openMode);
}

void BGenericSocket::setTextModeEnabled(bool enabled)
{
    if ( !isSocketSet() )
        return;
    ioDevice()->setTextModeEnabled(enabled);
}

int BGenericSocket::socketDescriptor() const
{
    if ( !isSocketSet() )
        return -1;
    const B_D(BGenericSocket);
    return !d->asocket.isNull() ? d->asocket->socketDescriptor() : d->lsocket->socketDescriptor();
}

BGenericSocket::SocketType BGenericSocket::socketType() const
{
    if ( !isSocketSet() )
        return NoSocket;
    const B_D(BGenericSocket);
    if ( !d->asocket.isNull() )
    {
        switch ( d->asocket->socketType() )
        {
        case QAbstractSocket::TcpSocket:
            return static_cast<QSslSocket *>( d->asocket.data() ) ? SslSocket : TcpSocket;
        case QAbstractSocket::UdpSocket:
            return UdpSocket;
        default:
            return NoSocket;
        }
    }
    else
    {
        return LocalSocket;
    }
}

QAbstractSocket::SocketState BGenericSocket::state() const
{
    if ( !isSocketSet() )
        return QAbstractSocket::UnconnectedState;
    const B_D(BGenericSocket);
    return !d->asocket.isNull() ? d->asocket->state() :
                                  static_cast<QAbstractSocket::SocketState>( d->lsocket->state() );
}

bool BGenericSocket::waitForBytesWritten(int msecs)
{
    return isSocketSet() && ioDevice()->waitForBytesWritten(msecs);
}

bool BGenericSocket::waitForConnected(int msecs)
{
    if ( !isSocketSet() )
        return false;
    B_D(BGenericSocket);
    return !d->asocket.isNull() ? d->asocket->waitForConnected(msecs) : d->lsocket->waitForConnected(msecs);
}

bool BGenericSocket::waitForDisconnected(int msecs)
{
    if ( !isSocketSet() )
        return false;
    B_D(BGenericSocket);
    return !d->asocket.isNull() ? d->asocket->waitForDisconnected(msecs) : d->lsocket->waitForDisconnected(msecs);
}

bool BGenericSocket::waitForReadyRead(int msecs)
{
    return isSocketSet() && ioDevice()->waitForReadyRead(msecs);
}

qint64 BGenericSocket::write(const QByteArray &byteArray)
{
    return isSocketSet() ? ioDevice()->write(byteArray) : -1;
}

//

BGenericSocket::BGenericSocket(BGenericSocketPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    //
}
