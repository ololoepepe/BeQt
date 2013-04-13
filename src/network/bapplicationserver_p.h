#ifndef BAPPLICATIONSERVER_P_H
#define BAPPLICATIONSERVER_P_H

class BApplicationServerPrivate;
class BGenericServer;

#include "bapplicationserver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QString>

/*============================================================================
================================ BApplicationServerPrivate ===================
============================================================================*/

class B_NETWORK_EXPORT BApplicationServerPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BApplicationServer)
public:
    static const QDataStream::Version DSVersion;
public:
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    explicit BApplicationServerPrivate(BApplicationServer *q, quint16 port, int timeout);
#else
    explicit BApplicationServerPrivate(BApplicationServer *q, const QString &serverName, int timeout);
#endif
    ~BApplicationServerPrivate();
public:
    void init();
public slots:
    void newPendingConnection();
public:
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    const quint16 Port;
#else
    const QString ServerName;
#endif
    const int OperationTimeout;
public:
    BGenericServer *server;
private:
    Q_DISABLE_COPY(BApplicationServerPrivate)
};

#endif // BAPPLICATIONSERVER_P_H
