#ifndef BNETWORKCONNECTON_P_H
#define BNETWORKCONNECTON_P_H

class BNetworkConnectionPrivate;
class BNetworkOperationMetaData;
class BNetworkOperation;
class BSocketWrapper;

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

class B_NETWORK_EXPORT BNetworkConnectionPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BNetworkConnectionPrivateObject(BNetworkConnectionPrivate *p);
    ~BNetworkConnectionPrivateObject();
    //
    BNetworkConnectionPrivate *const _m_p;
public slots:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void downloadProgress(const BNetworkOperationMetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void uploadProgress(const BNetworkOperationMetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void dataReceived(const QByteArray &data, const BNetworkOperationMetaData &metaData);
    void dataSent(const BNetworkOperationMetaData &metaData);
    void operationDestroyed(const BNetworkOperationMetaData &metaData);
private:
    Q_DISABLE_COPY(BNetworkConnectionPrivateObject)
};

class B_NETWORK_EXPORT BNetworkConnectionPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BNetworkConnection)
    Q_DECLARE_TR_FUNCTIONS(BNetworkConnection)
public:
    typedef QPair<QByteArray, BNetworkOperationMetaData> Data;
    //
    BNetworkConnectionPrivate(BNetworkConnection *q, BGenericSocket *s);
    BNetworkConnectionPrivate(BNetworkConnection *q, BGenericSocket::SocketType type);
    ~BNetworkConnectionPrivate();
    //
    void init();
    void setSocket(BGenericSocket *s);
    void sendNext();
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void downloadProgress(const BNetworkOperationMetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void uploadProgress(const BNetworkOperationMetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void dataReceived(const QByteArray &data, const BNetworkOperationMetaData &metaData);
    void dataSent(const BNetworkOperationMetaData &metaData);
    void operationDestroyed(const BNetworkOperationMetaData &metaData);
    BNetworkOperation *createOperation(const BNetworkOperationMetaData &metaData);
    //
    BNetworkConnectionPrivateObject *const _m_o;
    //
    const QUuid UniqueId;
    //
    QPointer<BGenericSocket> socket;
    QPointer<BSocketWrapper> socketWrapper;
    QMap<BNetworkOperationMetaData, BNetworkOperation *> requests;
    QMap<BNetworkOperationMetaData, BNetworkOperation *> replies;
    QQueue<Data> dataQueue;
    bool detailedLog;
    bool autoDelete;
private:
    Q_DISABLE_COPY(BNetworkConnectionPrivate)
};

#endif // BNETWORKCONNECTON_P_H
