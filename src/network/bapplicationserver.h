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
    bool tryListen( const QString &serverName = QString() );
    bool sendMessage( int &argc, char **argv, const QString &serverName = QString() );
    bool sendMessage( const QStringList &arguments = QStringList(), const QString &serverName = QString() );
protected:
    BApplicationServer(BApplicationServerPrivate &d);
    //
    virtual void handleMessage(const QStringList &arguments);
private:
    Q_DISABLE_COPY(BApplicationServer)
};

#endif // BAPPLICATIONSERVER_H

