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

#ifndef BTEXTCODECMENU_P_H
#define BTEXTCODECMENU_P_H

#include "btextcodecmenu.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QString>

class QSignalMapper;

/*============================================================================
================================ BTextCodecMenuPrivate =======================
============================================================================*/

class B_WIDGETS_EXPORT BTextCodecMenuPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BTextCodecMenu)
public:
    explicit BTextCodecMenuPrivate(BTextCodecMenu *q, BTextCodecMenu::Style s);
    ~BTextCodecMenuPrivate();
public:
    void init();
public Q_SLOTS:
    void retranslateUi();
    void codecSelected(const QString &cn);
public:
    const BTextCodecMenu::Style Style;
public:
    QString codecName;
    QSignalMapper *mapper;
private:
    Q_DISABLE_COPY(BTextCodecMenuPrivate)
};

#endif // BTEXTCODECMENU_P_H
