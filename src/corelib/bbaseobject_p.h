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

#ifndef BBASEOBJECT_P_H
#define BBASEOBJECT_P_H

#include "bglobal.h"
#include "bbaseobject.h"

#include <QtGlobal>
#include <QObject>

/*============================================================================
================================ BBaseObjectPrivate ==========================
============================================================================*/

class B_CORE_EXPORT BBaseObjectPrivate : public QObject
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BBaseObject)
public:
    explicit BBaseObjectPrivate(BBaseObject *q);
    virtual ~BBaseObjectPrivate();
public:
    void init();
protected:
    BBaseObject *const _m_q;
private:
    Q_DISABLE_COPY(BBaseObjectPrivate)
};

#endif // BBASEOBJECT_P_H
