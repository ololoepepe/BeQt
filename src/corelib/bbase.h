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

#ifndef BBASE_H
#define BBASE_H

class BBasePrivate;

#include "bglobal.h"

#include <QtGlobal>

/*============================================================================
================================ BBase =======================================
============================================================================*/

class B_CORE_EXPORT BBase
{
    B_DECLARE_PRIVATE(BBase)
protected:
    BBasePrivate *const _m_d;
public:
    explicit BBase();
    virtual ~BBase();
protected:
    explicit BBase(BBasePrivate &d);
private:
    Q_DISABLE_COPY(BBase)
};

#endif // BBASE_H
