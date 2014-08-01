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

#ifndef BLINEEDIT_P_H
#define BLINEEDIT_P_H

#include "blineedit.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QObject>

/*============================================================================
================================ BLineEditPrivate ============================
============================================================================*/

class B_WIDGETS_EXPORT BLineEditPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BLineEdit)
public:
    bool valid;
public:
    explicit BLineEditPrivate(BLineEdit *q);
    ~BLineEditPrivate();
public:
    void init();
public Q_SLOTS:
    void textChanged();
private:
    Q_DISABLE_COPY(BLineEditPrivate)
};

#endif // BLINEEDIT_P_H
