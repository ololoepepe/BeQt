#ifndef BNETWORKCONNECTON_H
#define BNETWORKCONNECTON_H

class BSocketShell;

#include <QObject>
#include <QByteArray>
#include <QDataStream>
#include <QString>
#include <QMap>
#include <QAbstractSocket>

#if defined(BNETWORK_LIBRARY)
#  define BNETWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BNETWORKSHARED_EXPORT Q_DECL_IMPORT
#endif

class BNETWORKSHARED_EXPORT BNetworkConnection : public QObject
{
    Q_OBJECT
public:
    enum OperationType
    {
        Invalid = 0,
        Request = 1,
        Reply = 2
    };
    //
    static const QDataStream::Version DefaultDataStreamVersion;
    static const QString AuthorizeOperation;
    //
    static bool checkSocket(QAbstractSocket *socket);
    static bool checkInDataStream(const QDataStream &in);
    static bool checkInOutDataStreams(const QDataStream &in, const QDataStream &out);
    //
    explicit BNetworkConnection(QAbstractSocket *socket, int authorizationTimeout = 10000, QObject *parent = 0);
    //
    void setDataStreamVersion(QDataStream::Version version);
    void setCriticalBufferSize(quint64 size);
    void setCloseOnCriticalBufferSizeReached(bool close);
    bool connectToHost(const QString &hostName, quint16 port,
                       const QString &login, const QString &password, int timeout = 5000);
    bool disconnectFromHost(int timeout = 5000);
    bool isValid() const;
    bool isConnected() const;
    bool isAuthorized() const;
    QAbstractSocket::SocketError error() const;
    QString errorString() const;
    QDataStream::Version dataStreamVersion() const;
    quint64 criticalBufferSize() const;
    bool closeOnCriticalBufferSizeReached() const;
    QString address() const;
protected:
    typedef void (BNetworkConnection::*RequestHandler)(QDataStream &, QDataStream &);
    typedef void (BNetworkConnection::*ReplyHandler)(bool, QDataStream &);
    typedef void (BNetworkConnection::*Logger)(const QString &);
    //
    void addRequestHandler(const QString &id, RequestHandler handler);
    void addReplyHandler(const QString &id, ReplyHandler handler);
    void setLogger(Logger logger);
    void log(const QString &text);
    bool sendData(OperationType type, const QString &operation, const QByteArray &data);
    virtual bool authorize(const QString &login, const QString &password);
private:
    QDataStream::Version _m_dataStreamVersion;
    QAbstractSocket *_m_socket;
    BSocketShell *_m_socketShell;
    QMap<QString, RequestHandler> _m_requestHandlers;
    QMap<QString, ReplyHandler> _m_replyHandlers;
    int _m_lastOperationType;
    QString _m_lastOperation;
    Logger _m_logger;
    bool _m_authorized;
    QString _m_address;
    //
    void _m_handleAuthorizationRequest(QDataStream &in, QDataStream &out);
    void _m_handleAuthorizationReply(bool success, QDataStream &in);
private slots:
    void _m_checkAuthorization();
    void _m_disconnected();
    void _m_error(QAbstractSocket::SocketError error);
    void _m_dataReceived(const QByteArray &data);
    void _m_dataSent();
signals:
    void authorized();
    void authorizationFailed();
    void disconnected();
    void errorOccured(const QString &err);
    void requestSent(const QString &operation);
    void replySent(const QString &operation);
    void criticalBufferSizeReached();
};

#endif // BNETWORKCONNECTON_H
