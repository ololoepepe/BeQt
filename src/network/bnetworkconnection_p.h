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

#ifndef BNETWORKCONNECTON_P_H
#define BNETWORKCONNECTON_P_H

class BLogger;
class BNetworkOperation;
class BNetworkServer;

#include "bnetworkconnection.h"

#include "bgenericsocket.h"
#include "bnetworkoperationmetadata.h"
#include "bsocketwrapper.h"

#include <BeQtCore/BUuid>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QAbstractSocket>
#include <QByteArray>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QPointer>
#include <QQueue>
#include <QString>

/*============================================================================
================================ BNetworkConnectionPrivate ===================
============================================================================*/

class B_NETWORK_EXPORT BNetworkConnectionPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BNetworkConnection)
public:
    struct Data
    {
        int compressionLevel;
        QByteArray data;
        BNetworkOperationMetaData metaData;
    };
public:
    const BUuid UniqueId;
    BNetworkServer *const Server;
public:
    bool autoDelete;
    QQueue<Data> dataQueue;
    QMap<QString, BNetworkConnection::ExternalHandler> externalReplyHandlers;
    QMap<QString, BNetworkConnection::ExternalHandler> externalRequestHandlers;
    QMap<QString, BNetworkConnection::InternalHandler> internalReplyHandlers;
    QMap<QString, BNetworkConnection::InternalHandler> internalRequestHandlers;
    BLogger *logger;
    BNetworkConnection::LoggingMode loggingMode;
    QMap<QObject *, BNetworkOperationMetaData> operations;
    QMap<BNetworkOperationMetaData, BNetworkOperation *> replies;
    QMap<BNetworkOperationMetaData, BNetworkOperation *> requests;
    QPointer<BGenericSocket> socket;
    QPointer<BSocketWrapper> socketWrapper;
    bool translations;
public:
    explicit BNetworkConnectionPrivate(BNetworkConnection *q, BNetworkServer *server = 0);
    ~BNetworkConnectionPrivate();
public:
    BNetworkOperation *createOperation(const BNetworkOperationMetaData &metaData);
    void init();
    void sendNext();
    bool sendReply(BNetworkOperation *op, const QByteArray &data);
    bool sendReply(BNetworkOperation *op, int compressionLevel, const QByteArray &data);
    BNetworkOperation *sendRequest(const QString &op, const QByteArray &data);
    BNetworkOperation *sendRequest(const QString &op, int compressionLevel, const QByteArray &data);
    void setSocket(BGenericSocket *s);
public Q_SLOTS:
    void connected();
    void dataReceived(const QByteArray &data, const BNetworkOperationMetaData &metaData);
    void dataSent(const BNetworkOperationMetaData &metaData);
    void disconnected();
    void downloadProgress(const BNetworkOperationMetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void error(QAbstractSocket::SocketError socketError);
    void logIncomingConnection();
    void operationCanceled();
    void operationDestroyed(QObject *obj);
    void uploadProgress(const BNetworkOperationMetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
private:
    Q_DISABLE_COPY(BNetworkConnectionPrivate)
};

#endif // BNETWORKCONNECTON_P_H
