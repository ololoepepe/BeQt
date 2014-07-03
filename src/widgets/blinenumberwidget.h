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

#ifndef BLINENUMBERWIDGET_H
#define BLINENUMBERWIDGET_H

class BLineNumberWidgetPrivate;

class QPaintEvent;
class QSize;
class QPlainTextEdit;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBaseObject>

#include <QWidget>
#include <QObject>

/*============================================================================
================================ BLineNumberWidget ===========================
============================================================================*/

class B_WIDGETS_EXPORT BLineNumberWidget : public QWidget, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BLineNumberWidget)
public:
    explicit BLineNumberWidget(QPlainTextEdit *edit);
    ~BLineNumberWidget();
public:
    QSize sizeHint() const;
protected:
    void paintEvent(QPaintEvent *e);
private:
    Q_DISABLE_COPY(BLineNumberWidget)
};

#endif // BLINENUMBERWIDGET_H
