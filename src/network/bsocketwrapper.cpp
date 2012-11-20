#include "bsocketwrapper.h"
#include "bgenericsocket.h"
#include "bsocketwrapper_p.h"

#include <QDataStream>
#include <QObject>
#include <QAbstractSocket>
#include <QLocalSocket>
#include <QByteArray>
#include <QIODevice>
#include <QThread>
#include <QUuid>
#include <QMetaObject>

/*============================================================================
================================ Socket Wrapper Private
============================================================================*/

BSocketWrapperPrivate::BSocketWrapperPrivate(BSocketWrapper *q) :
    BBasePrivate(q)
{
    //
}

BSocketWrapperPrivate::~BSocketWrapperPrivate()
{
    //
}

//

void BSocketWrapperPrivate::init()
{
    BBasePrivate::init();
    resetIn();
    resetOut();
    comprLvl = 0;
    criticalBufferSize = 0;
    closeOnCriticalBufferSize = false;
}

void BSocketWrapperPrivate::resetIn()
{
    bytesInTotal = 0;
    metaIn.invalidate();
}

void BSocketWrapperPrivate::resetOut()
{
    bytesOutTotal = 0;
    bytesOutReady = 0;
    metaOut.invalidate();
}

//

void BSocketWrapperPrivate::bytesWritten(qint64 bytes)
{
    B_Q(BSocketWrapper);
    bytesOutReady += bytes;
    if ( metaOut.isValid() )
        QMetaObject::invokeMethod( q, "uploadProgress", Q_ARG(BNetworkOperationMetaData, metaOut),
                                   Q_ARG(qint64, bytesOutReady), Q_ARG(qint64, bytesOutTotal) );
    if (bytesOutReady == bytesOutTotal)
    {
        QMetaObject::invokeMethod( q, "dataSent", Q_ARG(BNetworkOperationMetaData, metaOut) );
        resetOut();
    }
}

void BSocketWrapperPrivate::disconnected()
{
    resetIn();
    resetOut();
}

void BSocketWrapperPrivate::error(QAbstractSocket::SocketError socketError)
{
    //
}

void BSocketWrapperPrivate::readyRead()
{
    B_Q(BSocketWrapper);
    QDataStream in( socket->ioDevice() );
    in.setVersion(BSocketWrapper::DataStreamVersion);
    forever
    {
        if (criticalBufferSize > 0 && socket->bytesAvailable() >= criticalBufferSize)
        {
            if (closeOnCriticalBufferSize)
                socket->close();
            QMetaObject::invokeMethod(q, "criticalBufferSizeReached");
            break;
        }
        if (0 == bytesInTotal)
        {
            if ( socket->bytesAvailable() < sizeof(qint64) )
                break;
            in >> bytesInTotal;
        }
        qint64 bytes = socket->bytesAvailable();
        if ( metaIn.isValid() )
        {
            qint64 total = (bytes < bytesInTotal) ? bytes : bytesInTotal;
            QMetaObject::invokeMethod( q, "downloadProgress", Q_ARG(BNetworkOperationMetaData, metaIn),
                                       Q_ARG(qint64, bytes), Q_ARG(qint64, total) );
        }
        if (bytes < bytesInTotal)
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
            metaIn.setId(id);
            metaIn.setIsRequest(req);
            metaIn.setOperation(op);
            bytesInTotal = 0;
        }
        else
        {
            QByteArray ba;
            in >> ba;
            QMetaObject::invokeMethod( q, "dataReceived", Q_ARG( QByteArray, qUncompress(ba) ),
                                       Q_ARG(BNetworkOperationMetaData, metaIn) );
            resetIn();
        }
    }
}

/*============================================================================
================================ Socket Wrapper
============================================================================*/

BSocketWrapper::BSocketWrapper(QObject *parent) :
    QObject(parent), BBase( *new BSocketWrapperPrivate(this) )
{
    //
}

BSocketWrapper::BSocketWrapper(BGenericSocket *socket, QObject *parent) :
    QObject(parent), BBase( *new BSocketWrapperPrivate(this) )
{
    setSocket(socket);
}

BSocketWrapper::BSocketWrapper(BGenericSocket::SocketType type, QObject *parent) :
    QObject(parent), BBase( *new BSocketWrapperPrivate(this) )
{
    setSocket( new BGenericSocket(type) );
}

//

void BSocketWrapper::setSocket(BGenericSocket *socket)
{
    B_D(BSocketWrapper);
    if ( !socket || socket->thread() != thread() || !socket->isSocketSet() || !unsetSocket() )
        return;
    d->socket = socket;
    connect( socket, SIGNAL( bytesWritten(qint64) ), d, SLOT( bytesWritten(qint64) ) );
    connect( socket, SIGNAL( disconnected() ), d, SLOT( disconnected() ) );
    connect( socket, SIGNAL( error(QAbstractSocket::SocketError) ),
             d, SLOT( error(QAbstractSocket::SocketError) ) );
    connect( socket, SIGNAL( readyRead() ), d, SLOT( readyRead() ) );
}

void BSocketWrapper::setCompressionLevel(int level)
{
    if (level < 0)
        level = -1;
    else if (level > 9)
        level = 9;
    d_func()->comprLvl = level;
}

void BSocketWrapper::setCriticalBufferSize(qint64 size)
{
    d_func()->criticalBufferSize = size;
}

void BSocketWrapper::setCloseOnCriticalBufferSize(bool close)
{
    d_func()->closeOnCriticalBufferSize = close;
}

bool BSocketWrapper::unsetSocket()
{
    B_D(BSocketWrapper);
    if (!d->socket)
        return true;
    if ( isBuisy() )
        return false;
    disconnect( d->socket, SIGNAL( bytesWritten(qint64) ), d, SLOT( bytesWritten(qint64) ) );
    disconnect( d->socket, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
    disconnect( d->socket, SIGNAL( error(QAbstractSocket::SocketError) ),
                d, SLOT( error(QAbstractSocket::SocketError) ) );
    disconnect( d->socket, SIGNAL( readyRead() ), d, SLOT( readyRead() ) );
    d->socket = 0;
    d->resetIn();
    d->resetOut();
    return true;
}

BGenericSocket *BSocketWrapper::socket() const
{
    return d_func()->socket.data();
}

int BSocketWrapper::compressionLevel() const
{
    return d_func()->comprLvl;
}

qint64 BSocketWrapper::criticalBufferSize() const
{
    return d_func()->criticalBufferSize;
}

bool BSocketWrapper::closeOnCriticalBufferSize() const
{
    return d_func()->closeOnCriticalBufferSize;
}

bool BSocketWrapper::isBuisy() const
{
    return d_func()->bytesOutTotal > 0;
}

bool BSocketWrapper::sendData(const QByteArray &data, const BNetworkOperationMetaData &metaData)
{
    B_D(BSocketWrapper);
    if ( d->socket.isNull() || !d->socket->isWritable() || isBuisy() )
        return false;
    if ( !metaData.isValid() && data.isEmpty() )
        return true;
    if ( metaData.isValid() )
    {
        d->metaOut = metaData;
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
        qint64 bom = d->socket->write(bam);
        if (bom > 0)
            d->bytesOutTotal = bom;
        else
            return false;
    }
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(DataStreamVersion);
    out << (qint64) 0;
    out << false;
    out << qCompress(data, d->comprLvl);
    out.device()->seek(0);
    out << (qint64) ( ba.size() - sizeof(qint64) );
    qint64 bo = d->socket->write(ba);
    if (bo < 0)
        return false;
    d->bytesOutTotal += bo;
    if ( d->metaOut.isValid() )
        emit uploadProgress(d->metaOut, d->bytesOutReady, d->bytesOutReady);
    d->socket->flush();
    return true;
}

//

const QDataStream::Version BSocketWrapper::DataStreamVersion = QDataStream::Qt_4_8;

//

BSocketWrapper::BSocketWrapper(BSocketWrapperPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    //
}
