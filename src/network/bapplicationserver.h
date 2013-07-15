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
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    explicit BApplicationServer(quint16 port, int operationTimeout = 5 * BeQt::Second);
#else
    explicit BApplicationServer(const QString &serverName, int operationTimeout = 5 * BeQt::Second);
#endif
    explicit BApplicationServer(const QString &serverName, quint16 port, int operationTimeout = 5 * BeQt::Second);
    ~BApplicationServer();
protected:
    explicit BApplicationServer(BApplicationServerPrivate &d);
public:
    bool isValid() const;
    bool testServer() const;
    bool listen();
    bool sendMessage(int &argc, char **argv);
    bool sendMessage( const QStringList &arguments = QStringList() );
protected:
    virtual void handleMessage(const QStringList &arguments);
private:
    Q_DISABLE_COPY(BApplicationServer)
};

#endif // BAPPLICATIONSERVER_H
