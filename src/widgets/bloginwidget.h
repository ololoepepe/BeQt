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

class QValidator;

#include "bpasswordwidget.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BPassword>
#include <BeQtCore/BTranslation>

#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QWidget>

/*============================================================================
================================ BLoginWidget ================================
============================================================================*/

class B_WIDGETS_EXPORT BLoginWidget : public QWidget, public BBaseObject
{
    Q_OBJECT
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(QString addressInputMask READ addressInputMask WRITE setAddressInputMask)
    Q_PROPERTY(BTranslation addressLabel READ addressLabel WRITE setAddressLabel)
    Q_PROPERTY(BTranslation addressPlaceholderText READ addressPlaceholderText WRITE setAddressPlaceholderText)
    Q_PROPERTY(bool addressRequired READ addressRequired)
    Q_PROPERTY(AddressType addressType READ addressType)
    Q_PROPERTY(const QValidator * addressValidator READ addressValidator WRITE setAddressValidator)
    Q_PROPERTY(QStringList availableAddresses READ availableAddresses WRITE setAvailableAddresses)
    Q_PROPERTY(int generatedPasswordLength READ generatedPasswordLength WRITE setGeneratedPasswordLength)
    Q_PROPERTY(BPasswordWidget::GeneratePasswordFunction generatePasswordFunction READ generatePasswordFunction
               WRITE setGeneratePasswordFunction)
    Q_PROPERTY(bool generatePasswordVisible READ generatePasswordVisible WRITE setGeneratePasswordVisible)
    Q_PROPERTY(bool hasValidInput READ hasValidInput NOTIFY inputValidityChanged STORED false)
    Q_PROPERTY(QString login READ login WRITE setLogin NOTIFY loginChanged)
    Q_PROPERTY(bool loginEnabled READ loginEnabled WRITE setLoginEnabled)
    Q_PROPERTY(QString loginInputMask READ loginInputMask WRITE setLoginInputMask)
    Q_PROPERTY(BTranslation loginLabel READ loginLabel WRITE setLoginLabel)
    Q_PROPERTY(BTranslation loginPlaceholderText READ loginPlaceholderText WRITE setLoginPlaceholderText)
    Q_PROPERTY(bool loginRequired READ loginRequired)
    Q_PROPERTY(const QValidator * loginValidator READ loginValidator WRITE setLoginValidator)
    Q_PROPERTY(QString passwordInputMask READ passwordInputMask WRITE setPasswordInputMask)
    Q_PROPERTY(BTranslation passwordLabel READ passwordLabel WRITE setPasswordLabel)
    Q_PROPERTY(BTranslation passwordPlaceholderText READ passwordPlaceholderText WRITE setPasswordPlaceholderText)
    Q_PROPERTY(bool passwordRequired READ passwordRequired)
    Q_PROPERTY(const QValidator * passwordValidator READ passwordValidator WRITE setPasswordValidator)
    Q_PROPERTY(QStringList persistentAddresses READ persistentAddresses WRITE setPersistentAddresses)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(bool portEnabled READ portEnabled WRITE setPortEnabled)
    Q_PROPERTY(QString portInputMask READ portInputMask WRITE setPortInputMask)
    Q_PROPERTY(BTranslation portLabel READ portLabel WRITE setPortLabel)
    Q_PROPERTY(BTranslation portPlaceholderText READ portPlaceholderText WRITE setPortPlaceholderText)
    Q_PROPERTY(bool portRequired READ portRequired)
    Q_PROPERTY(const QValidator * portValidator READ portValidator WRITE setPortValidator)
    Q_PROPERTY(bool savePasswordVisible READ savePasswordVisible WRITE setSavePasswordVisible)
    Q_PROPERTY(BPassword securePassword READ securePassword)
    Q_PROPERTY(bool showPasswordVisible READ showPasswordVisible WRITE setShowPasswordVisible)
    Q_PROPERTY(QString simplePassword READ simplePassword)
    Q_ENUMS(AddressType)
    Q_ENUMS(PasswordType)
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
    void setLoginEnabled(bool b, bool required = true);
    void setLoginInputMask(const QString &mask);
    void setLoginLabel(const BTranslation &t);
    void setLoginPlaceholderText(const BTranslation &t);
    void setLoginValidator(const QValidator *v);
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
