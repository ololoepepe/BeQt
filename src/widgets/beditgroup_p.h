/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtWidgets module of the BeQt library.
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

#ifndef BEDITGROUP_P_H
#define BEDITGROUP_P_H

class QLineEdit;

#include "beditgroup.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QMap>
#include <QObject>

/*============================================================================
================================ BEditGroupPrivate ===========================
============================================================================*/

class B_WIDGETS_EXPORT BEditGroupPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BEditGroup)
public:
    QMap<QObject *, QLineEdit *> ledtMap;
    bool match;
public:
    explicit BEditGroupPrivate(BEditGroup *q);
    ~BEditGroupPrivate();
public:
    void init();
public Q_SLOTS:
    void ledtDestroyed(QObject *object);
    void textChanged();
private:
    Q_DISABLE_COPY(BEditGroupPrivate)
};

#endif // BEDITGROUP_P_H
