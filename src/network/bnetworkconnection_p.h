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

/*============================================================================
================================ Network Connection Private
============================================================================*/

class B_NETWORK_EXPORT BNetworkConnectionPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BNetworkConnection)
    Q_OBJECT
public:
    typedef QPair<QByteArray, BNetworkOperationMetaData> Data;
    //
    BNetworkConnectionPrivate(BNetworkConnection *q);
    ~BNetworkConnectionPrivate();
    //
    void init();
    void setSocket(BGenericSocket *s);
    void sendNext();
    BNetworkOperation *createOperation(const BNetworkOperationMetaData &metaData);
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
    Q_DISABLE_COPY(BNetworkConnectionPrivate)
};

#endif // BNETWORKCONNECTON_P_H
