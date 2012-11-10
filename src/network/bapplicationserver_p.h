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

class BApplicationServerPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BApplicationServerPrivateObject(BApplicationServerPrivate *p);
    ~BApplicationServerPrivateObject();
    //
    BApplicationServerPrivate *const _m_p;
public slots:
    void newPendingConnection();
private:
    Q_DISABLE_COPY(BApplicationServerPrivateObject)
};

class BApplicationServerPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BApplicationServer)
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
    BApplicationServerPrivateObject *const _m_o;
    //
    BGenericServer *server;
private:
    Q_DISABLE_COPY(BApplicationServerPrivate)
    //
    friend class BApplicationServerPrivateObject;
};

#endif // BAPPLICATIONSERVER_P_H

