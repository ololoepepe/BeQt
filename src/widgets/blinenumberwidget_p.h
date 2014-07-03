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

#ifndef BLINENUMBERWIDGET_P_H
#define BLINENUMBERWIDGET_P_H

class QPlainTextEdit;
class QPaintEvent;
class QRect;
class QEvent;

#include "blinenumberwidget.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QObject>

/*============================================================================
================================ BLineNumberWidgetPrivate ====================
============================================================================*/

class B_WIDGETS_EXPORT BLineNumberWidgetPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BLineNumberWidget)
public:
    explicit BLineNumberWidgetPrivate(BLineNumberWidget *q, QPlainTextEdit *edit);
    ~BLineNumberWidgetPrivate();
public:
    void init();
    bool eventFilter(QObject *o, QEvent *e);
    int getWidth() const;
    bool paintEvent(QPaintEvent *e);
public Q_SLOTS:
    void update(const QRect &rect, int dy);
    void updateArea();
    void resize();
public:
    QPlainTextEdit *const Edit;
private:
    Q_DISABLE_COPY(BLineNumberWidgetPrivate)
};

#endif // BLINENUMBERWIDGET_P_H
