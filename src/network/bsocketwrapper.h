#ifndef BSOCKETWRAPPER_H
#define BSOCKETWRAPPER_H

class BSocketWrapperPrivate;

class QByteArray;

#include "bgenericsocket.h"
#include "bnetworkoperationmetadata.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QDataStream>

class B_NETWORK_EXPORT BSocketWrapper : public QObject, public BBase
{
    B_DECLARE_PRIVATE(BSocketWrapper)
    Q_OBJECT
public:
    explicit BSocketWrapper(QObject *parent = 0);
    explicit BSocketWrapper(BGenericSocket *socket, QObject *parent = 0);
    explicit BSocketWrapper(BGenericSocket::SocketType type, QObject *parent = 0);
    //
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
    bool sendData( const QByteArray &data, const BNetworkOperationMetaData &metaData = BNetworkOperationMetaData() );
    //
    static const QDataStream::Version DataStreamVersion;
signals:
    void downloadProgress(const BNetworkOperationMetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void uploadProgress(const BNetworkOperationMetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void dataReceived(const QByteArray &data, const BNetworkOperationMetaData &metaData);
    void dataSent(const BNetworkOperationMetaData &metaData);
    void criticalBufferSizeReached();
protected:
    BSocketWrapper(BSocketWrapperPrivate &d);
private:
    Q_DISABLE_COPY(BSocketWrapper)
};

#endif // BSOCKETWRAPPER_H
