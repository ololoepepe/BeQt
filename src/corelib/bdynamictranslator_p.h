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

#ifndef BDYNAMICTRANSLATOR_P_H
#define BDYNAMICTRANSLATOR_P_H

#include "bdynamictranslator.h"

#include "bbaseobject_p.h"
#include "btranslation.h"

#include <QObject>
#include <QPointer>
#include <QString>

/*============================================================================
================================ BDynamicTranslatorPrivate ===================
============================================================================*/

class B_CORE_EXPORT BDynamicTranslatorPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BDynamicTranslator)
public:
    QString targetPropertyName;
    QString targetSlotName;
    BTranslation translation;
    QPointer<QObject> triggerSender;
    QString triggerSignal;
public:
    explicit BDynamicTranslatorPrivate(BDynamicTranslator *q);
    ~BDynamicTranslatorPrivate();
public:
    void init();
public Q_SLOTS:
    void translate();
private:
    Q_DISABLE_COPY(BDynamicTranslatorPrivate)
};

#endif // BDYNAMICTRANSLATOR_P_H
