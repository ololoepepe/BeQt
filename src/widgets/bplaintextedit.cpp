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

#include "bplaintextedit.h"
#include "bplaintextedit_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QPlainTextEdit>
#include <QWidget>
#include <QMimeData>

#include <QDebug>

/*============================================================================
================================ BPlainTextEditPrivate =======================
============================================================================*/

/*============================== Public constructors =======================*/

BPlainTextEditPrivate::BPlainTextEditPrivate(BPlainTextEdit *q) :
    BBasePrivate(q)
{
    //
}

BPlainTextEditPrivate::~BPlainTextEditPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BPlainTextEditPrivate::init()
{
    drag = true;
}

/*============================================================================
================================ BPlainTextEdit ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BPlainTextEdit::BPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent), BBase( *new BPlainTextEditPrivate(this) )
{
    d_func()->init();
}

BPlainTextEdit::~BPlainTextEdit()
{
    //
}

/*============================== Protected constructors ====================*/

BPlainTextEdit::BPlainTextEdit(BPlainTextEditPrivate &d, QWidget *parent) :
    QPlainTextEdit(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BPlainTextEdit::setDragEnabled(bool b)
{
    d_func()->drag = b;
}

bool BPlainTextEdit::dragEnabled() const
{
    return d_func()->drag;
}

/*============================== Protected methods =========================*/

QMimeData *BPlainTextEdit::createMimeDataFromSelection() const
{
    return d_func()->drag ? QPlainTextEdit::createMimeDataFromSelection() : 0;
}
