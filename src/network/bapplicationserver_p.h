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
================================ Application Server Private Object
============================================================================*/

class BApplicationServerPrivateObject : public BBasePrivateObject
{
    B_DECLARE_PRIVATE_O(BApplicationServer)
    Q_OBJECT
public:
    explicit BApplicationServerPrivateObject(BApplicationServerPrivate *p);
    ~BApplicationServerPrivateObject();
public slots:
    void newPendingConnection();
private:
    Q_DISABLE_COPY(BApplicationServerPrivateObject)
};

/*============================================================================
================================ Application Server Private
============================================================================*/

class BApplicationServerPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BApplicationServer)
    B_DECLARE_OBJECT(BApplicationServer)
    Q_DECLARE_TR_FUNCTIONS(BApplicationServer)
public:
    static const int OperationTimeout;
    static const QDataStream::Version DSVersion;
    //
    explicit BApplicationServerPrivate(BApplicationServer *q);
    ~BApplicationServerPrivate();
    //
    void newPendingConnection();
    //
    BGenericServer *server;
protected:
    BApplicationServerPrivate(BApplicationServer &q, BApplicationServerPrivateObject &o);
private:
    Q_DISABLE_COPY(BApplicationServerPrivate)
};

#endif // BAPPLICATIONSERVER_P_H

