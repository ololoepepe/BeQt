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

#ifndef BLINEEDIT_H
#define BLINEEDIT_H

class BLineEditPrivate;

class QString;
class QWidget;

#include <BeQtCore/BBaseObject>

#include <QLineEdit>

/*============================================================================
================================ BLineEdit ===================================
============================================================================*/

class B_WIDGETS_EXPORT BLineEdit : public QLineEdit, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BLineEdit)
public:
    explicit BLineEdit(QWidget *parent = 0);
    explicit BLineEdit(const QString &contents, QWidget *parent = 0);
    ~BLineEdit();
protected:
    explicit BLineEdit(BLineEditPrivate &d, QWidget *parent = 0);
public Q_SLOTS:
    void checkValidity();
Q_SIGNALS:
    void inputValidityChanged(bool b);
private:
    Q_DISABLE_COPY(BLineEdit)
};

#endif // BLINEEDIT_H
