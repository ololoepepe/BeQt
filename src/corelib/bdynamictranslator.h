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

#ifndef BDYNAMICTRANSLATOR_H
#define BDYNAMICTRANSLATOR_H

class BDynamicTranslatorPrivate;

class QByteArray;

#include "bbaseobject.h"
#include "btranslation.h"

#include <QObject>

/*============================================================================
================================ BDynamicTranslator ==========================
============================================================================*/

class B_CORE_EXPORT BDynamicTranslator : public QObject, public BBaseObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray targetPropertyName READ targetPropertyName WRITE setTargetPropertyName)
    Q_PROPERTY(QByteArray targetSlotName READ targetSlotName WRITE setTargetSlotName)
    Q_PROPERTY(TargetType targetType READ targetType)
    Q_PROPERTY(BTranslation translation READ translation WRITE setTranslation)
    Q_PROPERTY(QObject * triggerSender READ triggerSender)
    Q_PROPERTY(QByteArray triggerSignal READ triggerSignal)
    Q_ENUMS(TargetType)
    B_DECLARE_PRIVATE(BDynamicTranslator)
public:
    enum TargetType
    {
        NoTarget = 0,
        PropertyTarget,
        SlotTarget
    };
public:
    explicit BDynamicTranslator(QObject *parent, const QByteArray &targetPropertyName, const BTranslation &t);
    explicit BDynamicTranslator(QObject *parent, const BTranslation &t, const QByteArray &targetSlotName);
    explicit BDynamicTranslator(QObject *parent, QObject *triggerSender, const QByteArray &triggerSignal,
                                const QByteArray &targetPropertyName, const BTranslation &t);
    explicit BDynamicTranslator(QObject *parent, QObject *triggerSender, const QByteArray &triggerSignal,
                                const BTranslation &t, const QByteArray &targetSlotName);
    ~BDynamicTranslator();
protected:
    explicit BDynamicTranslator(BDynamicTranslatorPrivate &d, QObject *parent);
public:
    void setTargetPropertyName(const QByteArray &targetPropertyName);
    void setTargetSlotName(const QByteArray &targetSlotName);
    void setTranslation(const BTranslation &translation);
    void setTrigger(QObject *triggerSender, const QByteArray &triggerSignal);
    QByteArray targetPropertyName() const;
    QByteArray targetSlotName() const;
    TargetType targetType() const;
    BTranslation translation() const;
    QObject *triggerSender() const;
    QByteArray triggerSignal() const;
private:
    Q_DISABLE_COPY(BDynamicTranslator)
};

#endif // BDYNAMICTRANSLATOR_H
