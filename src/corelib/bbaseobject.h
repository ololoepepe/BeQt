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

#ifndef BBASEOBJECT_H
#define BBASEOBJECT_H

class BBaseObjectPrivate;

#include "bglobal.h"

#include <QtGlobal>

/*============================================================================
================================ BBaseObject =================================
============================================================================*/

class B_CORE_EXPORT BBaseObject
{
    B_DECLARE_PRIVATE(BBaseObject)
protected:
    BBaseObjectPrivate *const _m_d;
public:
    explicit BBaseObject();
    virtual ~BBaseObject();
protected:
    explicit BBaseObject(BBaseObjectPrivate &d);
private:
    Q_DISABLE_COPY(BBaseObject)
};

#endif // BBASEOBJECT_H
