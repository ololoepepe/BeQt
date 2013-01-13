#ifndef BREMOTELOGGER_H
#define BREMOTELOGGER_H

class BRemoteLoggerPrivate;
class BGenericSocket;

#include <BeQtCore/BLogger>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QString>

/*============================================================================
================================ BRemoteLogger ===============================
============================================================================*/

class BRemoteLogger : public BLogger
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BRemoteLogger)
public:
    explicit BRemoteLogger(QObject *parent = 0);
    explicit BRemoteLogger(BGenericSocket *socket, QObject *parent = 0);
    explicit BRemoteLogger(BGenericSocket *socket, const QString &fileName, QObject *parent = 0);
    explicit BRemoteLogger(const QString &hostName, quint16 port, QObject *parent = 0);
    explicit BRemoteLogger(const QString &hostName, quint16 port, const QString &fileName, QObject *parent = 0);
    ~BRemoteLogger();
protected:
    explicit BRemoteLogger(BRemoteLoggerPrivate &d);
public:
    void setLogToRemoteEnabled(bool enabled);
    void setRemote(BGenericSocket *socket);
    void setRemote(const QString &hostName, quint16 port);
    void setRemoteTimeout(int msecs);
    bool isLogToRemoteEnabled() const;
    BGenericSocket *socket() const;
    QString hostName() const;
    quint16 port() const;
    int remoteTimeout() const;
private:
    Q_DISABLE_COPY(BRemoteLogger)
};

#endif // BREMOTELOGGER_H
