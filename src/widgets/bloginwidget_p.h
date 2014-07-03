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

#ifndef BLOGINWIDGET_P_H
#define BLOGINWIDGET_P_H

class BPasswordWidget;

class QLineEdit;
class QComboBox;
class QFormLayout;
class QHBoxLayout;
class QToolButton;

#include "bloginwidget.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbaseobject_p.h>
#include <BeQtCore/BTranslation>

#include <QObject>
#include <QStringList>

/*============================================================================
================================ BLoginWidgetPrivate =========================
============================================================================*/

class B_WIDGETS_EXPORT BLoginWidgetPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BLoginWidget)
public:
    explicit BLoginWidgetPrivate(BLoginWidget *q);
    ~BLoginWidgetPrivate();
public:
    void init();
    void updateTabOrder();
public:
    bool valid;
    BLoginWidget::AddressType addressType;
    BLoginWidget::PasswordType passwordType;
    bool addressRequired;
    bool portRequired;
    bool loginRequired;
    bool passwordRequired;
    BTranslation addressLabel;
    BTranslation portLabel;
    BTranslation loginLabel;
    BTranslation passwordLabel;
    BTranslation addressPlaceholderText;
    BTranslation portPlaceholderText;
    BTranslation loginPlaceholderText;
    BTranslation passwordPlaceholderText;
    BTranslation removeAddressButtonToolTip;
    QStringList persistentAddresses;
    QFormLayout *flt;
      QLineEdit *ledtAddress;
      QHBoxLayout *hltAddress;
        QComboBox *cmboxAddress;
        QToolButton *tbtnRemoveAddress;
      QLineEdit *ledtPort;
      QLineEdit *ledtLogin;
      QLineEdit *ledtPassword;
      BPasswordWidget *pwdwgt;
public Q_SLOTS:
    void retranslateUi();
    void checkInputs();
    void cmboxAddressCurrentIndexChanged(int index);
    void removeCurrentAddress();
private:
    Q_DISABLE_COPY(BLoginWidgetPrivate)
};

#endif // BLOGINWIDGET_P_H
