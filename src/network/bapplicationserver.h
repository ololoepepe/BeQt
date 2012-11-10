#ifndef BAPPLICATIONSERVER_H
#define BAPPLICATIONSERVER_H

class BApplicationServerPrivate;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QString>
#include <QStringList>

class BApplicationServer : public BBase
{
    B_DECLARE_PRIVATE(BApplicationServer)
public:
    BApplicationServer();
    ~BApplicationServer();
    //
    bool tryListen(const QString &serverName);
    bool sendMessage(const QString &serverName, int &argc, char **argv);
    bool sendMessage( const QString &serverName, const QStringList &arguments = QStringList() );
protected:
    BApplicationServer(BApplicationServerPrivate &d);
    //
    virtual void handleMessage(const QStringList &arguments);
private:
    Q_DISABLE_COPY(BApplicationServer)
};

#endif // BAPPLICATIONSERVER_H

