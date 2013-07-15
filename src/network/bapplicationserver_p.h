#ifndef BAPPLICATIONSERVER_P_H
#define BAPPLICATIONSERVER_P_H

class BApplicationServerPrivate;
class BGenericServer;

#include "bapplicationserver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QString>

/*============================================================================
================================ BApplicationServerPrivate ===================
============================================================================*/

class B_NETWORK_EXPORT BApplicationServerPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BApplicationServer)
public:
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    explicit BApplicationServerPrivate(BApplicationServer *q, quint16 port, int timeout);
#else
    explicit BApplicationServerPrivate(BApplicationServer *q, const QString &serverName, int timeout);
#endif
    explicit BApplicationServerPrivate(BApplicationServer *q, const QString &serverName, quint16 port, int timeout);
    ~BApplicationServerPrivate();
public:
    void init();
public Q_SLOTS:
    void newPendingConnection();
public:
    const QString ServerName;
    const quint16 Port;
    const int OperationTimeout;
public:
    BGenericServer *server;
private:
    Q_DISABLE_COPY(BApplicationServerPrivate)
};

#endif // BAPPLICATIONSERVER_P_H
