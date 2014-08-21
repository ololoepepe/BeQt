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

/*============================== Public constructors =======================*/

BDynamicTranslator::BDynamicTranslator(QObject *parent, const QByteArray &targetPropertyName, const BTranslation &t) :
    QObject(parent), BBaseObject(*new BDynamicTranslatorPrivate(this))
{
    d_func()->init();
    setTrigger(bApp, SIGNAL(languageChanged()));
    setTargetPropertyName(targetPropertyName);
    setTranslation(t);
}

BDynamicTranslator::BDynamicTranslator(QObject *parent, const BTranslation &t, const QByteArray &targetSlotName) :
    QObject(parent), BBaseObject(*new BDynamicTranslatorPrivate(this))
{
    d_func()->init();
    setTrigger(bApp, SIGNAL(languageChanged()));
    setTargetSlotName(targetSlotName);
    setTranslation(t);
}

BDynamicTranslator::BDynamicTranslator(QObject *parent, QObject *triggerSender, const QByteArray &triggerSignal,
                                       const QByteArray &targetPropertyName, const BTranslation &t) :
    QObject(parent), BBaseObject(*new BDynamicTranslatorPrivate(this))
{
    d_func()->init();
    setTrigger(triggerSender, triggerSignal);
    setTargetPropertyName(targetPropertyName);
    setTranslation(t);
}

BDynamicTranslator::BDynamicTranslator(QObject *parent, QObject *triggerSender, const QByteArray &triggerSignal,
                                       const BTranslation &t, const QByteArray &targetSlotName) :
    QObject(parent), BBaseObject(*new BDynamicTranslatorPrivate(this))
{
    d_func()->init();
    setTrigger(triggerSender, triggerSignal);
    setTargetSlotName(targetSlotName);
    setTranslation(t);
}

BDynamicTranslator::~BDynamicTranslator()
{
    //
}

/*============================== Protected constructors ====================*/

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
