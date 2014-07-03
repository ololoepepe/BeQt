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

#ifndef BSOCKETWRAPPER_H
#define BSOCKETWRAPPER_H

class BSocketWrapperPrivate;

class QByteArray;
class QVariant;

#include "bgenericsocket.h"
#include "bnetworkoperationmetadata.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBaseObject>

#include <QObject>

/*============================================================================
================================ BSocketWrapper ==============================
============================================================================*/

class B_NETWORK_EXPORT BSocketWrapper : public QObject, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSocketWrapper)
public:
    explicit BSocketWrapper(QObject *parent = 0);
    explicit BSocketWrapper(BGenericSocket *socket, QObject *parent = 0);
    explicit BSocketWrapper(BGenericSocket::SocketType type, QObject *parent = 0);
    ~BSocketWrapper();
protected:
    explicit BSocketWrapper(BSocketWrapperPrivate &d, QObject *parent = 0);
public:
    void setSocket(BGenericSocket *socket);
    void setCompressionLevel(int level);
    void setCriticalBufferSize(qint64 size);
    void setCloseOnCriticalBufferSize(bool close);
    bool unsetSocket();
    BGenericSocket *socket() const;
    int compressionLevel() const;
    qint64 criticalBufferSize() const;
    bool closeOnCriticalBufferSize() const;
    bool isBuisy() const;
    bool sendData(const QByteArray &data, const BNetworkOperationMetaData &metaData = BNetworkOperationMetaData());
    bool sendData(const QVariant &variant, const BNetworkOperationMetaData &metaData = BNetworkOperationMetaData());
    bool sendData(const QByteArray &data, int compressionLevel,
                  const BNetworkOperationMetaData &metaData = BNetworkOperationMetaData());
    bool sendData(const QVariant &variant, int compressionLevel,
                  const BNetworkOperationMetaData &metaData = BNetworkOperationMetaData());
Q_SIGNALS:
    void downloadProgress(const BNetworkOperationMetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void uploadProgress(const BNetworkOperationMetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void dataReceived(const QByteArray &data, const BNetworkOperationMetaData &metaData);
    void dataSent(const BNetworkOperationMetaData &metaData);
    void criticalBufferSizeReached();
private:
    Q_DISABLE_COPY(BSocketWrapper)
};

#endif // BSOCKETWRAPPER_H
