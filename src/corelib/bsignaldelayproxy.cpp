#include "bsignaldelayproxy.h"
#include "bsignaldelayproxy_p.h"

#include "bglobal.h"
#include "bnamespace.h"
#include "bbase_p.h"
#include "bbase_p.h"

#include <QObject>
#include <QString>
#include <QVariant>
#include <QTimer>
#include <QMetaObject>

/*============================================================================
================================ BSignalDelayProxyPrivate ====================
============================================================================*/

/*============================== Public constructors =======================*/

BSignalDelayProxyPrivate::BSignalDelayProxyPrivate(BSignalDelayProxy *q) :
    BBasePrivate(q)
{
    //
}

BSignalDelayProxyPrivate::~BSignalDelayProxyPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BSignalDelayProxyPrivate::init()
{
    tmrIntermediate = new QTimer(this);
    connect( tmrIntermediate, SIGNAL( timeout() ), this, SLOT( timeout() ) );
    tmrMaximum = new QTimer(this);
    connect( tmrMaximum, SIGNAL( timeout() ), this, SLOT( timeout() ) );
    intermediateDelay = 500;
    maximumDelay = BeQt::Second;
}

void BSignalDelayProxyPrivate::trigger(const QVariant &data)
{
    lastData = data;
    if ( !tmrMaximum->isActive() )
        tmrMaximum->start(maximumDelay);
    tmrIntermediate->stop();
    tmrIntermediate->start(intermediateDelay);
}

/*============================== Public slots ==============================*/

void BSignalDelayProxyPrivate::timeout()
{
    B_Q(BSignalDelayProxy);
    tmrIntermediate->stop();
    tmrMaximum->stop();
    QMetaObject::invokeMethod(q, "triggered");
    switch ( lastData.type() )
    {
    case QVariant::Int:
        QMetaObject::invokeMethod( q, "triggered", Q_ARG( int, lastData.toInt() ) );
        break;
    case QVariant::String:
        QMetaObject::invokeMethod( q, "triggered", Q_ARG( QString, lastData.toString() ) );
        break;
    default:
        break;
    }
}

/*============================================================================
================================ BSignalDelayProxy ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BSignalDelayProxy::BSignalDelayProxy(QObject *parent) :
    QObject(parent), BBase( *new BSignalDelayProxyPrivate(this) )
{
    d_func()->init();
}

BSignalDelayProxy::BSignalDelayProxy(int intermediateDelay, int maximumDelay, QObject *parent) :
    QObject(parent), BBase( *new BSignalDelayProxyPrivate(this) )
{
    d_func()->init();
    setIntermediateDelay(intermediateDelay);
    setMaximumDelay(maximumDelay);
}

BSignalDelayProxy::~BSignalDelayProxy()
{
    //
}

/*============================== Protected constructors ====================*/

BSignalDelayProxy::BSignalDelayProxy(BSignalDelayProxyPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BSignalDelayProxy::setIntermediateDelay(int msecs)
{
    if (msecs < 0)
        return;
    d_func()->intermediateDelay = msecs;
}

void BSignalDelayProxy::setMaximumDelay(int msecs)
{
    if (msecs < 0)
        return;
    d_func()->maximumDelay = msecs;
}

int BSignalDelayProxy::intermediateDelay() const
{
    return d_func()->intermediateDelay;
}

int BSignalDelayProxy::maximumDelay() const
{
    return d_func()->maximumDelay;
}

void BSignalDelayProxy::setConnection(QObject *sender, const char *signal, QObject *receiver, const char *method)
{
    if (!sender || !signal || !receiver || !method)
        return;
    connect( sender, signal, this, SLOT( trigger() ) );
    connect(this, SIGNAL( triggered() ), receiver, method);
}

void BSignalDelayProxy::setIntConnection(QObject *sender, const char *signal, QObject *receiver, const char *method)
{
    if (!sender || !signal || !receiver || !method)
        return;
    connect( sender, signal, this, SLOT( trigger(int) ) );
    connect(this, SIGNAL( triggered(int) ), receiver, method);
}

void BSignalDelayProxy::setStringConnection(QObject *sender, const char *signal, QObject *receiver, const char *method)
{
    if (!sender || !signal || !receiver || !method)
        return;
    connect( sender, signal, this, SLOT( trigger(QString) ) );
    connect(this, SIGNAL( triggered(QString) ), receiver, method);
}

/*============================== Public slots ==============================*/

void BSignalDelayProxy::trigger()
{
    d_func()->trigger();
}

void BSignalDelayProxy::trigger(int number)
{
    d_func()->trigger(number);
}

void BSignalDelayProxy::trigger(const QString &string)
{
    d_func()->trigger(string);
}
