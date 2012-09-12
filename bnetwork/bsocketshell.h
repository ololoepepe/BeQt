#ifndef BSOCKETSHELL_H
#define BSOCKETSHELL_H

#include "bgenericsocket.h"

#include <QObject>
#include <QDataStream>
#include <QByteArray>
#include <QPointer>
#include <QAbstractSocket>
#include <QUuid>
#include <QString>

#if defined(BNETWORK_LIBRARY)
#  define BNETWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BNETWORKSHARED_EXPORT Q_DECL_IMPORT
#endif

class BNETWORKSHARED_EXPORT BSocketShell : public QObject
{
    Q_OBJECT
public:
    class MetaData
    {
    public:
        MetaData();
        MetaData(const MetaData &other);
        MetaData(const QUuid &id, bool request, const QString &operation);
        //
        void setId(const QUuid &id);
        void setIsRequest(bool request);
        void setOperation(const QString &operation);
        void invalidate();
        const QUuid &id() const;
        bool isRequest() const;
        const QString &operation() const;
        bool isValid() const;
        //
        MetaData &operator=(const MetaData &other);
        bool operator==(const MetaData &other) const;
        bool operator<(const MetaData &other) const;
    private:
        QUuid _m_id;
        bool _m_request;
        QString _m_operation;
    };
    //
    static const QDataStream::Version DataStreamVersion;
    //
    explicit BSocketShell(QObject *parent = 0);
    explicit BSocketShell(BGenericSocket *socket, QObject *parent = 0);
    explicit BSocketShell(BGenericSocket::SocketType type, QObject *parent = 0);
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
    bool sendData( const QByteArray &data, const MetaData &metaData = MetaData() );
private:
    QPointer<BGenericSocket> _m_socket;
    qint64 _m_bytesInTotal;
    MetaData _m_metaIn;
    qint64 _m_bytesOutTotal;
    qint64 _m_bytesOutReady;
    MetaData _m_metaOut;
    int _m_comprLvl;
    qint64 _m_criticalBufferSize;
    bool _m_closeOnCriticalBufferSize;
    //
    void _m_init();
    void _m_resetIn();
    void _m_resetOut();
private slots:
    void _m_bytesWritten(qint64 bytes);
    void _m_disconnected();
    void _m_error(QAbstractSocket::SocketError socketError);
    void _m_readyRead();
signals:
    void downloadProgress(const BSocketShell::MetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void uploadProgress(const BSocketShell::MetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void dataReceived(const QByteArray &data, const BSocketShell::MetaData &metaData);
    void dataSent(const BSocketShell::MetaData &metaData);
    void criticalBufferSizeReached();
};

#endif // BSOCKETSHELL_H
