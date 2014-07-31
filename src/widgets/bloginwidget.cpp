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

#include "bloginwidget.h"
#include "bloginwidget_p.h"

#include "bapplication.h"
#include "bguitools.h"
#include "bpasswordwidget.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BPassword>
#include <BeQtCore/BTranslation>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QByteArray>
#include <QComboBox>
#include <QDebug>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QToolButton>
#include <QValidator>
#include <QWidget>

/*============================================================================
================================ BLoginWidgetPrivate =========================
============================================================================*/

/*============================== Public constructors =======================*/

BLoginWidgetPrivate::BLoginWidgetPrivate(BLoginWidget *q) :
    BBaseObjectPrivate(q)
{
    //
}

BLoginWidgetPrivate::~BLoginWidgetPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BLoginWidgetPrivate::init()
{
    lastPort = 0;
    valid = false;
    addressType = BLoginWidget::NoAddress;
    passwordType = BLoginWidget::SecurePassword;
    addressRequired = false;
    portRequired = false;
    loginRequired = true;
    passwordRequired = true;
    addressLabel = BTranslation::translate("BLoginWidgetPrivate", "Server address:", "lbl text");
    portLabel = BTranslation::translate("BLoginWidgetPrivate", "Port:", "lbl text");
    loginLabel = BTranslation::translate("BLoginWidgetPrivate", "Login:", "lbl text");
    passwordLabel = BTranslation::translate("BLoginWidgetPrivate", "Password:", "lbl text");
    removeAddressButtonToolTip = BTranslation::translate("BLoginWidgetPrivate", "Remove current address from history",
                                                         "tbtn toolTip");
    //
    flt = new QFormLayout(q_func());
      ledtAddress = 0;
      hltAddress = 0;
      cmboxAddress = 0;
      tbtnRemoveAddress = 0;
      ledtPort = 0;
      ledtLogin = new QLineEdit;
        connect(ledtLogin, SIGNAL(textChanged(QString)), this, SLOT(checkInputs()));
        connect(ledtLogin, SIGNAL(textChanged(QString)), q_func(), SIGNAL(loginChanged(QString)));
      flt->addRow(loginLabel, ledtLogin);
      ledtPassword = 0;
      pwdwgt = new BPasswordWidget;
        connect(pwdwgt, SIGNAL(passwordChanged()), this, SLOT(checkInputs()));
        connect(pwdwgt, SIGNAL(passwordChanged()), q_func(), SIGNAL(passwordChanged()));
        connect(pwdwgt, SIGNAL(passwordChanged(QString)), q_func(), SIGNAL(passwordChanged(QString)));
        connect(pwdwgt, SIGNAL(passwordChanged(QByteArray)), q_func(), SIGNAL(passwordChanged(QByteArray)));
        connect(pwdwgt, SIGNAL(passwordEdited(QString)), q_func(), SIGNAL(passwordEdited(QString)));
      flt->addRow(passwordLabel, pwdwgt);
    //
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

void BLoginWidgetPrivate::updateTabOrder()
{
    QList<QWidget *> list;
    if (ledtAddress)
        list << ledtAddress;
    if (cmboxAddress)
        list << cmboxAddress;
    if (tbtnRemoveAddress)
        list << tbtnRemoveAddress;
    if (ledtPort)
        list << ledtPort;
    if (ledtLogin)
        list << ledtLogin;
    if (ledtPassword)
        list << ledtPassword;
    if (pwdwgt)
        list << pwdwgt;
    for (int i = 0; i < list.size() - 1; ++i)
        QWidget::setTabOrder(list.at(i), list.at(i + 1));
}

/*============================== Public slots ==============================*/

void BLoginWidgetPrivate::checkInputs()
{
    bool nvalid = true;
    if (addressRequired) {
        if (ledtAddress)
            nvalid = ledtAddress->hasAcceptableInput();
        else if (cmboxAddress && cmboxAddress->lineEdit())
            nvalid = cmboxAddress->lineEdit()->hasAcceptableInput();
        else if (cmboxAddress && cmboxAddress->currentIndex() < 0)
            nvalid = false;
    }
    if (portRequired)
        nvalid = nvalid && ledtPort && !ledtPort->text().isEmpty() && ledtPort->hasAcceptableInput();
    if (loginRequired)
        nvalid = nvalid && ledtLogin && !ledtLogin->text().isEmpty() && ledtLogin->hasAcceptableInput();
    if (passwordRequired) {
        if (ledtPassword)
            nvalid = nvalid && !ledtPassword->text().isEmpty() && ledtPassword->hasAcceptableInput();
        else if (pwdwgt)
            nvalid = nvalid && pwdwgt->password().charCountHint();
        else
            nvalid = false;
    }
    bool b = nvalid != valid;
    valid = nvalid;
    if (b)
        QMetaObject::invokeMethod(q_func(), "inputValidityChanged", Q_ARG(bool, valid));
    if (ledtPort) {
        int nport = ledtPort->text().toInt();
        if (nport != lastPort) {
            lastPort = nport;
            QMetaObject::invokeMethod(q_func(), "portChanged", Q_ARG(int, nport));
        }
    }
}

void BLoginWidgetPrivate::cmboxAddressCurrentIndexChanged(int index)
{
    if (index < 0 || !tbtnRemoveAddress)
        return;
    tbtnRemoveAddress->setEnabled(!persistentAddresses.contains(cmboxAddress->itemText(index)));
}

void BLoginWidgetPrivate::removeCurrentAddress()
{
    if (!cmboxAddress)
        return;
    QString text = cmboxAddress->currentText();
    QStringList list = q_func()->availableAddresses();
    list.removeAll(text);
    cmboxAddress->clear();
    cmboxAddress->addItems(list);
}

void BLoginWidgetPrivate::retranslateUi()
{
    if (hltAddress)
        BGuiTools::labelForField<QLabel>(hltAddress)->setText(addressLabel);
    if (ledtAddress)
        ledtAddress->setPlaceholderText(addressPlaceholderText);
    if (cmboxAddress && cmboxAddress->lineEdit())
        cmboxAddress->lineEdit()->setPlaceholderText(addressPlaceholderText);
    if (tbtnRemoveAddress)
        tbtnRemoveAddress->setToolTip(removeAddressButtonToolTip);
    if (ledtPort) {
        BGuiTools::labelForField<QLabel>(ledtPort)->setText(portLabel);
        ledtPort->setPlaceholderText(portPlaceholderText);
    }
    if (ledtLogin) {
        BGuiTools::labelForField<QLabel>(ledtLogin)->setText(loginLabel);
        ledtLogin->setPlaceholderText(loginPlaceholderText);
    }
    if (ledtPassword) {
        BGuiTools::labelForField<QLabel>(ledtPassword)->setText(passwordLabel);
        ledtPassword->setPlaceholderText(passwordPlaceholderText);
    }
    if (pwdwgt)
        BGuiTools::labelForField<QLabel>(pwdwgt)->setText(passwordLabel);
}

/*============================================================================
================================ BLoginWidget ===============================
============================================================================*/

/*============================== Public constructors =======================*/

BLoginWidget::BLoginWidget(QWidget *parent) :
    QWidget(parent), BBaseObject(*new BLoginWidgetPrivate(this))
{
    d_func()->init();
}

BLoginWidget::~BLoginWidget()
{
    //
}

/*============================== Protected constructors ====================*/

BLoginWidget::BLoginWidget(BLoginWidgetPrivate &d, QWidget *parent) :
    QWidget(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QString BLoginWidget::address() const
{
    switch (d_func()->addressType) {
    case SimpleAddress:
        return d_func()->ledtAddress->text();
    case StaticComboAddress:
    case EditableComboAddress:
        return d_func()->cmboxAddress->currentText();
    default:
        return QString();
    }
}

QString BLoginWidget::addressInputMask() const
{
    QLineEdit *ledt = d_func()->cmboxAddress ? d_func()->cmboxAddress->lineEdit() : d_func()->ledtAddress;
    return ledt ? ledt->inputMask() : QString();
}

BTranslation BLoginWidget::addressLabel() const
{
    return d_func()->addressLabel;
}

BTranslation BLoginWidget::addressPlaceholderText() const
{
    return d_func()->addressPlaceholderText;
}

bool BLoginWidget::addressRequired() const
{
    return d_func()->addressRequired;
}

BLoginWidget::AddressType BLoginWidget::addressType() const
{
    return d_func()->addressType;
}

const QValidator *BLoginWidget::addressValidator() const
{
    QLineEdit *ledt = d_func()->cmboxAddress ? d_func()->cmboxAddress->lineEdit() : d_func()->ledtAddress;
    return ledt ? ledt->validator() : 0;
}

QStringList BLoginWidget::availableAddresses() const
{
    if (!d_func()->cmboxAddress)
        return QStringList();
    QStringList list;
    foreach (int i, bRangeD(0, d_func()->cmboxAddress->count() - 1))
        list << d_func()->cmboxAddress->itemText(i);
    return list;
}

int BLoginWidget::generatedPasswordLength() const
{
    return d_func()->pwdwgt ? d_func()->pwdwgt->generatedPasswordLength() : 0;
}

BPasswordWidget::GeneratePasswordFunction BLoginWidget::generatePasswordFunction() const
{
    return d_func()->pwdwgt ? d_func()->pwdwgt->generatePasswordFunction() : 0;
}

bool BLoginWidget::generatePasswordVisible() const
{
    return d_func()->pwdwgt ? d_func()->pwdwgt->generatePasswordVisible() : false;
}

bool BLoginWidget::hasValidInput() const
{
    return d_func()->valid;
}

QString BLoginWidget::login() const
{
    return d_func()->ledtLogin ? d_func()->ledtLogin->text() : QString();
}

bool BLoginWidget::loginEnabled() const
{
    return d_func()->ledtLogin;
}

QString BLoginWidget::loginInputMask() const
{
    return d_func()->ledtLogin ? d_func()->ledtLogin->inputMask() : QString();
}

BTranslation BLoginWidget::loginLabel() const
{
    return d_func()->loginLabel;
}

BTranslation BLoginWidget::loginPlaceholderText() const
{
    return d_func()->loginPlaceholderText;
}

bool BLoginWidget::loginRequired() const
{
    return d_func()->loginRequired;
}

const QValidator *BLoginWidget::loginValidator() const
{
    return d_func()->ledtLogin ? d_func()->ledtLogin->validator() : 0;
}

QString BLoginWidget::passwordInputMask() const
{
    if (d_func()->pwdwgt)
        return d_func()->pwdwgt->inputMask();
    else if (d_func()->ledtPassword)
        return d_func()->ledtPassword->inputMask();
    else
        return QString();
}

BTranslation BLoginWidget::passwordLabel() const
{
    return d_func()->passwordLabel;
}

BTranslation BLoginWidget::passwordPlaceholderText() const
{
    return d_func()->passwordPlaceholderText;
}

bool BLoginWidget::passwordRequired() const
{
    return d_func()->passwordRequired;
}

BLoginWidget::PasswordType BLoginWidget::passwordType() const
{
    return d_func()->passwordType;
}

const QValidator *BLoginWidget::passwordValidator() const
{
    if (d_func()->pwdwgt)
        return d_func()->pwdwgt->validator();
    else if (d_func()->ledtPassword)
        return d_func()->ledtPassword->validator();
    else
        return 0;
}

QStringList BLoginWidget::persistentAddresses() const
{
    return d_func()->persistentAddresses;
}

int BLoginWidget::port() const
{
    return d_func()->ledtPort ? d_func()->ledtPort->text().toInt() : 0;
}

bool BLoginWidget::portEnabled() const
{
    return d_func()->ledtPort;
}

QString BLoginWidget::portInputMask() const
{
    return d_func()->ledtPort ? d_func()->ledtPort->inputMask() : QString();
}

BTranslation BLoginWidget::portLabel() const
{
    return d_func()->portLabel;
}

BTranslation BLoginWidget::portPlaceholderText() const
{
    return d_func()->portPlaceholderText;
}

bool BLoginWidget::portRequired() const
{
    return d_func()->portRequired;
}

const QValidator *BLoginWidget::portValidator() const
{
    return d_func()->ledtPort ? d_func()->ledtPort->validator() : 0;
}

void BLoginWidget::restorePasswordWidgetState(const QByteArray &state)
{
    if (!d_func()->pwdwgt)
        return;
    d_func()->pwdwgt->restoreWidgetState(state);
}

bool BLoginWidget::savePasswordVisible() const
{
    return d_func()->pwdwgt ? d_func()->pwdwgt->savePasswordVisible() : false;
}

QByteArray BLoginWidget::savePasswordWidgetState() const
{
    return d_func()->pwdwgt ? d_func()->pwdwgt->saveWidgetState() : QByteArray();
}

BPassword BLoginWidget::securePassword() const
{
    return (SecurePassword == d_func()->passwordType) ? d_func()->pwdwgt->password() : BPassword();
}

void BLoginWidget::setAddressInputMask(const QString &mask)
{
    QLineEdit *ledt = d_func()->cmboxAddress ? d_func()->cmboxAddress->lineEdit() : d_func()->ledtAddress;
    if (!ledt)
        return;
    ledt->setInputMask(mask);
}

void BLoginWidget::setAddressLabel(const BTranslation &t)
{
    d_func()->addressLabel = t;
}

void BLoginWidget::setAddressPlaceholderText(const BTranslation &t)
{
    d_func()->addressPlaceholderText = t;
    QLineEdit *ledt = d_func()->cmboxAddress ? d_func()->cmboxAddress->lineEdit() : d_func()->ledtAddress;
    if (!ledt)
        return;
    ledt->setPlaceholderText(t);
}

void BLoginWidget::setAddressType(AddressType t, bool required)
{
    if (t == d_func()->addressType)
        return setAddressRequired(required);
    d_func()->addressType = t;
    if (d_func()->ledtAddress) {
        QWidget *lbl = BGuiTools::labelForField<QWidget>(d_func()->ledtAddress);
        delete d_func()->ledtAddress;
        d_func()->ledtAddress = 0;
        delete lbl;
    }
    if (d_func()->cmboxAddress) {
        QWidget *lbl = BGuiTools::labelForField<QWidget>(d_func()->hltAddress);
        delete d_func()->hltAddress;
        d_func()->hltAddress = 0;
        delete d_func()->cmboxAddress;
        d_func()->cmboxAddress = 0;
        delete d_func()->tbtnRemoveAddress;
        d_func()->tbtnRemoveAddress = 0;
        delete lbl;
        d_func()->persistentAddresses.clear();
    }
    if (SimpleAddress == t) {
        d_func()->ledtAddress = new QLineEdit;
        d_func()->flt->insertRow(0, d_func()->addressLabel, d_func()->ledtAddress);
        connect(d_func()->ledtAddress, SIGNAL(textChanged(QString)), d_func(), SLOT(checkInputs()));
        connect(d_func()->ledtAddress, SIGNAL(textChanged(QString)), this, SIGNAL(addressChanged(QString)));
    } else if (StaticComboAddress == t) {
        d_func()->cmboxAddress = new QComboBox;
        d_func()->cmboxAddress->setEditable(false);
        d_func()->flt->insertRow(0, d_func()->addressLabel, d_func()->cmboxAddress);
        connect(d_func()->cmboxAddress, SIGNAL(currentIndexChanged(int)), d_func(), SLOT(checkInputs()));
        connect(d_func()->cmboxAddress, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(addressChanged(QString)));
    } else if (EditableComboAddress == t) {
        d_func()->hltAddress = new QHBoxLayout;
        d_func()->cmboxAddress = new QComboBox;
          d_func()->cmboxAddress->setEditable(true);
          connect(d_func()->cmboxAddress, SIGNAL(currentIndexChanged(int)),
                  d_func(), SLOT(cmboxAddressCurrentIndexChanged(int)));
        d_func()->hltAddress->addWidget(d_func()->cmboxAddress);
        d_func()->tbtnRemoveAddress = new QToolButton;
          d_func()->tbtnRemoveAddress->setIcon(BApplication::icon("editdelete"));
          d_func()->tbtnRemoveAddress->setToolTip(d_func()->removeAddressButtonToolTip);
          connect(d_func()->tbtnRemoveAddress, SIGNAL(clicked()), d_func(), SLOT(removeCurrentAddress()));
        d_func()->hltAddress->addWidget(d_func()->tbtnRemoveAddress);
        d_func()->flt->insertRow(0, d_func()->addressLabel, d_func()->hltAddress);
        connect(d_func()->cmboxAddress->lineEdit(), SIGNAL(textChanged(QString)), d_func(), SLOT(checkInputs()));
        connect(d_func()->cmboxAddress->lineEdit(), SIGNAL(textChanged(QString)),
                this, SIGNAL(addressChanged(QString)));
    } else {
        required = false;
    }
    d_func()->updateTabOrder();
    setAddressRequired(required);
}

void BLoginWidget::setAddressValidator(const QValidator *v)
{
    QLineEdit *ledt = d_func()->cmboxAddress ? d_func()->cmboxAddress->lineEdit() : d_func()->ledtAddress;
    if (!ledt)
        return;
    ledt->setValidator(v);
}

void BLoginWidget::setGeneratedPasswordLength(int len)
{
    if (!d_func()->pwdwgt)
        return;
    d_func()->pwdwgt->setGeneratedPasswordLength(len);
}

void BLoginWidget::setGeneratePasswordFunction(BPasswordWidget::GeneratePasswordFunction f)
{
    if (!d_func()->pwdwgt)
        return;
    d_func()->pwdwgt->setGeneratePasswordFunction(f);
}

void BLoginWidget::setPasswordInputMask(const QString &mask)
{
    if (d_func()->pwdwgt)
        return d_func()->pwdwgt->setInputMask(mask);
    else if (d_func()->ledtPassword)
        d_func()->ledtPassword->setInputMask(mask);
}

void BLoginWidget::setPasswordLabel(BTranslation &t)
{
    d_func()->passwordLabel = t;
}

void BLoginWidget::setPasswordPlaceholderText(const BTranslation &t)
{
    d_func()->passwordPlaceholderText = t;
    if (!d_func()->ledtPassword)
        return;
    d_func()->ledtPassword->setPlaceholderText(t);
}

void BLoginWidget::setPasswordType(PasswordType t, bool required)
{
    if (t == d_func()->passwordType)
        return setPasswordRequired(required);
    d_func()->passwordType = t;
    if (d_func()->ledtPassword) {
        QWidget *lbl = BGuiTools::labelForField<QWidget>(d_func()->ledtPassword);
        delete d_func()->ledtPassword;
        d_func()->ledtPassword = 0;
        delete lbl;
    }
    if (d_func()->pwdwgt) {
        QWidget *lbl = BGuiTools::labelForField<QWidget>(d_func()->pwdwgt);
        delete d_func()->pwdwgt;
        d_func()->pwdwgt = 0;
        delete lbl;
    }
    if (SimplePassword == t) {
        d_func()->ledtPassword = new QLineEdit;
        d_func()->flt->addRow(d_func()->passwordLabel, d_func()->ledtPassword);
        connect(d_func()->ledtPassword, SIGNAL(textChanged(QString)), d_func(), SLOT(checkInputs()));
        connect(d_func()->ledtPassword, SIGNAL(textChanged(QString)), this, SIGNAL(passwordChanged()));
        connect(d_func()->ledtPassword, SIGNAL(textChanged(QString)), this, SIGNAL(passwordChanged(QString)));
    } else if (SecurePassword == t) {
        d_func()->pwdwgt = new BPasswordWidget;
        d_func()->flt->addRow(d_func()->passwordLabel, d_func()->pwdwgt);
        connect(d_func()->pwdwgt, SIGNAL(passwordChanged()), d_func(), SLOT(checkInputs()));
        connect(d_func()->pwdwgt, SIGNAL(passwordChanged()), this, SIGNAL(passwordChanged()));
        connect(d_func()->pwdwgt, SIGNAL(passwordChanged(QString)), this, SIGNAL(passwordChanged(QString)));
        connect(d_func()->pwdwgt, SIGNAL(passwordChanged(QByteArray)), this, SIGNAL(passwordChanged(QByteArray)));
        connect(d_func()->pwdwgt, SIGNAL(passwordEdited(QString)), this, SIGNAL(passwordEdited(QString)));
    } else {
        required = false;
    }
    d_func()->updateTabOrder();
    setPasswordRequired(required);
}

void BLoginWidget::setPasswordValidator(const QValidator *v)
{
    if (d_func()->pwdwgt)
        return d_func()->pwdwgt->setValidator(v);
    else if (d_func()->ledtPassword)
        d_func()->ledtPassword->setValidator(v);
}

void BLoginWidget::setPersistentAddress(const QString &a)
{
    setPersistentAddresses(QStringList() << a);
}

void BLoginWidget::setPersistentAddresses(const QStringList &list)
{
    if (!d_func()->cmboxAddress)
        return;
    d_func()->persistentAddresses = list;
    d_func()->cmboxAddressCurrentIndexChanged(d_func()->cmboxAddress->currentIndex());
}

void BLoginWidget::setPortEnabled(bool b, bool required)
{
    if ((b && d_func()->ledtPort) || (!b && !d_func()->ledtPort))
        return;
    if (b) {
        d_func()->ledtPort = new QLineEdit;
        d_func()->ledtPort->setValidator(new QIntValidator(this));
        int ind = (NoAddress == d_func()->addressType) ? 0 : 1;
        d_func()->flt->insertRow(ind, d_func()->portLabel, d_func()->ledtPort);
        connect(d_func()->ledtPort, SIGNAL(textChanged(QString)), d_func(), SLOT(checkInputs()));
    } else {
        QWidget *lbl = BGuiTools::labelForField<QWidget>(d_func()->ledtPort);
        delete d_func()->ledtPort;
        d_func()->ledtPort = 0;
        delete lbl;
        required = false;
    }
    d_func()->updateTabOrder();
    setPortRequired(required);
}

void BLoginWidget::setPortInputMask(const QString &mask)
{
    if (!d_func()->ledtPort)
        return;
    d_func()->ledtPort->setInputMask(mask);
}

void BLoginWidget::setPortLabel(const BTranslation &t)
{
    d_func()->portLabel = t;
}

void BLoginWidget::setPortPlaceholderText(const BTranslation &t)
{
    d_func()->portPlaceholderText = t;
    if (!d_func()->ledtPort)
        return;
    d_func()->ledtPort->setPlaceholderText(t);
}

void BLoginWidget::setPortValidator(const QValidator *v)
{
    if (!d_func()->ledtPort)
        return;
    d_func()->ledtPort->setValidator(v);
}

void BLoginWidget::setLoginEnabled(bool b, bool required)
{
    if ((b && d_func()->ledtLogin) || (!b && !d_func()->ledtLogin))
        return;
    if (b) {
        d_func()->ledtLogin = new QLineEdit;
        int ind = (NoPassword == d_func()->passwordType) ? d_func()->flt->rowCount() : (d_func()->flt->rowCount() - 1);
        d_func()->flt->insertRow(ind, d_func()->loginLabel, d_func()->ledtLogin);
        connect(d_func()->ledtLogin, SIGNAL(textChanged(QString)), d_func(), SLOT(checkInputs()));
        connect(d_func()->ledtLogin, SIGNAL(textChanged(QString)), this, SIGNAL(loginChanged(QString)));
    } else {
        QWidget *lbl = BGuiTools::labelForField<QWidget>(d_func()->ledtLogin);
        delete d_func()->ledtLogin;
        d_func()->ledtLogin = 0;
        delete lbl;
        required = false;
    }
    d_func()->updateTabOrder();
    setLoginRequired(required);
}

void BLoginWidget::setLoginInputMask(const QString &mask)
{
    if (!d_func()->ledtLogin)
        return;
    d_func()->ledtLogin->setInputMask(mask);
}

void BLoginWidget::setLoginLabel(const BTranslation &t)
{
    d_func()->loginLabel = t;
}

void BLoginWidget::setLoginPlaceholderText(const BTranslation &t)
{
    d_func()->loginPlaceholderText = t;
    if (!d_func()->ledtLogin)
        return;
    d_func()->ledtLogin->setPlaceholderText(t);
}

void BLoginWidget::setLoginValidator(const QValidator *v)
{
    if (!d_func()->ledtLogin)
        return;
    d_func()->ledtLogin->setValidator(v);
}

bool BLoginWidget::showPasswordVisible() const
{
    return d_func()->pwdwgt ? d_func()->pwdwgt->showPasswordVisible() : false;
}

QString BLoginWidget::simplePassword() const
{
    return (SimplePassword == d_func()->passwordType) ? d_func()->ledtPassword->text() : QString();
}

/*============================== Public slots ==============================*/

void BLoginWidget::setAddress(const QString &a)
{
    switch (d_func()->addressType) {
    case SimpleAddress:
        d_func()->ledtAddress->setText(a);
        break;
    case StaticComboAddress: {
        int ind = d_func()->cmboxAddress->findText(a, Qt::MatchFixedString);
        if (ind >= 0)
            d_func()->cmboxAddress->setCurrentIndex(ind);
        break;
    }
    case EditableComboAddress: {
        int ind = d_func()->cmboxAddress->findText(a, Qt::MatchFixedString);
        if (ind >= 0)
            d_func()->cmboxAddress->setCurrentIndex(ind);
        else
            d_func()->cmboxAddress->lineEdit()->setText(a);
        break;
    }
    default:
        break;
    }
}

void BLoginWidget::setAddressRequired(bool b)
{
    d_func()->addressRequired = b;
    d_func()->checkInputs();
}

void BLoginWidget::setAvailableAddresses(const QStringList &list)
{
    if (!d_func()->cmboxAddress)
        return;
    d_func()->cmboxAddress->clear();
    d_func()->cmboxAddress->addItems(list);
    if (d_func()->cmboxAddress->count())
        d_func()->cmboxAddress->setCurrentIndex(0);
}

void BLoginWidget::setGeneratePasswordVisible(bool visible)
{
    if (!d_func()->pwdwgt)
        return;
    d_func()->pwdwgt->setGeneratePasswordVisible(visible);
}

void BLoginWidget::setLogin(const QString &l)
{
    if (!d_func()->ledtLogin)
        return;
    d_func()->ledtLogin->setText(l);
}

void BLoginWidget::setLoginRequired(bool b)
{
    d_func()->loginRequired = b;
    d_func()->checkInputs();
}

void BLoginWidget::setPassword(const QString &pwd)
{
    if (!d_func()->ledtPassword)
        return;
    d_func()->ledtPassword->setText(pwd);
}

void BLoginWidget::setPassword(const BPassword &pwd)
{
    if (!d_func()->pwdwgt)
        return;
    d_func()->pwdwgt->setPassword(pwd);
}

void BLoginWidget::setPasswordRequired(bool b)
{
    d_func()->passwordRequired = b;
    d_func()->checkInputs();
}

void BLoginWidget::setPort(int port)
{
    if (port < 0 || !d_func()->ledtPort)
        return;
    d_func()->ledtPort->setText(QString::number(port));
}

void BLoginWidget::setPortRequired(bool b)
{
    d_func()->portRequired = b;
    d_func()->checkInputs();
}

void BLoginWidget::setSavePasswordVisible(bool visible)
{
    if (!d_func()->pwdwgt)
        return;
    d_func()->pwdwgt->setSavePasswordVisible(visible);
}

void BLoginWidget::setShowPasswordVisible(bool visible)
{
    if (!d_func()->pwdwgt)
        return;
    d_func()->pwdwgt->setShowPasswordVisible(visible);
}
