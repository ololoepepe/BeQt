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

#ifndef BPASSWORDWIDGET_H
#define BPASSWORDWIDGET_H

class BPasswordWidgetPrivate;

class QByteArray;
class QString;
class QValidator;

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BPassword>

#include <QCryptographicHash>
#include <QWidget>

/*============================================================================
================================ BPasswordWidget =============================
============================================================================*/

class B_WIDGETS_EXPORT BPasswordWidget : public QWidget, public BBaseObject
{
    Q_OBJECT
    Q_PROPERTY(int generatedPasswordLength READ generatedPasswordLength WRITE setGeneratedPasswordLength)
    Q_PROPERTY(GeneratePasswordFunction generatePasswordFunction READ GeneratePasswordFunction
               WRITE setGeneratePasswordFunction)
    Q_PROPERTY(bool generatePasswordVisible READ generatePasswordVisible WRITE setGeneratePasswordVisible)
    Q_PROPERTY(bool hasAcceptableInput READ hasAcceptableInput STORED false)
    Q_PROPERTY(QString inputMask READ inputMask WRITE setInputMask)
    Q_PROPERTY(bool empty READ isEmpty STORED false)
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength)
    Q_PROPERTY(QString openPassword READ openPassword WRITE setPassword)
    Q_PROPERTY(BPassword password READ password WRITE setPassword)
    Q_PROPERTY(bool savePassword READ savePassword WRITE setSavePassword NOTIFY savePasswordChanged)
    Q_PROPERTY(bool savePasswordVisible READ savePasswordVisible WRITE setSavePasswordVisible)
    Q_PROPERTY(bool showPassword READ showPassword WRITE setShowPassword NOTIFY showPasswordChanged)
    Q_PROPERTY(bool showPasswordVisible READ showPasswordVisible WRITE setShowPasswordVisible)
    Q_PROPERTY(const QValidator * validator READ validator WRITE setValidator)
    B_DECLARE_PRIVATE(BPasswordWidget)
public:
    typedef QString (*GeneratePasswordFunction)(int length);
public:
    explicit BPasswordWidget(QWidget *parent = 0);
    explicit BPasswordWidget(const BPassword &password, QWidget *parent = 0);
    ~BPasswordWidget();
protected:
    explicit BPasswordWidget(BPasswordWidgetPrivate &d, QWidget *parent = 0);
public:
    static bool savePassword(const QByteArray &state);
    static bool showPassword(const QByteArray &state);
public:
    QCryptographicHash::Algorithm algorithm() const;
    int charCountHint(BPassword::Mode mode = BPassword::FlexibleMode) const;
    QByteArray encryptedPassword(int *charCountHint = 0) const;
    QByteArray encryptedPassword(QCryptographicHash::Algorithm a, int *charCountHint = 0) const;
    int generatedPasswordLength() const;
    GeneratePasswordFunction generatePasswordFunction() const;
    bool generatePasswordVisible() const;
    bool hasAcceptableInput() const;
    QString inputMask() const;
    bool isEmpty() const;
    int maxLength() const;
    BPassword::Mode mode() const;
    QString openPassword() const;
    BPassword password() const;
    void restorePasswordState(const QByteArray &ba);
    void restoreState(const QByteArray &ba);
    void restoreWidgetState(const QByteArray &ba);
    bool savePassword() const;
    QByteArray savePasswordState(BPassword::Mode mode = BPassword::FlexibleMode) const;
    bool savePasswordVisible() const;
    QByteArray saveState(BPassword::Mode mode = BPassword::FlexibleMode) const;
    QByteArray saveWidgetState() const;
    void setGeneratedPasswordLength(int len);
    void setGeneratePasswordFunction(GeneratePasswordFunction f);
    void setGeneratePasswordVisible(bool visible);
    void setInputMask(const QString &mask);
    void setMaxLength(int length);
    void setMode(BPassword::Mode mode);
    void setPassword(QCryptographicHash::Algorithm a, const QByteArray &password, int charCount = -1);
    void setValidator(const QValidator *v);
    bool showPassword() const;
    bool showPasswordVisible() const;
    const QValidator *validator() const;
public Q_SLOTS:
    void clear();
    void encrypt(QCryptographicHash::Algorithm a = QCryptographicHash::Sha1);
    void generatePassword();
    void setPassword(const BPassword &password);
    void setPassword(const QString &password);
    void setSavePassword(bool b);
    void setSavePasswordVisible(bool visible);
    void setShowPassword(bool b);
    void setShowPasswordVisible(bool visible);
Q_SIGNALS:
    void inputValidityChanged(bool b);
    void passwordChanged();
    void passwordChanged(const QString &password);
    void passwordChanged(const QByteArray &encryptedPassword);
    void passwordEdited(const QString &password);
    void passwordGenerated(const QString &password);
    void returnPressed();
    void savePasswordChanged(bool b);
    void showPasswordChanged(bool b);
private:
    Q_DISABLE_COPY(BPasswordWidget)
};

#endif // BPASSWORDWIDGET_H
