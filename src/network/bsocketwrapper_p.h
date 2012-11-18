#ifndef BSOCKETWRAPPER_P_H
#define BSOCKETWRAPPER_P_H

class BSocketWrapperPrivate;

#include "bgenericsocket.h"
#include "bsocketwrapper.h"
#include "bnetworkoperationmetadata.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QAbstractSocket>
#include <QPointer>

class B_NETWORK_EXPORT BSocketWrapperPrivateObject : public BBasePrivateObject
{
    B_DECLARE_PRIVATE_O(BSocketWrapper)
    Q_OBJECT
public:
    explicit BSocketWrapperPrivateObject(BSocketWrapperPrivate *p);
    ~BSocketWrapperPrivateObject();
public slots:
    void bytesWritten(qint64 bytes);
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void readyRead();
private:
    Q_DISABLE_COPY(BSocketWrapperPrivateObject)
};

class B_NETWORK_EXPORT BSocketWrapperPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSocketWrapper)
    B_DECLARE_OBJECT(BSocketWrapper)
public:
    explicit BSocketWrapperPrivate(BSocketWrapper *q);
    ~BSocketWrapperPrivate();
    //
    void init();
    void resetIn();
    void resetOut();
    void bytesWritten(qint64 bytes);
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void readyRead();
    //
    QPointer<BGenericSocket> socket;
    qint64 bytesInTotal;
    BNetworkOperationMetaData metaIn;
    qint64 bytesOutTotal;
    qint64 bytesOutReady;
    BNetworkOperationMetaData metaOut;
    int comprLvl;
    qint64 criticalBufferSize;
    bool closeOnCriticalBufferSize;
protected:
    BSocketWrapperPrivate(BSocketWrapper &q, BSocketWrapperPrivateObject &o);
private:
    Q_DISABLE_COPY(BSocketWrapperPrivate)
};

#endif // BSOCKETWRAPPER_P_H
