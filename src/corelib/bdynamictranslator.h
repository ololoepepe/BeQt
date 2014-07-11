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

class BTranslation;

#include "bbaseobject.h"

#include <QObject>

/*============================================================================
================================ BDynamicTranslator ==========================
============================================================================*/

class B_CORE_EXPORT BDynamicTranslator : public QObject, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BDynamicTranslator)
public:
    enum TargetType
    {
        NoTarget = 0,
        PropertyTarget,
        SlotTarget
    };
public:
    explicit BDynamicTranslator(QObject *parent, const char *targetPropertyName, const BTranslation &translation);
    explicit BDynamicTranslator(QObject *parent, const BTranslation &translation, const char *targetSlotName);
    explicit BDynamicTranslator(QObject *parent, QObject *triggerSender, const char *triggerSignal,
                                const char *targetPropertyName, const BTranslation &translation);
    explicit BDynamicTranslator(QObject *parent, QObject *triggerSender, const char *triggerSignal,
                                const BTranslation &translation, const char *targetSlotName);
    ~BDynamicTranslator();
protected:
    explicit BDynamicTranslator(BDynamicTranslatorPrivate &d, QObject *parent);
public:
    void setTargetPropertyName(const char *targetPropertyName);
    void setTargetSlotName(const char *targetSlotName);
    void setTranslation(const BTranslation &translation);
    void setTrigger(QObject *triggerSender, const char *triggerSignal);
    const char *targetPropertyName() const;
    const char *targetSlotName() const;
    TargetType targetType() const;
    BTranslation translation() const;
    QObject *triggerSender() const;
    const char *triggerSignal() const;
private:
    Q_DISABLE_COPY(BDynamicTranslator)
};

#endif // BDYNAMICTRANSLATOR_H
