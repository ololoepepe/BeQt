/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "bspamnotifier.h"
#include "bspamnotifier_p.h"

#include "bglobal.h"
#include "bbaseobject.h"
#include "bbaseobject_p.h"

#include <QElapsedTimer>
#include <QMetaObject>
#include <QObject>
#include <QTimer>

/*============================================================================
================================ BSpamNotifierPrivate ========================
============================================================================*/

/*============================== Public constructors =======================*/

BSpamNotifierPrivate::BSpamNotifierPrivate(BSpamNotifier *q) :
    BBaseObjectPrivate(q)
{
    //
}

BSpamNotifierPrivate::~BSpamNotifierPrivate()
{
    delete etimer;
}

/*============================== Public methods ============================*/

void BSpamNotifierPrivate::init()
{
    etimer = 0;
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    interval = 0;
    limit = 0;
    count = 0;
    elapsed = 0;
    enabled = true;
}

void BSpamNotifierPrivate::testSpam(int dcount)
{
    if (dcount <= 0)
        return;
    count += dcount;
    if (count >= limit) {
        elapsed = timeElapsed();
        timer.stop();
        if (etimer)
            etimer->invalidate();
        delete etimer;
        etimer = 0;
        QMetaObject::invokeMethod(q_func(), "spammed", Q_ARG(int, elapsed));
    }
}

int BSpamNotifierPrivate::timeElapsed() const
{
    return (timer.isActive() && etimer) ? etimer->elapsed() : elapsed;

}

/*============================== Public slots ==============================*/

void BSpamNotifierPrivate::timeout()
{
    count = 0;
}

/*============================================================================
================================ BSpamNotifier ===============================
============================================================================*/

/*!
\class BSpamNotifier
\inmodule BeQtCore
\brief The BSpamNotifier class provides means of detecting "spamming", i.e. too frequent signal invocation.

Example:

\snippet src/corelib/bspamnotifier.cpp 0

In the example above, the BSpamNotifier is used to count number of clicks in a specified time period. When the number
of clicks in a half of a second (500 milliseconds) becomes 2 or more, a message is shown.
*/

/*!
\property BSpamNotifier::active

This property gets BSpamNotifier's state.

If the internal timer has been activated by calling the spam() slot, this property is true. Otherwise it is false.

By default, this property is false.
*/

/*!
\property BSpamNotifier::checkInterval

This property gets and sets BSpamNotifier's smap check interval, i.e. the interval after which the number of spam
events (calls to spam()) is calculated.

By default, this property is 0.

The BSpamNotifier can not be used if this property is 0 or less.
*/

/*!
\property BSpamNotifier::enabled

This property gets and sets BSpamNotifier's enabled state. If the BSpamNotifier notifier is disabled, calling the
spam() slot does nothing.

By default, this property is true.
*/

/*!
\property BSpamNotifier::eventCount

This property gets the count of spam events of the last check period of the BSpamNotifier.

By default, this property is 0.
*/

/*!
\property BSpamNotifier::eventLimit

This property gets and sets BSpamNotifier's spam event limit for every checkInterval.

By default, this property is 0.

The BSpamNotifier can not be used if this property is 0 or less.
*/

/*!
\property BSpamNotifier::timeElapsed

This property gets the time (in milliseconds) that elapsed form the first spam event if the internal timer is active,
and the time that elapsed since the internal timer activation and until when the spammed() slot was called if the
internal timer is inactive.

By default, this property is .
*/

/*!
\fn BSpamNotifier::spammed(int msecsElapsed)

This signal is emitted if the number of spam events (the spam() slot invocations) in a specified period (checkInterval)
exceeds the eventLimit.

\a msecsElapsed is the time that elapsed since the internal timer activation and until when the spammed() slot was
called.
*/

/*============================== Public constructors =======================*/

/*!
Constructs a spam notifier and sets it's parent to \a parent.
*/

BSpamNotifier::BSpamNotifier(QObject *parent) :
    QObject(parent), BBaseObject(*new BSpamNotifierPrivate(this))
{
    d_func()->init();
}

/*!
Constructs a spam notifier. Sets it's check interval to \a checkInterval, event limit to \a eventLimit, and parent to
\a parent.
*/

BSpamNotifier::BSpamNotifier(QObject *parent, int checkInterval, int eventLimit) :
    QObject(parent), BBaseObject(*new BSpamNotifierPrivate(this))
{
    d_func()->init();
    setCheckInterval(checkInterval);
    setEventLimit(eventLimit);
}

/*!
Destroys the object, deleting the associated data object.
*/

BSpamNotifier::~BSpamNotifier()
{
    //
}

/*============================== Protected constructors ====================*/

/*!
Constructs an object and associates the given data object \a d with it. Parent is set to \a parent.
*/

BSpamNotifier::BSpamNotifier(BSpamNotifierPrivate &d, QObject *parent) :
    QObject(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

int BSpamNotifier::checkInterval() const
{
    return d_func()->interval;
}

int BSpamNotifier::eventCount() const
{
    return d_func()->count;
}

int BSpamNotifier::eventLimit() const
{
    return d_func()->limit;
}

bool BSpamNotifier::isActive() const
{
    return d_func()->timer.isActive();
}

bool BSpamNotifier::isEnabled() const
{
    return d_func()->enabled;
}

void BSpamNotifier::setCheckInterval(int msecs)
{
    B_D(BSpamNotifier);
    if (msecs < 0 || d->timer.isActive())
        return;
    d->interval = msecs;
}

void BSpamNotifier::setEventLimit(int count)
{
    B_D(BSpamNotifier);
    if (count < 0 || d->timer.isActive())
        return;
    d->limit = count;
}

int BSpamNotifier::timeElapsed() const
{
    return d_func()->timeElapsed();
}

/*============================== Public slots ==============================*/

void BSpamNotifier::setEnabled(bool enabled)
{
    d_func()->enabled = enabled;
}

/*!
Adds the \a eventWeight to the event counter. If the value of that counter in a specified period (checkInterval)
exceeds the eventLimit, the spammed() signal is triggered.

if \a eventWeight is less or equal to zero, this slot does nothing.
*/

void BSpamNotifier::spam(int eventWeight)
{
    if (eventWeight <= 0)
        return;
    B_D(BSpamNotifier);
    if (!d->enabled)
        return;
    if (!d->timer.isActive()) {
        d->timer.start(d->interval);
        d->etimer = new QElapsedTimer;
        d->etimer->start();
    }
    d_func()->testSpam(eventWeight);
}
