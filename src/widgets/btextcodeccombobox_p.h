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

#ifndef BTEXTCODECCOMBOBOX_P_H
#define BTEXTCODECCOMBOBOX_P_H

#include "btextcodeccombobox.h"
#include "btextcodecmenu.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QObject>
#include <QString>

/*============================================================================
================================ BTextCodecComboBoxPrivate ===================
============================================================================*/

class B_WIDGETS_EXPORT BTextCodecComboBoxPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BTextCodecComboBox)
public:
    explicit BTextCodecComboBoxPrivate(BTextCodecComboBox *q, BTextCodecMenu::Style s);
    ~BTextCodecComboBoxPrivate();
public:
    void init();
public Q_SLOTS:
    void retranslateUi();
    void setCodecName(const QString &cn);
    void currentIndexChanged(int index);
public:
    const BTextCodecMenu::Style Style;
public:
    QString codecName;
    BTextCodecMenu *mnu;
private:
    Q_DISABLE_COPY(BTextCodecComboBoxPrivate)
};

#endif // BTEXTCODECCOMBOBOX_P_H
