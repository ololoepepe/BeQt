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

class QByteArray;
class QString;
class QStringList;
class QValidator;

#include "bpasswordwidget.h"

#include <BeQtCore/BBaseObject>

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
        EditableComboAddress,
        SimpleAddress,
        StaticComboAddress
    };
    enum PasswordType
    {
        NoPassword = 0,
        SecurePassword,
        SimplePassword
    };
public:
    explicit BLoginWidget(QWidget *parent = 0);
    ~BLoginWidget();
protected:
    explicit BLoginWidget(BLoginWidgetPrivate &d, QWidget *parent = 0);
public:
    QString address() const;
    QString addressInputMask() const;
    BTranslation addressLabel() const;
    BTranslation addressPlaceholderText() const;
    bool addressRequired() const;
    AddressType addressType() const;
    const QValidator *addressValidator() const;
    QStringList availableAddresses() const;
    int generatedPasswordLength() const;
    BPasswordWidget::GeneratePasswordFunction generatePasswordFunction() const;
    bool generatePasswordVisible() const;
    bool hasValidInput() const;
    QString login() const;
    bool loginEnabled() const;
    QString loginInputMask() const;
    BTranslation loginLabel() const;
    BTranslation loginPlaceholderText() const;
    bool loginRequired() const;
    const QValidator *loginValidator() const;
    QString passwordInputMask() const;
    BTranslation passwordLabel() const;
    BTranslation passwordPlaceholderText() const;
    bool passwordRequired() const;
    PasswordType passwordType() const;
    const QValidator *passwordValidator() const;
    QStringList persistentAddresses() const;
    int port() const;
    bool portEnabled() const;
    QString portInputMask() const;
    BTranslation portLabel() const;
    BTranslation portPlaceholderText() const;
    bool portRequired() const;
    const QValidator *portValidator() const;
    void restorePasswordWidgetState(const QByteArray &state);
    bool savePasswordVisible() const;
    QByteArray savePasswordWidgetState() const;
    BPassword securePassword() const;
    void setAddressInputMask(const QString &mask);
    void setAddressLabel(const BTranslation &t);
    void setAddressPlaceholderText(const BTranslation &t);
    void setAddressType(AddressType t, bool required = true);
    void setAddressValidator(const QValidator *v);
    void setGeneratedPasswordLength(int len);
    void setGeneratePasswordFunction(BPasswordWidget::GeneratePasswordFunction f);
    void setPasswordInputMask(const QString &mask);
    void setPasswordLabel(BTranslation &t);
    void setPasswordPlaceholderText(const BTranslation &t);
    void setPasswordType(PasswordType t, bool required = true);
    void setPasswordValidator(const QValidator *v);
    void setPersistentAddress(const QString &a);
    void setPersistentAddresses(const QStringList &list);
    void setPortEnabled(bool b, bool required = true);
    void setPortInputMask(const QString &mask);
    void setPortLabel(const BTranslation &t);
    void setPortPlaceholderText(const BTranslation &t);
    void setPortValidator(const QValidator *v);
    void setLoginEnabled(bool b, bool required = true);
    void setLoginInputMask(const QString &mask);
    void setLoginLabel(const BTranslation &t);
    void setLoginPlaceholderText(const BTranslation &t);
    void setLoginValidator(const QValidator *v);
    bool showPasswordVisible() const;
    QString simplePassword() const;
public Q_SLOTS:
    void setAddress(const QString &a);
    void setAddressRequired(bool b);
    void setAvailableAddresses(const QStringList &list);
    void setGeneratePasswordVisible(bool visible);
    void setLogin(const QString &l);
    void setLoginRequired(bool b);
    void setPassword(const QString &pwd);
    void setPassword(const BPassword &pwd);
    void setPasswordRequired(bool b);
    void setPort(int port);
    void setPortRequired(bool b);
    void setSavePasswordVisible(bool visible);
    void setShowPasswordVisible(bool visible);
Q_SIGNALS:
    void addressChanged(const QString &address);
    void inputValidityChanged(bool b);
    void loginChanged(const QString &login);
    void passwordChanged();
    void passwordChanged(const QString &password);
    void passwordChanged(const QByteArray &encryptedPassword);
    void passwordEdited(const QString &password);
    void portChanged(int port);
private:
    Q_DISABLE_COPY(BLoginWidget)
};

#endif // BLOGINWIDGET_H
