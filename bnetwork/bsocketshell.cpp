#include "bsocketshell.h"

#include <QDataStream>
#include <QObject>
#include <QAbstractSocket>
#include <QLocalSocket>
#include <QByteArray>
#include <QIODevice>

const QDataStream::Version BSocketShell::_m_DSVer = QDataStream::Qt_4_7;

//

BSocketShell::BSocketShell(QObject *parent) :
    QObject(parent)
{
    _m_socket = 0;
    _m_bytesToWrite = 0;
    _m_dataSize = 0;
    _m_comprLvl = 0;
    _m_criticalBufferSize = 0;
    _m_closeOnCriticalBufferSizeReached = false;
}

//

void BSocketShell::setSocket(QAbstractSocket *socket)
{
    if ( !_m_unsetSocket() )
        return;
    _m_setSocket(socket);
}

void BSocketShell::setSocket(QLocalSocket *socket)
{
    if ( !_m_unsetSocket() )
        return;
    _m_setSocket(socket);
}

void BSocketShell::setCompressionLevel(int level)
{
    if ( isBuisy() )
        return;
    if (level < 0)
        level = -1;
    else if (level > 9)
        level = 9;
    _m_comprLvl = level;
}

void BSocketShell::setCriticalBufferSize(SzType size)
{
    _m_criticalBufferSize = size;
}

void BSocketShell::setCloseOnCriticalBufferSizeReached(bool close)
{
    _m_closeOnCriticalBufferSizeReached = close;
}

bool BSocketShell::unsetSocket()
{
    return _m_unsetSocket();
}

QIODevice *BSocketShell::socket() const
{
    return _m_socket;
}

int BSocketShell::compressionLevel() const
{
    return _m_comprLvl;
}

BSocketShell::SzType BSocketShell::criticalBufferSize() const
{
    return _m_criticalBufferSize;
}

bool BSocketShell::closeOnCriticalBufferSizeReached() const
{
    return _m_closeOnCriticalBufferSizeReached;
}

bool BSocketShell::isBuisy() const
{
    return _m_bytesToWrite || _m_dataSize;
}

void BSocketShell::sendData(const QByteArray &data)
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(_m_DSVer);
    out << (SzType) 0;
    out << qCompress(data, _m_comprLvl);
    out.device()->seek(0);
    out << (SzType) ( ba.size() - sizeof(SzType) );
    _m_bytesToWrite = _m_socket->write(ba);
    QAbstractSocket *s = dynamic_cast<QAbstractSocket *>(_m_socket);
    if (s)
        s->flush();
    QLocalSocket *ls = dynamic_cast<QLocalSocket *>(_m_socket);
    if (ls)
        ls->flush();
}

//

void BSocketShell::_m_setSocket(QIODevice *socket)
{
    if (!socket)
        return;
    _m_socket = socket;
    connect( _m_socket, SIGNAL( bytesWritten(qint64) ), this, SLOT( _m_bytesWritten(qint64) ) );
    connect( _m_socket, SIGNAL( readyRead() ), this, SLOT( _m_readyRead() ) );
}

bool BSocketShell::_m_unsetSocket()
{
    if (!_m_socket)
        return true;
    if ( isBuisy() )
        return false;
    disconnect( _m_socket, SIGNAL( bytesWritten(qint64) ), this, SLOT( _m_bytesWritten(qint64) ) );
    disconnect( _m_socket, SIGNAL( readyRead() ), this, SLOT( _m_readyRead() ) );
    _m_socket = 0;
    _m_bytesToWrite = 0;
    _m_dataSize = 0;
    return true;
}

//

void BSocketShell::_m_bytesWritten(qint64 bytes)
{
    _m_bytesToWrite -= bytes;
    if (0 == _m_bytesToWrite)
        emit dataSent();
}

void BSocketShell::_m_readyRead()
{
    QDataStream in(_m_socket);
    in.setVersion(_m_DSVer);
    forever
    {
        if (_m_criticalBufferSize > 0 && _m_socket->bytesAvailable() >= (qint64) _m_criticalBufferSize)
        {
            if (_m_closeOnCriticalBufferSizeReached)
                _m_socket->close();
            emit criticalBufferSizeReached();
        }
        if (0 == _m_dataSize)
        {
            if ( _m_socket->bytesAvailable() < sizeof(SzType) )
                break;
            in >> _m_dataSize;
        }
        if (_m_socket->bytesAvailable() < (qint64) _m_dataSize)
            break;
        QByteArray ba;
        in >> ba;
        _m_dataSize = 0;
        emit dataReceived( qUncompress(ba) );
    }
}
