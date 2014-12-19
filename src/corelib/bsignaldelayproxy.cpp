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

#include "bsignaldelayproxy.h"
#include "bsignaldelayproxy_p.h"

#include "bbaseobject.h"
#include "bbaseobject_p.h"
#include "bnamespace.h"

#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariant>

/*============================================================================
================================ BSignalDelayProxyPrivate ====================
============================================================================*/

/*============================== Public constructors =======================*/

BSignalDelayProxyPrivate::BSignalDelayProxyPrivate(BSignalDelayProxy *q) :
    BBaseObjectPrivate(q)
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
    connect(&tmrIntermediate, SIGNAL(timeout()), this, SLOT(timeout()));
    connect(&tmrMaximum, SIGNAL(timeout()), this, SLOT(timeout()));
    intermediateDelay = 500;
    maximumDelay = BeQt::Second;
}

void BSignalDelayProxyPrivate::trigger(const QVariant &data)
{
    lastData = data;
    if (!tmrMaximum.isActive())
        tmrMaximum.start(maximumDelay);
    tmrIntermediate.stop();
    tmrIntermediate.start(intermediateDelay);
}

/*============================== Public slots ==============================*/

void BSignalDelayProxyPrivate::timeout()
{
    B_Q(BSignalDelayProxy);
    tmrIntermediate.stop();
    tmrMaximum.stop();
    QMetaObject::invokeMethod(q, "triggered");
    switch (lastData.type()) {
    case QVariant::Int:
        QMetaObject::invokeMethod(q, "triggered", Q_ARG(int, lastData.toInt()));
        break;
    case QVariant::String:
        QMetaObject::invokeMethod(q, "triggered", Q_ARG(QString, lastData.toString()));
        break;
    default:
        break;
    }
}

/*============================================================================
================================ BSignalDelayProxy ===========================
============================================================================*/

/*!
\class BSignalDelayProxy
\inmodule BeQtCore
\brief The BSignalDelayProxy class provides a proxy for signals and slots system.

The BSignalDelayProxy is used to proxy some signal, i.e. create a delay between that signal emission and the target
slot/signal activation.

Example:

\snippet src/corelib/bsignaldelayproxy.cpp 0

In the example above, we have a QAbstractItemModel-based model with huge amount of data, and a
QSortFilterProxyModel-based filter model with an O(n) sorting algorythm. The data is shown in a QTableView, and the
data is somehow sorted when a user types some text in a QLineEdit.

Without BSignalDelayProxy, the data would be sorted every time a user types or deletes a single character, which will
obviously lead to pure performance due to the amount of data and the sorting algorythm complexity. This would happen
if we connect the textChanged() signal of the line edit to the setSortingPattern() slot of the sorting model directly.

But if we use BSignalDelayProxy as a mediator, the setSortingPattern() slot will be triggered no more than 1 time a
second (1000 milliseconds). A user may enter some 4-6 characters during this delay, and the slot will be triggered only
once. Here, 1000 milliseconds is the so called \e intermediate delay, i.e. the minimum delay between the first signal
emission and the target method invocation.

On the other hand, even if the user continues to enter text for more than 2 seconds (2000 milliseconds), the slot will
be triggered anyway. Here, 2000 milliseconds is the \e maximum delay between the first signal emission and the target
method invocation.
*/

/*!
\property BSignalDelayProxy::intermediateDelay

This property gets and sets the minimum delay between the first signal emission and the target method invocation of the
BSignalDelayProxy.

By default, this property is 500 milliseconds.
*/

/*!
\property BSignalDelayProxy::maximumDelay

This property gets the maximum delay between the first signal emission and the target method invocation of the
BSignalDelayProxy.

By default, this property is 1000 milliseconds.
*/

/*!
\fn BSignalDelayProxy::triggered()

This signal is emitted at least intermediateDelay milliseconds after the first trigger() slot invocation and no more
than maximumDelay milliseconds after the first trigger() slot invocation.
*/

/*!
\fn BSignalDelayProxy::triggered(int number)

This signal is emitted at least intermediateDelay milliseconds after the first trigger() slot invocation and no more
than maximumDelay milliseconds after the first trigger() slot invocation.

\a number is the argument with which the slot was invoked.
*/

/*!
\fn BSignalDelayProxy::triggered(const QString &string)

This signal is emitted at least intermediateDelay milliseconds after the first trigger() slot invocation and no more
than maximumDelay milliseconds after the first trigger() slot invocation.

\a string is the argument with which the slot was invoked.
*/

/*============================== Public constructors =======================*/

/*!
Constructs a signal delay proxy and sets it's parent to \a parent.
*/

BSignalDelayProxy::BSignalDelayProxy(QObject *parent) :
    QObject(parent), BBaseObject(*new BSignalDelayProxyPrivate(this))
{
    d_func()->init();
}

/*!
Constructs a signal delay proxy. Sets it's intermediate delay to \a intermediateDelay, maximum delay to
\a maximumDelay, and parent to \a parent.
*/

BSignalDelayProxy::BSignalDelayProxy(int intermediateDelay, int maximumDelay, QObject *parent) :
    QObject(parent), BBaseObject(*new BSignalDelayProxyPrivate(this))
{
    d_func()->init();
    setIntermediateDelay(intermediateDelay);
    setMaximumDelay(maximumDelay);
}

/*!
Destroys the object, deleting the associated data object.
*/

BSignalDelayProxy::~BSignalDelayProxy()
{
    //
}

/*============================== Protected constructors ====================*/

/*!
Constructs an object and associates the given data object \a d with it. Parent is set to \a parent.
*/

BSignalDelayProxy::BSignalDelayProxy(BSignalDelayProxyPrivate &d, QObject *parent) :
    QObject(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

int BSignalDelayProxy::intermediateDelay() const
{
    return d_func()->intermediateDelay;
}

int BSignalDelayProxy::maximumDelay() const
{
    return d_func()->maximumDelay;
}

/*!
Connects the \a {sender}'s signal \a signal with this BSignalDelayProxy's trigger() slot, and connects this object's
triggered() signal with the \a {receiver}'s method \a method.

Signals/slots are considered having no arguments.

\sa setIntConnection(), setStringConnection()
*/

void BSignalDelayProxy::setConnection(QObject *sender, const char *signal, QObject *receiver, const char *method)
{
    if (!sender || !signal || !receiver || !method)
        return;
    connect(sender, signal, this, SLOT(trigger()));
    connect(this, SIGNAL(triggered()), receiver, method);
}

/*!
Connects the \a {sender}'s signal \a signal with this BSignalDelayProxy's trigger() slot, and connects this object's
triggered() signal with the \a {receiver}'s method \a method.

Signals/slots are considered having one int argument.

\sa setConnection(), setStringConnection()
*/

void BSignalDelayProxy::setIntConnection(QObject *sender, const char *signal, QObject *receiver, const char *method)
{
    if (!sender || !signal || !receiver || !method)
        return;
    connect(sender, signal, this, SLOT(trigger(int)));
    connect(this, SIGNAL(triggered(int)), receiver, method);
}

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

/*!
Connects the \a {sender}'s signal \a signal with this BSignalDelayProxy's trigger() slot, and connects this object's
triggered() signal with the \a {receiver}'s method \a method.

Signals/slots are considered having one QString argument.

\sa setConnection(), setIntConnection()
*/

void BSignalDelayProxy::setStringConnection(QObject *sender, const char *signal, QObject *receiver, const char *method)
{
    if (!sender || !signal || !receiver || !method)
        return;
    connect(sender, signal, this, SLOT(trigger(QString)));
    connect(this, SIGNAL(triggered(QString)), receiver, method);
}

/*============================== Public slots ==============================*/

/*!
Activates the BSignalDelayProxy internal timer.

\sa triggered()
*/

void BSignalDelayProxy::trigger()
{
    d_func()->trigger();
}

/*!
\overload
Activates the BSignalDelayProxy internal timer. The associated data is set to \a number.

\sa triggered()
*/

void BSignalDelayProxy::trigger(int number)
{
    d_func()->trigger(number);
}

/*!
\overload
Activates the BSignalDelayProxy internal timer. The associated data is set to \a string.

\sa triggered()
*/

void BSignalDelayProxy::trigger(const QString &string)
{
    d_func()->trigger(string);
}
