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

#ifndef BBASE_P_H
#define BBASE_P_H

class BBase;

#include "bglobal.h"

#include <QtGlobal>
#include <QObject>

/*============================================================================
================================ BBasePrivate ================================
============================================================================*/

class B_CORE_EXPORT BBasePrivate : public QObject
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BBase)
public:
    explicit BBasePrivate(BBase *q);
    virtual ~BBasePrivate();
public:
    void init();
protected:
    BBase *const _m_q;
private:
    Q_DISABLE_COPY(BBasePrivate)
};

#endif // BBASE_P_H
