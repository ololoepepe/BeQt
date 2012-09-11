#ifndef BSOCKETSHELL_H
#define BSOCKETSHELL_H

class QAbstractSocket;
class QLocalSocket;
class QIODevice;

#include <QObject>
#include <QDataStream>
#include <QByteArray>

#if defined(BNETWORK_LIBRARY)
#  define BNETWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BNETWORKSHARED_EXPORT Q_DECL_IMPORT
#endif

class BNETWORKSHARED_EXPORT BSocketShell : public QObject
{
    Q_OBJECT
public:
    typedef quint64 SzType;
    //
    explicit BSocketShell(QObject *parent = 0);
    //
    void setSocket(QAbstractSocket *socket);
    void setSocket(QLocalSocket *socket);
    void setCompressionLevel(int level);
    void setCriticalBufferSize(SzType size);
    void setCloseOnCriticalBufferSizeReached(bool close);
    bool unsetSocket();
    QIODevice *socket() const;
    int compressionLevel() const;
    SzType criticalBufferSize() const;
    bool closeOnCriticalBufferSizeReached() const;
    bool isBuisy() const;
    void sendData(const QByteArray &data);
private:
    static const QDataStream::Version _m_DSVer;
    //
    QIODevice *_m_socket;
    qint64 _m_bytesToWrite;
    SzType _m_dataSize;
    int _m_comprLvl;
    SzType _m_criticalBufferSize;
    bool _m_closeOnCriticalBufferSizeReached;
    //
    void _m_setSocket(QIODevice *socket);
    bool _m_unsetSocket();
private slots:
    void _m_bytesWritten(qint64 bytes);
    void _m_readyRead();
signals:
    void dataReceived(QByteArray data);
    void dataSent();
    void criticalBufferSizeReached();
};

#endif // BSOCKETSHELL_H
