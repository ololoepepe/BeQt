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

#include <BeQtCore/private/bbase_p.h>
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

class B_NETWORK_EXPORT BNetworkConnectionPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BNetworkConnection)
public:
    typedef QPair<QByteArray, BNetworkOperationMetaData> Data;
public:
    explicit BNetworkConnectionPrivate(BNetworkConnection *q, BNetworkServer *server = 0);
    ~BNetworkConnectionPrivate();
public:
    void init();
    void setSocket(BGenericSocket *s);
    void sendNext();
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
