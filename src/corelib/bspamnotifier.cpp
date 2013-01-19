#include "bspamnotifier.h"
#include "bspamnotifier_p.h"

#include "bglobal.h"
#include "bbase.h"
#include "bbase_p.h"

#include <QObject>
#include <QTimer>
#include <QMetaObject>

/*============================================================================
================================ BSpamNotifierPrivate ========================
============================================================================*/

/*============================== Public constructors =======================*/

BSpamNotifierPrivate::BSpamNotifierPrivate(BSpamNotifier *q) :
    BBasePrivate(q)
{
    //
}

BSpamNotifierPrivate::~BSpamNotifierPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BSpamNotifierPrivate::init()
{
    timer = new QTimer(this);
    connect( timer, SIGNAL( timeout() ), this, SLOT( timeout() ) );
    interval = 0;
    limit = 0;
    count = 0;
    elapsed = 0;
}

/*============================== Public slots ==============================*/

void BSpamNotifierPrivate::timeout()
{
    count = 0;
}

void BSpamNotifierPrivate::testSpam(int dcount)
{
    if (dcount <= 0)
        return;
    count += dcount;
    if (count >= limit)
    {
        elapsed = timeElapsed();
        timer->stop();
        QMetaObject::invokeMethod( q_func(), "spammed", Q_ARG(int, elapsed) );
    }
}

int BSpamNotifierPrivate::timeElapsed() const
{
    return timer->isActive() ? ( timer->interval() - timer->remainingTime() ) : elapsed;
}

/*============================================================================
================================ BSpamNotifier ===============================
============================================================================*/

/*============================== Public constructors =======================*/

BSpamNotifier::BSpamNotifier(QObject *parent) :
    QObject(parent), BBase( *new BSpamNotifierPrivate(this) )
{
    d_func()->init();
}

BSpamNotifier::BSpamNotifier(QObject *parent, int checkInterval, int eventLimit) :
    QObject(parent), BBase( *new BSpamNotifierPrivate(this) )
{
    d_func()->init();
    setCheckInterval(checkInterval);
    setEventLimit(eventLimit);
}

BSpamNotifier::~BSpamNotifier()
{
    //
}

/*============================== Protected constructors ====================*/

BSpamNotifier::BSpamNotifier(BSpamNotifierPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BSpamNotifier::setCheckInterval(int msecs)
{
    B_D(BSpamNotifier);
    if ( msecs < 0 || d->timer->isActive() )
        return;
    d->interval = msecs;
}

void BSpamNotifier::setEventLimit(int count)
{
    B_D(BSpamNotifier);
    if ( count < 0 || d->timer->isActive() )
        return;
    d->limit = count;
}

int BSpamNotifier::checkInterval() const
{
    return d_func()->interval;
}

int BSpamNotifier::eventLimit() const
{
    return d_func()->limit;
}

int BSpamNotifier::eventCount() const
{
    return d_func()->count;
}

int BSpamNotifier::timeElapsed() const
{
    return d_func()->timeElapsed();
}

bool BSpamNotifier::isActive() const
{
    return d_func()->timer->isActive();
}

/*============================== Public slots ==============================*/

void BSpamNotifier::spam(int eventWeight)
{
    if (eventWeight <= 0)
        return;
    B_D(BSpamNotifier);
    if ( !d->timer->isActive() )
        d->timer->start(d->interval);
    d_func()->testSpam(eventWeight);
}
