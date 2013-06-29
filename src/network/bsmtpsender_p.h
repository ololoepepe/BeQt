#ifndef BSMTPSENDER_P_H
#define BSMTPSENDER_P_H

class BSmtpSenderPrivate;

class QTextStream;

#include "bsmtpsender.h"
#include "bemail.h"
#include "bgenericsocket.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QString>
#include <QAbstractSocket>

/*============================================================================
================================ BSmtpSenderPrivate ==========================
============================================================================*/

class B_NETWORK_EXPORT BSmtpSenderPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSmtpSender)
public:
    enum Stage
    {
        Initial = 0,
        Auth,
        AuthLogin,
        AuthPassword,
        Sender,
        Receiver,
        Data,
        Mail,
        Quit
    };
public:
    explicit BSmtpSenderPrivate(BSmtpSender *q);
    ~BSmtpSenderPrivate();
public:
    void init();
    void send(QTextStream &stream, const QString &what);
    void handleInitial(QTextStream &stream);
    void handleAuth(QTextStream &stream);
    void handleAuthLogin(QTextStream &stream);
    void handleAuthPassword(QTextStream &stream);
    void handleSender(QTextStream &stream);
    void handleReceiver(QTextStream &stream);
    void handleData(QTextStream &stream);
    void handleMail(QTextStream &stream);
    void handleQuit(QTextStream &stream);
public Q_SLOTS:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError e);
    void readyRead();
public:
    QString address;
    quint16 port;
    BGenericSocket::SocketType socketType;
    QString hostName;
    QString login;
    QString password;
    BEmail email;
    bool buisy;
    bool success;
    QString err;
    BGenericSocket *socket;
    int stage;
    int receiver;
private:
    Q_DISABLE_COPY(BSmtpSenderPrivate)
};

#endif // BSMTPSENDER_P_H
