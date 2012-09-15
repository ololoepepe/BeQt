#ifndef BNETWORKCONNECTON_H
#define BNETWORKCONNECTON_H

#include "bgenericsocket.h"
#include "bnetworkoperation.h"
#include "bsocketwrapper.h"

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QAbstractSocket>
#include <QPointer>
#include <QQueue>
#include <QPair>
#include <QMap>
#include <QUuid>

#if defined(BNETWORK_LIBRARY)
#  define BNETWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BNETWORKSHARED_EXPORT Q_DECL_IMPORT
#endif

class BNETWORKSHARED_EXPORT BNetworkConnection : public QObject
{
    Q_OBJECT
public:
    explicit BNetworkConnection(BGenericSocket *socket, QObject *parent = 0);
    explicit BNetworkConnection(BGenericSocket::SocketType type, QObject *parent = 0);
    //
    void setCriticalBufferSize(qint64 size);
    void setCloseOnCriticalBufferSize(bool close);
    void setDetailedLogMode(bool enabled);
    void setAutoDeleteSentReplies(bool enabled);
    void connectToHost(const QString &hostName, quint16 port = 0);
    bool connectToHostBlocking(const QString &hostName, quint16 port = 0, int msecs = 30000);
    void disconnectFromHost();
    bool disconnectFromHostBlocking(int msecs = 30000);
    void close();
    bool isValid() const;
    bool isConnected() const;
    const QUuid &uniqueId() const;
    QAbstractSocket::SocketError error() const;
    QString errorString() const;
    qint64 criticalBufferSize() const;
    bool closeOnCriticalBufferSize() const;
    bool detailedLogMode() const;
    bool autoDeleteSentReplies() const;
    QString peerAddress() const;
    BNetworkOperation *sendRequest( const QString &operation, const QByteArray &data = QByteArray() );
protected:
    bool sendReply(BNetworkOperation *operation, const QByteArray &data);
    virtual void log(const QString &text);
private:
    typedef QPair<QByteArray, BSocketWrapper::MetaData> _m_Data;
    //
    const QUuid _m_CUniqueId;
    //
    QPointer<BGenericSocket> _m_socket;
    QPointer<BSocketWrapper> _m_socketWrapper;
    QMap<BSocketWrapper::MetaData, BNetworkOperation *> _m_requests;
    QMap<BSocketWrapper::MetaData, BNetworkOperation *> _m_replies;
    QQueue<_m_Data> _m_dataQueue;
    bool _m_detailedLog;
    bool _m_autoDelete;
    //
    void _m_init();
    void _m_setSocket(BGenericSocket *socket);
    void _m_sendNext();
    QString _m_operation(bool request) const;
private slots:
    void _m_disconnected();
    void _m_error(QAbstractSocket::SocketError socketError);
    void _m_downloadProgress(const BSocketWrapper::MetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void _m_uploadProgress(const BSocketWrapper::MetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void _m_dataReceived(const QByteArray &data, const BSocketWrapper::MetaData &metaData);
    void _m_dataSent(const BSocketWrapper::MetaData &metaData);
    void _m_operationDestroyed(QObject *object);
signals:
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void requestSent(BNetworkOperation *operation);
    void replyReceived(BNetworkOperation *operation);
    void incomingRequest(BNetworkOperation *operation);
    void requestReceived(BNetworkOperation *operation);
    void replySent(BNetworkOperation *operation);
    void criticalBufferSizeReached();
};

#endif // BNETWORKCONNECTON_H
