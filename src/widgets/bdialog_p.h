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

#ifndef BDIALOG_P_H
#define BDIALOG_P_H

class QDialogButtonBox;
class QVBoxLayout;
class QWidget;

#include "bdialog.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QObject>

/*============================================================================
================================ BDialogPrivate ==============================
============================================================================*/

class B_WIDGETS_EXPORT BDialogPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BDialog)
public:
    QDialogButtonBox *dlgbbox;
    QVBoxLayout *vlt;
    QWidget *wgt;
public:
    explicit BDialogPrivate(BDialog *q);
    ~BDialogPrivate();
public:
    void init();
    void resetWidget(QWidget *w = 0);
private:
    Q_DISABLE_COPY(BDialogPrivate)
};

#endif // BDIALOG_P_H
