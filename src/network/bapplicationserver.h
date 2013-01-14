#ifndef BAPPLICATIONSERVER_H
#define BAPPLICATIONSERVER_H

class BApplicationServerPrivate;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/BeQt>

#include <QString>
#include <QStringList>

/*============================================================================
================================ BApplicationServer ==========================
============================================================================*/

class B_NETWORK_EXPORT BApplicationServer : public BBase
{
    B_DECLARE_PRIVATE(BApplicationServer)
public:
    explicit BApplicationServer(const QString &serverName, int operationTimeout = 5 * BeQt::Second);
    ~BApplicationServer();
protected:
    explicit BApplicationServer(BApplicationServerPrivate &d);
public:
    bool isValid() const;
    bool tryListen();
    bool sendMessage(int &argc, char **argv);
    bool sendMessage( const QStringList &arguments = QStringList() );
protected:
    virtual void handleMessage(const QStringList &arguments);
private:
    Q_DISABLE_COPY(BApplicationServer)
};

#endif // BAPPLICATIONSERVER_H
