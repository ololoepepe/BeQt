/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtNetwork module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef BGENERICSOCKET_H
#define BGENERICSOCKET_H

class BGenericSocketPrivate;

class QTcpSocket;
class QSslSocket;
class QUdpSocket;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/BeQt>

#include <QObject>
#include <QAbstractSocket>
#include <QLocalSocket>
#include <QIODevice>
#include <QString>
#include <QByteArray>

/*============================================================================
================================ BGenericSocket ==============================
============================================================================*/

class B_NETWORK_EXPORT BGenericSocket : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BGenericSocket)
public:
    enum SocketType
    {
        NoSocket = 0x00,                    //0 0 0 0 0 0 0 0
        AbstractSocket = 0x01,              //0 0 0 0 0 0 0 1
        TcpSocket = AbstractSocket + 0x02,  //0 0 0 0 0 0 1 1
        SslSocket = TcpSocket + 0x04,       //0 0 0 0 0 1 1 1
        UdpSocket = AbstractSocket + 0x08,  //0 0 0 0 1 0 0 1
        LocalSocket = 0x10                  //0 0 0 1 0 0 0 0
    };
public:
    explicit BGenericSocket(SocketType type, QObject *parent = 0);
    explicit BGenericSocket(QAbstractSocket *socket, QObject *parent = 0);
    explicit BGenericSocket(QLocalSocket *socket, QObject *parent = 0);
    ~BGenericSocket();
protected:
    explicit BGenericSocket(BGenericSocketPrivate &d, QObject *parent = 0);
public:
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
    quint16 peerPort() const;
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
    bool waitForConnected(int msecs = 30 * BeQt::Second);
    bool waitForDisconnected(int msecs = 30 * BeQt::Second);
    bool waitForReadyRead(int msecs = 30 * BeQt::Second);
    qint64 write(const QByteArray &byteArray);
Q_SIGNALS:
    void aboutToClose();
    void connected();
    void bytesWritten(qint64 bytes);
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void readChannelFinished();
    void readyRead();
    void stateChanged(QAbstractSocket::SocketState socketState);
private:
    Q_DISABLE_COPY(BGenericSocket)
};

#endif // BGENERICSOCKET_H
