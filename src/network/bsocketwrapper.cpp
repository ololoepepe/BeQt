/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtNetwork module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "bsocketwrapper.h"
#include "bsocketwrapper_p.h"

#include "bgenericsocket.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BUuid>

#include <QAbstractSocket>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QMetaObject>
#include <QObject>
#include <QThread>
#include <QVariant>

/*============================================================================
================================ BSocketWrapperPrivate =======================
============================================================================*/

/*============================== Public constructors =======================*/

BSocketWrapperPrivate::BSocketWrapperPrivate(BSocketWrapper *q) :
    BBaseObjectPrivate(q)
{
    //
}

BSocketWrapperPrivate::~BSocketWrapperPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BSocketWrapperPrivate::init()
{
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

bool BSocketWrapperPrivate::sendData(const QByteArray &data, const BNetworkOperationMetaData &metaData)
{
    return sendData(data, comprLvl, metaData);
}

bool BSocketWrapperPrivate::sendData(const QByteArray &data, int compressionLevel,
                                     const BNetworkOperationMetaData &metaData)
{
    if (socket.isNull() || !socket->isWritable() || q_func()->isBuisy())
        return false;
    if (!metaData.isValid() && data.isEmpty())
        return true;
    if (metaData.isValid()) {
        metaOut = metaData;
        QByteArray bam;
        QDataStream outm(&bam, QIODevice::WriteOnly);
        outm.setVersion(BeQt::DataStreamVersion);
        outm << (qint64) 0;
        outm << true;
        outm << metaData.id();
        outm << metaData.isRequest();
        outm << metaData.operation();
        outm.device()->seek(0);
        outm << (qint64) (bam.size() - sizeof(qint64));
        qint64 bom = socket->write(bam);
        if (bom > 0)
            bytesOutTotal = bom;
        else
            return false;
    }
    if (compressionLevel < 0)
        compressionLevel = -1;
    else if (compressionLevel > 9)
        compressionLevel = 9;
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(BeQt::DataStreamVersion);
    out << (qint64) 0;
    out << false;
    out << qCompress(data, compressionLevel);
    out.device()->seek(0);
    out << (qint64) (ba.size() - sizeof(qint64));
    qint64 bo = socket->write(ba);
    if (bo < 0)
        return false;
    bytesOutTotal += bo;
    if (metaOut.isValid()) {
        QMetaObject::invokeMethod(q_func(), "uploadProgress", Q_ARG(BNetworkOperationMetaData, metaOut),
                                  Q_ARG(qint64, bytesOutReady), Q_ARG(qint64, bytesOutReady));
    }
    socket->flush();
    return true;
}

/*============================== Public slots ==============================*/

void BSocketWrapperPrivate::bytesWritten(qint64 bytes)
{
    B_Q(BSocketWrapper);
    bytesOutReady += bytes;
    if (metaOut.isValid()) {
        QMetaObject::invokeMethod(q, "uploadProgress", Q_ARG(BNetworkOperationMetaData, metaOut),
                                  Q_ARG(qint64, bytesOutReady), Q_ARG(qint64, bytesOutTotal));
    }
    if (bytesOutReady == bytesOutTotal) {
        QMetaObject::invokeMethod(q, "dataSent", Q_ARG(BNetworkOperationMetaData, metaOut));
        resetOut();
    }
}

void BSocketWrapperPrivate::disconnected()
{
    resetIn();
    resetOut();
}

void BSocketWrapperPrivate::error(QAbstractSocket::SocketError)
{
    //
}

void BSocketWrapperPrivate::readyRead()
{
    B_Q(BSocketWrapper);
    QDataStream in(socket->ioDevice());
    in.setVersion(BeQt::DataStreamVersion);
    forever {
        if (criticalBufferSize > 0 && socket->bytesAvailable() >= criticalBufferSize) {
            if (closeOnCriticalBufferSize)
                socket->close();
            QMetaObject::invokeMethod(q, "criticalBufferSizeReached");
            break;
        }
        if (0 == bytesInTotal) {
            if ( socket->bytesAvailable() < sizeof(qint64) )
                break;
            in >> bytesInTotal;
        }
        qint64 bytes = socket->bytesAvailable();
        if (metaIn.isValid()) {
            QMetaObject::invokeMethod(q, "downloadProgress", Q_ARG(BNetworkOperationMetaData, metaIn),
                                      Q_ARG(qint64, bytes), Q_ARG(qint64, bytesInTotal));
        }
        if (bytes < bytesInTotal)
            break;
        bool meta = false;
        in >> meta;
        if (meta) {
            BUuid id;
            bool req;
            QString op;
            in >> id;
            in >> req;
            in >> op;
            metaIn.setId(id);
            metaIn.setIsRequest(req);
            metaIn.setOperation(op);
            bytesInTotal = 0;
        } else {
            QByteArray ba;
            in >> ba;
            QMetaObject::invokeMethod(q, "dataReceived", Q_ARG( QByteArray, qUncompress(ba)),
                                      Q_ARG(BNetworkOperationMetaData, metaIn));
            resetIn();
        }
    }
}

/*============================================================================
================================ BSocketWrapper ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BSocketWrapper::BSocketWrapper(QObject *parent) :
    QObject(parent), BBaseObject(*new BSocketWrapperPrivate(this))
{
    d_func()->init();
}

BSocketWrapper::BSocketWrapper(BGenericSocket *socket, QObject *parent) :
    QObject(parent), BBaseObject(*new BSocketWrapperPrivate(this))
{
    d_func()->init();
    setSocket(socket);
}

BSocketWrapper::BSocketWrapper(BGenericSocket::SocketType type, QObject *parent) :
    QObject(parent), BBaseObject(*new BSocketWrapperPrivate(this))
{
    d_func()->init();
    setSocket( new BGenericSocket(type) );
}

BSocketWrapper::~BSocketWrapper()
{
    //
}

/*============================== Protected constructors ====================*/

BSocketWrapper::BSocketWrapper(BSocketWrapperPrivate &d, QObject *parent) :
    QObject(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

bool BSocketWrapper::closeOnCriticalBufferSize() const
{
    return d_func()->closeOnCriticalBufferSize;
}

int BSocketWrapper::compressionLevel() const
{
    return d_func()->comprLvl;
}

qint64 BSocketWrapper::criticalBufferSize() const
{
    return d_func()->criticalBufferSize;
}

bool BSocketWrapper::isBuisy() const
{
    return d_func()->bytesOutTotal > 0;
}

bool BSocketWrapper::sendData(const QByteArray &data, const BNetworkOperationMetaData &metaData)
{
    return d_func()->sendData(data, metaData);
}

bool BSocketWrapper::sendData(const QVariant &variant, const BNetworkOperationMetaData &metaData)
{
    return d_func()->sendData(BeQt::serialize(variant), metaData);
}

bool BSocketWrapper::sendData(const QByteArray &data, int compressionLevel, const BNetworkOperationMetaData &metaData)
{
    return d_func()->sendData(data, compressionLevel, metaData);
}

bool BSocketWrapper::sendData(const QVariant &variant, int compressionLevel, const BNetworkOperationMetaData &metaData)
{
    return d_func()->sendData(BeQt::serialize(variant), compressionLevel, metaData);
}

void BSocketWrapper::setCloseOnCriticalBufferSize(bool close)
{
    d_func()->closeOnCriticalBufferSize = close;
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

void BSocketWrapper::setSocket(BGenericSocket *socket)
{
    B_D(BSocketWrapper);
    if (!socket || (socket->thread() != thread()) || !socket->isSocketSet() || !unsetSocket())
        return;
    d->socket = socket;
    connect(socket, SIGNAL(bytesWritten(qint64)), d, SLOT(bytesWritten(qint64)));
    connect(socket, SIGNAL(disconnected()), d, SLOT(disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), d, SLOT(error(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(readyRead()), d, SLOT(readyRead()));
}

BGenericSocket *BSocketWrapper::socket() const
{
    return d_func()->socket.data();
}

bool BSocketWrapper::unsetSocket()
{
    B_D(BSocketWrapper);
    if (!d->socket)
        return true;
    if (isBuisy())
        return false;
    disconnect(d->socket, SIGNAL(bytesWritten(qint64)), d, SLOT(bytesWritten(qint64)));
    disconnect(d->socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    disconnect(d->socket, SIGNAL(error(QAbstractSocket::SocketError)), d, SLOT(error(QAbstractSocket::SocketError)));
    disconnect(d->socket, SIGNAL(readyRead()), d, SLOT(readyRead()));
    d->socket = 0;
    d->resetIn();
    d->resetOut();
    return true;
}
