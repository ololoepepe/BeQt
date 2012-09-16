#include "bgenericsocket.h"

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

BGenericSocket::BGenericSocket(SocketType type, QObject *parent) :
    QObject(parent)
{
    switch (type)
    {
    case LocalSocket:
        _m_setSocket(new QLocalSocket);
        break;
    case SslSocket:
        _m_setSocket(new QSslSocket);
        break;
    case TcpSocket:
        _m_setSocket(new QTcpSocket);
        break;
    case UdpSocket:
        _m_setSocket(new QUdpSocket);
        break;
    default:
        break;
    }
}

//

QIODevice *BGenericSocket::ioDevice() const
{
    return !_m_asocket.isNull() ? static_cast<QIODevice *>( _m_asocket.data() ) :
                                  static_cast<QIODevice *>( _m_lsocket.data() );
}

QAbstractSocket *BGenericSocket::abstractSocket() const
{
    return _m_asocket.data();
}

QTcpSocket *BGenericSocket::tcpSocket() const
{
    return qobject_cast<QTcpSocket *>( _m_asocket.data() );
}

QSslSocket *BGenericSocket::sslSocket() const
{
    return qobject_cast<QSslSocket *>( _m_asocket.data() );
}

QUdpSocket *BGenericSocket::udpSocket() const
{
    return qobject_cast<QUdpSocket *>( _m_asocket.data() );
}

QLocalSocket *BGenericSocket::localSocket() const
{
    return _m_lsocket.data();
}

void BGenericSocket::abort()
{
    if ( !isSocketSet() )
        return;
    !_m_asocket.isNull() ? _m_asocket->abort() : _m_lsocket->abort();
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
    !_m_asocket.isNull() ? _m_asocket->connectToHost(hostName, port, openMode) :
                           _m_lsocket->connectToServer(hostName, openMode);
}

void BGenericSocket::disconnectFromHost()
{
    if ( !isSocketSet() )
        return;
    !_m_asocket.isNull() ? _m_asocket->disconnectFromHost() : _m_lsocket->disconnectFromServer();
}

QAbstractSocket::SocketError BGenericSocket::error() const
{
    if ( !isSocketSet() )
        return QAbstractSocket::UnknownSocketError;
    return !_m_asocket.isNull() ? _m_asocket->error() :
                                  static_cast<QAbstractSocket::SocketError>( _m_lsocket->error() );
}

QString BGenericSocket::errorString() const
{
    return isSocketSet() ? ioDevice()->errorString() : "";
}

bool BGenericSocket::flush()
{
    if ( !isSocketSet() )
        return false;
    return !_m_asocket.isNull() ? _m_asocket->flush() : _m_lsocket->flush();
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
    return !_m_asocket.isNull() || !_m_lsocket.isNull();
}

bool BGenericSocket::isTextModeEnabled() const
{
    return isSocketSet() && ioDevice()->isTextModeEnabled();
}

bool BGenericSocket::isValid() const
{
    if ( !isSocketSet() )
        return false;
    return !_m_asocket.isNull() ? _m_asocket->isValid() : _m_lsocket->isValid();
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
    return !_m_asocket.isNull() ? _m_asocket->peerAddress().toString() : _m_lsocket->serverName();
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
    return !_m_asocket.isNull() ? _m_asocket->readBufferSize() : _m_lsocket->readBufferSize();
}

QByteArray BGenericSocket::readLine(qint64 maxSize)
{
    return isSocketSet() ? ioDevice()->readLine(maxSize) : QByteArray();
}

void BGenericSocket::setReadBufferSize(qint64 size)
{
    if ( !isSocketSet() )
        return;
    return !_m_asocket.isNull() ? _m_asocket->setReadBufferSize(size) : _m_lsocket->setReadBufferSize(size);
}

bool BGenericSocket::setSocketDescriptor(int socketDescriptor, QIODevice::OpenMode openMode)
{
    if ( socketDescriptor < 0 || !isSocketSet() )
        return false;
    return !_m_asocket.isNull() ?
                _m_asocket->setSocketDescriptor(socketDescriptor, QAbstractSocket::ConnectedState, openMode) :
                _m_lsocket->setSocketDescriptor(socketDescriptor, QLocalSocket::ConnectedState, openMode);
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
    return !_m_asocket.isNull() ? _m_asocket->socketDescriptor() : _m_lsocket->socketDescriptor();
}

BGenericSocket::SocketType BGenericSocket::socketType() const
{
    if ( !isSocketSet() )
        return NoSocket;
    if ( !_m_asocket.isNull() )
    {
        switch ( _m_asocket->socketType() )
        {
        case QAbstractSocket::TcpSocket:
            return qobject_cast<QSslSocket *>( _m_asocket.data() ) ? SslSocket : TcpSocket;
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
    return !_m_asocket.isNull() ? _m_asocket->state() :
                                  static_cast<QAbstractSocket::SocketState>( _m_lsocket->state() );
}

bool BGenericSocket::waitForBytesWritten(int msecs)
{
    return isSocketSet() && ioDevice()->waitForBytesWritten(msecs);
}

bool BGenericSocket::waitForConnected(int msecs)
{
    if ( !isSocketSet() )
        return false;
    return !_m_asocket.isNull() ? _m_asocket->waitForConnected(msecs) : _m_lsocket->waitForConnected(msecs);
}

bool BGenericSocket::waitForDisconnected(int msecs)
{
    if ( !isSocketSet() )
        return false;
    return !_m_asocket.isNull() ? _m_asocket->waitForDisconnected(msecs) : _m_lsocket->waitForDisconnected(msecs);
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

void BGenericSocket::_m_setSocket(QAbstractSocket *socket)
{
    if (isSocketSet() || !socket)
        return;
    socket->setParent(this);
    _m_asocket = socket;
    _m_connectIODevice();
    connect(socket, SIGNAL( connected() ), this, SIGNAL( connected() ), Qt::DirectConnection);
    connect(socket, SIGNAL( disconnected() ), this, SIGNAL( disconnected() ), Qt::DirectConnection);
    connect(socket, SIGNAL( error(QAbstractSocket::SocketError) ),
            this, SIGNAL( error(QAbstractSocket::SocketError) ), Qt::DirectConnection);
    connect(socket, SIGNAL( stateChanged(QAbstractSocket::SocketState) ),
            this, SIGNAL( stateChanged(QAbstractSocket::SocketState) ), Qt::DirectConnection);
}

void BGenericSocket::_m_setSocket(QLocalSocket *socket)
{
    if (isSocketSet() || !socket)
        return;
    socket->setParent(this);
    _m_lsocket = socket;
    _m_connectIODevice();
    connect(socket, SIGNAL( connected() ), this, SIGNAL( connected() ), Qt::DirectConnection);
    connect(socket, SIGNAL( disconnected() ), this, SIGNAL( disconnected() ), Qt::DirectConnection);
    connect(socket, SIGNAL( error(QLocalSocket::SocketError) ),
            this, SLOT( _m_lsocketError(QLocalSocket::SocketError) ), Qt::DirectConnection);
    connect(socket, SIGNAL( stateChanged(QLocalSocket::SocketState) ),
            this, SIGNAL( _m_lsocketStateChanged(QLocalSocket::SocketState) ), Qt::DirectConnection);
}

void BGenericSocket::_m_connectIODevice()
{
    QIODevice *device = ioDevice();
    if (!device)
        return;
    connect(device, SIGNAL( aboutToClose() ), this, SIGNAL( aboutToClose() ), Qt::DirectConnection);
    connect(device, SIGNAL( bytesWritten(qint64) ), this, SIGNAL( bytesWritten(qint64) ), Qt::DirectConnection);
    connect(device, SIGNAL( readChannelFinished() ), this, SIGNAL( readChannelFinished() ), Qt::DirectConnection);
    connect(device, SIGNAL( readyRead() ), this, SIGNAL( readyRead() ), Qt::DirectConnection);
}

void BGenericSocket::_m_disconnectIODevice()
{
    QIODevice *device = ioDevice();
    if (!device)
        return;
    disconnect( device, SIGNAL( aboutToClose() ), this, SIGNAL( aboutToClose() ) );
    disconnect( device, SIGNAL( bytesWritten(qint64) ), this, SIGNAL( bytesWritten(qint64) ) );
    disconnect( device, SIGNAL( readChannelFinished() ), this, SIGNAL( readChannelFinished() ) );
    disconnect( device, SIGNAL( readyRead() ), this, SIGNAL( readyRead() ) );
}

//

void BGenericSocket::_m_lsocketError(QLocalSocket::LocalSocketError socketError)
{
    emit error( static_cast<QAbstractSocket::SocketError>(socketError) );
}

void BGenericSocket::_m_lsocketStateChanged(QLocalSocket::LocalSocketState socketState)
{
    emit stateChanged( static_cast<QAbstractSocket::SocketState>(socketState) );
}
