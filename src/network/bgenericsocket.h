#ifndef BGENERICSOCKET_H
#define BGENERICSOCKET_H

class QTcpSocket;
class QSslSocket;
class QUdpSocket;

#include <BeQt>

#include <QObject>
#include <QPointer>
#include <QAbstractSocket>
#include <QLocalSocket>
#include <QIODevice>
#include <QString>
#include <QByteArray>

class B_NETWORK_EXPORT BGenericSocket : public QObject
{
    Q_OBJECT
public:
    enum SocketType
    {
        NoSocket = 0x00,                    //000 0 0 0 0 0
        AbstractSocket = 0x01,              //000 0 0 0 0 1
        TcpSocket = AbstractSocket + 0x02,  //000 0 0 0 1 1
        SslSocket = TcpSocket + 0x04,       //000 0 0 1 1 1
        UdpSocket = AbstractSocket + 0x08,  //000 0 1 0 0 1
        LocalSocket = 0x10                  //000 1 0 0 0 0
    };
    //
    explicit BGenericSocket(SocketType type, QObject *parent = 0);
    //
    QIODevice *ioDevice() const;
    QAbstractSocket *abstractSocket() const;
    QTcpSocket *tcpSocket() const;
    QSslSocket *sslSocket() const;
    QUdpSocket *udpSocket() const;
    QLocalSocket *localSocket() const;
    void abort();
    qint64 bytesAvailable() const;
    qint64 bytesToWrite() const;
    bool canReadLine() const;
    void close();
    void connectToHost(const QString &hostName, quint16 port = 0, QIODevice::OpenMode openMode = QIODevice::ReadWrite);
    void disconnectFromHost();
    QAbstractSocket::SocketError error() const;
    QString errorString() const;
    bool flush();
    bool isOpen() const;
    bool isReadable() const;
    bool isSequential() const;
    bool isSocketSet() const;
    bool isTextModeEnabled() const;
    bool isValid() const;
    bool isWritable() const;
    QIODevice::OpenMode openMode() const;
    QString peerAddress() const;
    QByteArray read(qint64 maxSize);
    QByteArray readAll();
    qint64 readBufferSize() const;
    QByteArray readLine(qint64 maxSize = 0);
    void setReadBufferSize(qint64 size);
    bool setSocketDescriptor(int socketDescriptor, QIODevice::OpenMode openMode = QIODevice::ReadWrite);
    void setTextModeEnabled(bool enabled);
    int socketDescriptor() const;
    SocketType socketType() const;
    QAbstractSocket::SocketState state() const;
    bool waitForBytesWritten(int msecs);
    bool waitForConnected(int msecs = 30000);
    bool waitForDisconnected(int msecs = 30000);
    bool waitForReadyRead(int msecs);
    qint64 write(const QByteArray &byteArray);
private:
    QPointer<QAbstractSocket> _m_asocket;
    QPointer<QLocalSocket> _m_lsocket;
    //
    void _m_setSocket(QAbstractSocket *socket);
    void _m_setSocket(QLocalSocket *socket);
    void _m_connectIODevice();
    void _m_disconnectIODevice();
private slots:
    void _m_lsocketError(QLocalSocket::LocalSocketError socketError);
    void _m_lsocketStateChanged(QLocalSocket::LocalSocketState socketState);
signals:
    void aboutToClose();
    void connected();
    void bytesWritten(qint64 bytes);
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void readChannelFinished();
    void readyRead();
    void stateChanged(QAbstractSocket::SocketState socketState);
};

#endif // BGENERICSOCKET_H
