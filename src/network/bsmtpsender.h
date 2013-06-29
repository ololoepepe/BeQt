#ifndef BSMTPSENDER_H
#define BSMTPSENDER_H

class BSmtpSenderPrivate;

#include "bemail.h"
#include "bgenericsocket.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/BeQt>

#include <QObject>

/*============================================================================
================================ BSmtpSender =================================
============================================================================*/

class B_NETWORK_EXPORT BSmtpSender : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSmtpSender)
public:
    static bool send(const QString &address, const BEmail &email, int timeout = 30 * BeQt::Second,
                     const QString &userName = QString(), const QString &userPassword = QString(), quint16 port = 25,
                     BGenericSocket::SocketType type = BGenericSocket::SslSocket,
                     const QString &localHostName = QString(), QString *error = 0);
public:
    explicit BSmtpSender(QObject *parent = 0);
    ~BSmtpSender();
protected:
    explicit BSmtpSender(BSmtpSenderPrivate &d, QObject *parent = 0);
public:
    void setServer(const QString &address, quint16 port = 25);
    void setSocketType(BGenericSocket::SocketType type);
    void setLocalHostName(const QString &hostName);
    void setUser(const QString &name, const QString &password = QString());
    void setEmail(const BEmail &email);
    QString serverAddress() const;
    quint16 serverPort() const;
    BGenericSocket::SocketType socketType() const;
    QString localHostName() const;
    QString userName() const;
    QString userPassword() const;
    BEmail email() const;
    bool isValid() const;
    bool isBuisy() const;
    bool waitForFinished(int msecs = 30 * BeQt::Second);
    bool lastTransferSuccess() const;
    QString lastTransferError() const;
public Q_SLOTS:
    void send(const BEmail &email = BEmail());
Q_SIGNALS:
    void finished(bool success, const QString &error);
private:
    Q_DISABLE_COPY(BSmtpSender)
};

#endif // BSMTPSENDER_H
