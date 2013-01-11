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
    explicit BApplicationServerPrivate(BApplicationServer *q, int timeout);
    ~BApplicationServerPrivate();
public:
    void init();
public slots:
    void newPendingConnection();
public:
    const int OperationTimeout;
public:
    BGenericServer *server;
private:
    Q_DISABLE_COPY(BApplicationServerPrivate)
};

#endif // BAPPLICATIONSERVER_P_H
