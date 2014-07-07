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

#ifndef BPASSWORDWIDGET_P_H
#define BPASSWORDWIDGET_P_H

class BPasswordWidgetPrivate;

class QLineEdit;
class QToolButton;

#include "bpasswordwidget.h"

#include <BeQtCore/BPassword>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QObject>

/*============================================================================
================================ BPasswordWidgetPrivate ======================
============================================================================*/

class B_WIDGETS_EXPORT BPasswordWidgetPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BPasswordWidget)
public:
    int generatedLength;
    BPasswordWidget::GeneratePasswordFunction generateFunction;
    QLineEdit *ledt;
    BPassword pwd;
    BPassword pwdBackup;
    QToolButton *tbtnGenerate;
    QToolButton *tbtnSave;
    QToolButton *tbtnShow;
    bool wasEmpty;
public:
    explicit BPasswordWidgetPrivate(BPasswordWidget *q);
    ~BPasswordWidgetPrivate();
public:
    static QString defaultGeneratePasswordFunction(int len);
public:
    void init();
    void updateEdit();
public Q_SLOTS:
    void passwordChanged(const QString &password);
    void resetSave(bool b);
    void resetShow(bool b);
    void retranslateUi();
private:
    Q_DISABLE_COPY(BPasswordWidgetPrivate)
};

#endif // BPASSWORDWIDGET_P_H
