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

class BNetworkConnectionPrivate;
class BNetworkOperationMetaData;
class BNetworkOperation;
class BSocketWrapper;
class BNetworkServer;
class BLogger;

class QString;
class QByteArray;

#include "bnetworkconnection.h"
#include "bgenericsocket.h"

#include <BeQtCore/private/bbaseobject_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QCoreApplication>
#include <QAbstractSocket>
#include <QPointer>
#include <QQueue>
#include <QPair>
#include <QMap>
#include <QUuid>

/*============================================================================
================================ BNetworkConnectionPrivate ===================
============================================================================*/

class B_NETWORK_EXPORT BNetworkConnectionPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BNetworkConnection)
public:
    int al;
    struct Data
    {
        QByteArray data;
        BNetworkOperationMetaData metaData;
        int compressionLevel;
    };
public:
    explicit BNetworkConnectionPrivate(BNetworkConnection *q, BNetworkServer *server = 0);
    ~BNetworkConnectionPrivate();
public:
    void init();
    void setSocket(BGenericSocket *s);
    void sendNext();
    BNetworkOperation *sendRequest(const QString &op, const QByteArray &data);
    BNetworkOperation *sendRequest(const QString &op, int compressionLevel, const QByteArray &data);
    bool sendReply(BNetworkOperation *op, const QByteArray &data);
    bool sendReply(BNetworkOperation *op, int compressionLevel, const QByteArray &data);
    BNetworkOperation *createOperation(const BNetworkOperationMetaData &metaData);
public Q_SLOTS:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void downloadProgress(const BNetworkOperationMetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void uploadProgress(const BNetworkOperationMetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void dataReceived(const QByteArray &data, const BNetworkOperationMetaData &metaData);
    void dataSent(const BNetworkOperationMetaData &metaData);
    void operationCanceled();
    void operationDestroyed(QObject *obj);
public:
    const QUuid UniqueId;
    BNetworkServer *const Server;
public:
    QPointer<BGenericSocket> socket;
    QPointer<BSocketWrapper> socketWrapper;
    QMap<BNetworkOperationMetaData, BNetworkOperation *> requests;
    QMap<BNetworkOperationMetaData, BNetworkOperation *> replies;
    QMap<QObject *, BNetworkOperationMetaData> operations;
    QQueue<Data> dataQueue;
    QMap<QString, BNetworkConnection::InternalHandler> internalReplyHandlers;
    QMap<QString, BNetworkConnection::ExternalHandler> externalReplyHandlers;
    QMap<QString, BNetworkConnection::InternalHandler> internalRequestHandlers;
    QMap<QString, BNetworkConnection::ExternalHandler> externalRequestHandlers;
    BNetworkConnection::LoggingMode loggingMode;
    bool autoDelete;
    BLogger *logger;
    bool translations;
private:
    Q_DISABLE_COPY(BNetworkConnectionPrivate)
};

#endif // BNETWORKCONNECTON_P_H
