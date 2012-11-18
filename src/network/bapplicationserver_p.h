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
================================ Application Server Private
============================================================================*/

class BApplicationServerPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BApplicationServer)
    Q_OBJECT
public:
    static const int OperationTimeout;
    static const QDataStream::Version DSVersion;
    //
    explicit BApplicationServerPrivate(BApplicationServer *q);
    ~BApplicationServerPrivate();
    //
    BGenericServer *server;
public slots:
    void newPendingConnection();
private:
    Q_DISABLE_COPY(BApplicationServerPrivate)
};

#endif // BAPPLICATIONSERVER_P_H

