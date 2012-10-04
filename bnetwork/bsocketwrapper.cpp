#include "bsocketwrapper.h"
#include "bgenericsocket.h"

#include <QDataStream>
#include <QObject>
#include <QAbstractSocket>
#include <QLocalSocket>
#include <QByteArray>
#include <QIODevice>
#include <QThread>
#include <QUuid>

BSocketWrapper::MetaData::MetaData()
{
    _m_request = true;
}

BSocketWrapper::MetaData::MetaData(const MetaData &other)
{
    *this = other;
}

BSocketWrapper::MetaData::MetaData(const QUuid &id, bool request, const QString &operation)
{
    setId(id);
    setIsRequest(request);
    setOperation(operation);
}

//

void BSocketWrapper::MetaData::setIsRequest(bool request)
{
    _m_request = request;
}

void BSocketWrapper::MetaData::setId(const QUuid &id)
{
    _m_id = id;
}

void BSocketWrapper::MetaData::setOperation(const QString &operation)
{
    _m_operation = operation;
}

void BSocketWrapper::MetaData::invalidate()
{
    _m_id = QUuid();
    _m_request = true;
    _m_operation.clear();
}

bool BSocketWrapper::MetaData::isRequest() const
{
    return _m_request;
}

const QUuid &BSocketWrapper::MetaData::id() const
{
    return _m_id;
}

const QString &BSocketWrapper::MetaData::operation() const
{
    return _m_operation;
}

bool BSocketWrapper::MetaData::isValid() const
{
    return !_m_id.isNull() && !_m_operation.isEmpty();
}

//

BSocketWrapper::MetaData &BSocketWrapper::MetaData::operator=(const MetaData &other)
{
    setId( other.id() );
    setIsRequest( other.isRequest() );
    setOperation( other.operation() );
    return *this;
}

bool BSocketWrapper::MetaData::operator==(const MetaData &other) const
{
    return id() == other.id() && isRequest() == other.isRequest() && operation() == other.operation();
}

bool BSocketWrapper::MetaData::operator<(const MetaData &other) const
{
    return id() < other.id();
}

////

const QDataStream::Version BSocketWrapper::DataStreamVersion = QDataStream::Qt_4_8;

//

BSocketWrapper::BSocketWrapper(QObject *parent) :
    QObject(parent)
{
    _m_init();
}

BSocketWrapper::BSocketWrapper(BGenericSocket *socket, QObject *parent) :
    QObject(parent)
{
    _m_init();
    setSocket(socket);
}

BSocketWrapper::BSocketWrapper(BGenericSocket::SocketType type, QObject *parent) :
    QObject(parent)
{
    _m_init();
    setSocket( new BGenericSocket(type) );
}

//

void BSocketWrapper::setSocket(BGenericSocket *socket)
{
    if ( !socket || socket->thread() != thread() || !socket->isSocketSet() || !unsetSocket() )
        return;
    _m_socket = socket;
    connect(_m_socket, SIGNAL( bytesWritten(qint64) ), this, SLOT( _m_bytesWritten(qint64) ), Qt::DirectConnection);
    connect(_m_socket, SIGNAL( disconnected() ), this, SLOT( _m_disconnected() ), Qt::DirectConnection);
    connect(_m_socket, SIGNAL( error(QAbstractSocket::SocketError) ),
            this, SLOT( _m_error(QAbstractSocket::SocketError) ), Qt::DirectConnection);
    connect(_m_socket, SIGNAL( readyRead() ), this, SLOT( _m_readyRead() ), Qt::DirectConnection);
}

void BSocketWrapper::setCompressionLevel(int level)
{
    if (level < 0)
        level = -1;
    else if (level > 9)
        level = 9;
    _m_comprLvl = level;
}

void BSocketWrapper::setCriticalBufferSize(qint64 size)
{
    _m_criticalBufferSize = size;
}

void BSocketWrapper::setCloseOnCriticalBufferSize(bool close)
{
    _m_closeOnCriticalBufferSize = close;
}

bool BSocketWrapper::unsetSocket()
{
    if (!_m_socket)
        return true;
    if ( isBuisy() )
        return false;
    disconnect( _m_socket, SIGNAL( bytesWritten(qint64) ), this, SLOT( _m_bytesWritten(qint64) ) );
    disconnect( _m_socket, SIGNAL( disconnected() ), this, SLOT( _m_disconnected() ) );
    disconnect( _m_socket, SIGNAL( error(QAbstractSocket::SocketError) ),
                this, SLOT( _m_error(QAbstractSocket::SocketError) ) );
    disconnect( _m_socket, SIGNAL( readyRead() ), this, SLOT( _m_readyRead() ) );
    _m_socket = 0;
    _m_resetIn();
    _m_resetOut();
    return true;
}

BGenericSocket *BSocketWrapper::socket() const
{
    return _m_socket.data();
}

int BSocketWrapper::compressionLevel() const
{
    return _m_comprLvl;
}

qint64 BSocketWrapper::criticalBufferSize() const
{
    return _m_criticalBufferSize;
}

bool BSocketWrapper::closeOnCriticalBufferSize() const
{
    return _m_closeOnCriticalBufferSize;
}

bool BSocketWrapper::isBuisy() const
{
    return _m_bytesOutTotal > 0;
}

bool BSocketWrapper::sendData(const QByteArray &data, const MetaData &metaData)
{
    if ( _m_socket.isNull() || !_m_socket->isWritable() || isBuisy() )
        return false;
    if ( !metaData.isValid() && data.isEmpty() )
        return true;
    if ( metaData.isValid() )
    {
        _m_metaOut = metaData;
        QByteArray bam;
        QDataStream outm(&bam, QIODevice::WriteOnly);
        outm.setVersion(DataStreamVersion);
        outm << (qint64) 0;
        outm << true;
        outm << metaData.id();
        outm << metaData.isRequest();
        outm << metaData.operation();
        outm.device()->seek(0);
        outm << (qint64) ( bam.size() - sizeof(qint64) );
        qint64 bom = _m_socket->write(bam);
        if (bom > 0)
            _m_bytesOutTotal = bom;
        else
            return false;
    }
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(DataStreamVersion);
    out << (qint64) 0;
    out << false;
    out << qCompress(data, _m_comprLvl);
    out.device()->seek(0);
    out << (qint64) ( ba.size() - sizeof(qint64) );
    qint64 bo = _m_socket->write(ba);
    if (bo < 0)
        return false;
    _m_bytesOutTotal += bo;
    if ( _m_metaOut.isValid() )
        emit uploadProgress(_m_metaOut, _m_bytesOutReady, _m_bytesOutReady);
    _m_socket->flush();
    return true;
}

//

void BSocketWrapper::_m_init()
{
    _m_resetIn();
    _m_resetOut();
    _m_comprLvl = 0;
    _m_criticalBufferSize = 0;
    _m_closeOnCriticalBufferSize = false;
}

void BSocketWrapper::_m_resetIn()
{
    _m_bytesInTotal = 0;
    _m_metaIn.invalidate();
}

void BSocketWrapper::_m_resetOut()
{
    _m_bytesOutTotal = 0;
    _m_bytesOutReady = 0;
    _m_metaOut.invalidate();
}

//

void BSocketWrapper::_m_bytesWritten(qint64 bytes)
{
    _m_bytesOutReady += bytes;
    if ( _m_metaOut.isValid() )
        emit uploadProgress(_m_metaOut, _m_bytesOutReady, _m_bytesOutTotal);
    if (_m_bytesOutReady == _m_bytesOutTotal)
    {
        emit dataSent(_m_metaOut);
        _m_resetOut();
    }
}

void BSocketWrapper::_m_disconnected()
{
    _m_resetIn();
    _m_resetOut();
}

void BSocketWrapper::_m_error(QAbstractSocket::SocketError socketError)
{
    //
}

void BSocketWrapper::_m_readyRead()
{
    QDataStream in( _m_socket->ioDevice() );
    in.setVersion(DataStreamVersion);
    forever
    {
        if (_m_criticalBufferSize > 0 && _m_socket->bytesAvailable() >= _m_criticalBufferSize)
        {
            if (_m_closeOnCriticalBufferSize)
                _m_socket->close();
            emit criticalBufferSizeReached();
            break;
        }
        if (0 == _m_bytesInTotal)
        {
            if ( _m_socket->bytesAvailable() < sizeof(qint64) )
                break;
            in >> _m_bytesInTotal;
        }
        qint64 bytes = _m_socket->bytesAvailable();
        if ( _m_metaIn.isValid() )
            emit downloadProgress(_m_metaIn, bytes, bytes < _m_bytesInTotal ? bytes : _m_bytesInTotal);
        if (bytes < _m_bytesInTotal)
            break;
        bool meta = false;
        in >> meta;
        if (meta)
        {
            QUuid id;
            bool req;
            QString op;
            in >> id;
            in >> req;
            in >> op;
            _m_metaIn.setId(id);
            _m_metaIn.setIsRequest(req);
            _m_metaIn.setOperation(op);
            _m_bytesInTotal = 0;
        }
        else
        {
            QByteArray ba;
            in >> ba;
            emit dataReceived(qUncompress(ba), _m_metaIn);
            _m_resetIn();
        }
    }
}
