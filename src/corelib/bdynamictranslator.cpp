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

#include "bdynamictranslator.h"
#include "bdynamictranslator_p.h"

#include "bcoreapplication.h"
#include "btranslation.h"

#include <QByteArray>
#include <QDebug>
#include <QMetaObject>
#include <QObject>
#include <QPointer>
#include <QString>

/*============================================================================
================================ BDynamicTranslatorPrivate ===================
============================================================================*/

/*============================== Public constructors =======================*/

BDynamicTranslatorPrivate::BDynamicTranslatorPrivate(BDynamicTranslator *q) :
    BBaseObjectPrivate(q)
{
    //
}

BDynamicTranslatorPrivate::~BDynamicTranslatorPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BDynamicTranslatorPrivate::init()
{
    triggerSender = 0;
}

/*============================== Public slots ==============================*/

void BDynamicTranslatorPrivate::translate()
{
    QObject *target = q_func()->parent();
    if (!target)
        return;
    if (!targetPropertyName.isEmpty())
        target->setProperty(targetPropertyName.constData(), translation.translate());
    else if (!targetSlotName.isEmpty())
        QMetaObject::invokeMethod(target, targetSlotName.constData(), Q_ARG(QString, translation.translate()));
}

/*============================================================================
================================ BDynamicTranslator ==========================
============================================================================*/

/*!
\class BDynamicTranslator
\brief The BDynamicTranslator class provides means of dynamic translation.

This class is used to change some QObject's QString property or to invoke that QObject's slot with a QString argument
when some signal (usually BCoreApplication::languageChanged() or BApplication::languageChanged()) is emitted. Usually
some user-readable property that requires dynamic translation is changed with the help of this class. The QString is
retrieved dynamically form an associated BTranslator object.

Example:

\snippet src/corelib/bdynamictranslator.cpp 0

\note Since BDynamicTranslator is constructed with a parent parameter passed to it, it will be automatically deleted
when the parent objec tis destroyed.

\sa {Translations system}
*/

/*!
\property BDynamicTranslator::targetPropertyName

This property gets and sets the BDynamicTranslator's target property name.

By default, this property contains the property name passed to the object's constructor, or empty QByteArray, if the
other constructor was used.
*/

/*!
\property BDynamicTranslator::targetSlotName

This property gets and sets the BDynamicTranslator's target slot name.

By default, this property contains the slot name passed to the object's constructor, or empty QByteArray, if the other
constructor was used.
*/

/*!
\property BDynamicTranslator::targetType

This property gets the BDynamicTranslator's target type.

By default, this property contains either PropertyTarget or SlotTarget, depending on which constructor was used when
creating the object. The value of the property may also contain NoTarget, if an invalid QObject or it's property/slot
names were specified.
*/

/*!
\property BDynamicTranslator::translation

This property gets and sets the BTranslation object associated with the BDynamicTranslator.

By default, this property contains the translation passed to the object's constructor.
*/

/*!
\property BDynamicTranslator::triggerSender

This property gets a pointer to the QObject whose signal triggers translation.

By default, this property contains a pointer to the BCoreApplication instance (or 0, if there is no BCoreApplication).
*/

/*!
\property BDynamicTranslator::triggerSignal

This property gets the name of a signal which triggers translation.

By default, this property contains "languageChanged".
*/

/*!
\enum BDynamicTranslator::TargetType

This enum type is used to describe the target type of BDynamicTranslator.

\value NoTarget
No target. No slot will be invoked and no property will be changed.

\value PropertyTarget
QObject's property will be changed.

\value SlotTarget
QObject's slot will be invoked.
*/

/*============================== Public constructors =======================*/

/*!
Constructs an object and sets it's parent to \a parent, target property name to \a targetPropertyName, and translation
to \a t.

Trigger sender is set to a pointer to the BCoreApplication instance, and trigger signal is set to "languageChanged";

\sa setTargetPropertyName(), setTranslation(), setTrigger()
*/

BDynamicTranslator::BDynamicTranslator(QObject *parent, const QByteArray &targetPropertyName, const BTranslation &t) :
    QObject(parent), BBaseObject(*new BDynamicTranslatorPrivate(this))
{
    d_func()->init();
    setTrigger(bApp, SIGNAL(languageChanged()));
    setTargetPropertyName(targetPropertyName);
    setTranslation(t);
}

/*!
Constructs an object and sets it's parent to \a parent, target slot name to \a targetSlotName, and translation
to \a t.

Trigger sender is set to a pointer to the BCoreApplication instance, and trigger signal is set to "languageChanged";

\sa setTargetSlotName(), setTranslation(), setTrigger()
*/

BDynamicTranslator::BDynamicTranslator(QObject *parent, const BTranslation &t, const QByteArray &targetSlotName) :
    QObject(parent), BBaseObject(*new BDynamicTranslatorPrivate(this))
{
    d_func()->init();
    setTrigger(bApp, SIGNAL(languageChanged()));
    setTargetSlotName(targetSlotName);
    setTranslation(t);
}

/*!
Constructs an object and sets it's parent to \a parent, target property name to \a targetPropertyName, translation to
\a t, and calls setTrigger() with parameters \a triggerSender, and \a triggerSignal.

\sa setTargetPropertyName(), setTranslation(), setTrigger()
*/

BDynamicTranslator::BDynamicTranslator(QObject *parent, QObject *triggerSender, const QByteArray &triggerSignal,
                                       const QByteArray &targetPropertyName, const BTranslation &t) :
    QObject(parent), BBaseObject(*new BDynamicTranslatorPrivate(this))
{
    d_func()->init();
    setTrigger(triggerSender, triggerSignal);
    setTargetPropertyName(targetPropertyName);
    setTranslation(t);
}

/*!
Constructs an object and sets it's parent to \a parent, target slot name to \a targetSlotName, translation to \a t, and
calls setTrigger() with parameters \a triggerSender, and \a triggerSignal.

\sa setTargetPropertyName(), setTranslation(), setTrigger()
*/

BDynamicTranslator::BDynamicTranslator(QObject *parent, QObject *triggerSender, const QByteArray &triggerSignal,
                                       const BTranslation &t, const QByteArray &targetSlotName) :
    QObject(parent), BBaseObject(*new BDynamicTranslatorPrivate(this))
{
    d_func()->init();
    setTrigger(triggerSender, triggerSignal);
    setTargetSlotName(targetSlotName);
    setTranslation(t);
}

/*!
Destroys the object.
*/

BDynamicTranslator::~BDynamicTranslator()
{
    //
}

/*============================== Protected constructors ====================*/

/*!
Constructs an object and associates the given data object \a d with it. Object's parent is set to \a parent.
*/

BDynamicTranslator::BDynamicTranslator(BDynamicTranslatorPrivate &d, QObject *parent) :
    QObject(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BDynamicTranslator::setTargetPropertyName(const QByteArray &targetPropertyName)
{
    B_D(BDynamicTranslator);
    d->targetSlotName.clear();
    d->targetPropertyName = targetPropertyName;
}

void BDynamicTranslator::setTargetSlotName(const QByteArray &targetSlotName)
{
    B_D(BDynamicTranslator);
    d->targetPropertyName.clear();
    d->targetSlotName = targetSlotName;
}

void BDynamicTranslator::setTranslation(const BTranslation &translation)
{
    d_func()->translation = translation;
}

/*!
Connects \a {triggerSender}'s signal \a triggerSignal to the internal slot, which will in turn invoke target's slot or
change it's property.

Previously connected trigger object (if present) is disconnected from the slot.

\sa targetPropertyName, targetSlotName, targetType
*/

void BDynamicTranslator::setTrigger(QObject *triggerSender, const QByteArray &triggerSignal)
{
    B_D(BDynamicTranslator);
    if (!d->triggerSender.isNull() && !d->triggerSignal.isEmpty())
        disconnect(d->triggerSender.data(), d->triggerSignal.constData(), d, SLOT(translate()));
    d->triggerSender = triggerSender;
    d->triggerSignal = triggerSignal;
    if (triggerSender && !triggerSignal.isEmpty())
        connect(triggerSender, triggerSignal.constData(), d, SLOT(translate()));
}

QByteArray BDynamicTranslator::targetPropertyName() const
{
    return d_func()->targetPropertyName;
}

QByteArray BDynamicTranslator::targetSlotName() const
{
    return d_func()->targetSlotName;
}

BDynamicTranslator::TargetType BDynamicTranslator::targetType() const
{
    if (!d_func()->targetPropertyName.isEmpty())
        return PropertyTarget;
    else if (!d_func()->targetSlotName.isEmpty())
        return SlotTarget;
    else
        return NoTarget;
}

BTranslation BDynamicTranslator::translation() const
{
    return d_func()->translation;
}

QObject *BDynamicTranslator::triggerSender() const
{
    return d_func()->triggerSender;
}

QByteArray BDynamicTranslator::triggerSignal() const
{
    return d_func()->triggerSignal;
}
