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

#include "btranslation.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QMetaObject>
#include <QObject>
#include <QPointer>

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
    if (!targetPropertyName.isEmpty()) {
        target->setProperty(targetPropertyName.toLatin1().constData(), translation.translate());
    } else if (!targetSlotName.isEmpty()) {
        QMetaObject::invokeMethod(target, targetSlotName.toLatin1().constData(),
                                  Q_ARG(QString, translation.translate()));
    }
}

/*============================================================================
================================ BDynamicTranslator ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BDynamicTranslator::BDynamicTranslator(QObject *parent, const char *targetPropertyName,
                                       const BTranslation &translation) :
    QObject(parent), BBaseObject(*new BDynamicTranslatorPrivate(this))
{
    d_func()->init();
    setTrigger(qApp, SIGNAL(languageChanged()));
    setTargetPropertyName(targetPropertyName);
    setTranslation(translation);
}

BDynamicTranslator::BDynamicTranslator(QObject *parent, const BTranslation &translation, const char *targetSlotName) :
    QObject(parent), BBaseObject(*new BDynamicTranslatorPrivate(this))
{
    d_func()->init();
    setTrigger(qApp, SIGNAL(languageChanged()));
    setTargetSlotName(targetSlotName);
    setTranslation(translation);
}

BDynamicTranslator::BDynamicTranslator(QObject *parent, QObject *triggerSender, const char *triggerSignal,
                                       const char *targetPropertyName, const BTranslation &translation) :
    QObject(parent), BBaseObject(*new BDynamicTranslatorPrivate(this))
{
    d_func()->init();
    setTrigger(triggerSender, triggerSignal);
    setTargetPropertyName(targetPropertyName);
    setTranslation(translation);
}

BDynamicTranslator::BDynamicTranslator(QObject *parent, QObject *triggerSender, const char *triggerSignal,
                                       const BTranslation &translation, const char *targetSlotName) :
    QObject(parent), BBaseObject(*new BDynamicTranslatorPrivate(this))
{
    d_func()->init();
    setTrigger(triggerSender, triggerSignal);
    setTargetSlotName(targetSlotName);
    setTranslation(translation);
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

void BDynamicTranslator::setTargetPropertyName(const char *targetPropertyName)
{
    B_D(BDynamicTranslator);
    d->targetSlotName.clear();
    d->targetPropertyName = targetPropertyName;
}

void BDynamicTranslator::setTargetSlotName(const char *targetSlotName)
{
    B_D(BDynamicTranslator);
    d->targetPropertyName.clear();
    d->targetSlotName = targetSlotName;
}

void BDynamicTranslator::setTranslation(const BTranslation &translation)
{
    d_func()->translation = translation;
}

void BDynamicTranslator::setTrigger(QObject *triggerSender, const char *triggerSignal)
{
    B_D(BDynamicTranslator);
    if (!d->triggerSender.isNull() && !d->triggerSignal.isEmpty())
        disconnect(d->triggerSender.data(), d->triggerSignal.toLatin1().constData(), d, SLOT(translate()));
    d->triggerSender = triggerSender;
    d->triggerSignal = triggerSignal;
    if (triggerSender && triggerSignal)
        connect(triggerSender, triggerSignal, d, SLOT(translate()));
}

const char *BDynamicTranslator::targetPropertyName() const
{
    return d_func()->targetPropertyName.toLatin1().constData();
}

const char *BDynamicTranslator::targetSlotName() const
{
    return d_func()->targetSlotName.toLatin1().constData();
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

const char *BDynamicTranslator::triggerSignal() const
{
    return d_func()->triggerSignal.toLatin1().constData();
}
