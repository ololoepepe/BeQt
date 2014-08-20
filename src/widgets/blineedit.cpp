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

#include "blineedit.h"
#include "blineedit_p.h"

#include <QDebug>
#include <QLineEdit>
#include <QMetaObject>

/*============================================================================
================================ BLineEditPrivate ============================
============================================================================*/

/*============================== Public constructors =======================*/

BLineEditPrivate::BLineEditPrivate(BLineEdit *q) :
    BBaseObjectPrivate(q)
{
    //
}

BLineEditPrivate::~BLineEditPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BLineEditPrivate::init()
{
    valid = q_func()->hasAcceptableInput();
    connect(q_func(), SIGNAL(textChanged(QString)), this, SLOT(textChanged()));
}

/*============================== Public slots =============================*/

void BLineEditPrivate::textChanged()
{
    if (q_func()->hasAcceptableInput() == valid)
        return;
    valid = !valid;
    QMetaObject::invokeMethod(q_func(), "inputValidityChanged", Q_ARG(bool, valid));
}

/*============================================================================
================================ BLineEdit ===================================
============================================================================*/

/*============================== Public constructors =======================*/

BLineEdit::BLineEdit(QWidget *parent) :
    QLineEdit(parent), BBaseObject(*new BLineEditPrivate(this))
{
    d_func()->init();
}

BLineEdit::BLineEdit(const QString &contents, QWidget *parent) :
    QLineEdit(contents, parent), BBaseObject(*new BLineEditPrivate(this))
{
    d_func()->init();
}

BLineEdit::~BLineEdit()
{
    //
}

/*============================== Protected constructors ====================*/

BLineEdit::BLineEdit(BLineEditPrivate &d, QWidget *parent) :
    QLineEdit(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public slots =============================*/

void BLineEdit::checkValidity()
{
    d_func()->textChanged();
}
