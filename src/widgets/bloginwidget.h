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

#ifndef BLOGINWIDGET_H
#define BLOGINWIDGET_H

class BLoginWidgetPrivate;
class BPassword;
class BTranslation;

class QString;
class QStringList;
class QValidator;
class QByteArray;

#include "bpasswordwidget.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBaseObject>

#include <QObject>
#include <QWidget>

/*============================================================================
================================ BLoginWidget ================================
============================================================================*/

class B_WIDGETS_EXPORT BLoginWidget : public QWidget, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BLoginWidget)
public:
    enum AddressType
    {
        NoAddress = 0,
        SimpleAddress,
        StaticComboAddress,
        EditableComboAddress
    };
    enum PasswordType
    {
        NoPassword = 0,
        SimplePassword,
        SecurePassword
    };
public:
    explicit BLoginWidget(QWidget *parent = 0);
    ~BLoginWidget();
protected:
    explicit BLoginWidget(BLoginWidgetPrivate &d, QWidget *parent = 0);
public:
    void setAddressType(AddressType t, bool required = true);
    void setPortEnabled(bool b, bool required = true);
    void setLoginEnabled(bool b, bool required = true);
    void setPasswordType(PasswordType t, bool required = true);
    void setAddressRequired(bool b);
    void setPortRequired(bool b);
    void setLoginRequired(bool b);
    void setPasswordRequired(bool b);
    void setAddressLabel(const BTranslation &t);
    void setPortLabel(const BTranslation &t);
    void setLoginLabel(const BTranslation &t);
    void setPasswordLabelconst (BTranslation &t);
    void setAvailableAddresses(const QStringList &list);
    void setPersistentAddress(const QString &a);
    void setPersistentAddresses(const QStringList &list);
    void setAddressInputMask(const QString &mask);
    void setAddressValidator(QValidator *v);
    void setAddressPlaceholderText(const BTranslation &t);
    void setAddress(const QString &a);
    void setPortInputMask(const QString &mask);
    void setPortPlaceholderText(const BTranslation &t);
    void setPort(int port);
    void setLoginInputMask(const QString &mask);
    void setLoginValidator(QValidator *v);
    void setLoginPlaceholderText(const BTranslation &t);
    void setLogin(const QString &l);
    void setPasswordPlaceholderText(const BTranslation &t);
    void setPassword(const QString &pwd);
    void setPassword(const BPassword &pwd);
    void setSavePasswordVisible(bool visible);
    void setShowPasswordVisible(bool visible);
    void setGeneratePasswordVisible(bool visible);
    void setGeneratePasswordFunction(BPasswordWidget::GeneratePasswordFunction f);
    void setGeneratedPasswordLength(int len);
    void restorePasswordWidgetState(const QByteArray &state);
    QByteArray savePasswordWidgetState() const;
    AddressType addressType() const;
    bool portEnabled() const;
    bool loginEnabled() const;
    PasswordType passwordType() const;
    QStringList availableAddresses() const;
    QString address() const;
    int port() const;
    QString login() const;
    QString simplePassword() const;
    BPassword securePassword() const;
    bool hasValidInput() const;
Q_SIGNALS:
    void inputValidityChanged(bool b);
private:
    Q_DISABLE_COPY(BLoginWidget)
};

#endif // BLOGINWIDGET_H
