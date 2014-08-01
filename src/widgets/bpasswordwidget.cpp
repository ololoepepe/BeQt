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

#include "bpasswordwidget.h"
#include "bpasswordwidget_p.h"

#include "bapplication.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>
#include <BeQtCore/BPassword>
#include <BeQtCore/private/bbaseobject_p.h>

#include <BUuid>
#include <QByteArray>
#include <QCryptographicHash>
#include <QDebug>
#include <QHBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QMetaObject>
#include <QObject>
#include <QSize>
#include <QString>
#include <QToolButton>
#include <QVariant>
#include <QVariantMap>
#include <QWidget>

/*============================================================================
================================ BPasswordWidgetPrivate ======================
============================================================================*/

/*============================== Public constructors =======================*/

BPasswordWidgetPrivate::BPasswordWidgetPrivate(BPasswordWidget *q) :
    BBaseObjectPrivate(q)
{
    //
}

BPasswordWidgetPrivate::~BPasswordWidgetPrivate()
{
    //
}

/*============================== Static public methods =====================*/

QString BPasswordWidgetPrivate::defaultGeneratePasswordFunction(int len)
{
    return BUuid::createUuid().toString().remove('{').remove('-').remove('}').left(len);
}

/*============================== Public methods ============================*/

void BPasswordWidgetPrivate::init()
{
    valid = false;
    B_Q(BPasswordWidget);
    wasEmpty = true;
    generateFunction = &defaultGeneratePasswordFunction;
    generatedLength = 16;
    QHBoxLayout *hlt = new QHBoxLayout(q);
    hlt->setContentsMargins(0, 0, 0, 0);
    ledt = new QLineEdit(q);
      ledt->setEchoMode(QLineEdit::Password);
      connect(ledt, SIGNAL(textChanged(QString)), this, SLOT(passwordChanged(QString)));
      connect(ledt, SIGNAL(textEdited(QString)), q, SIGNAL(passwordEdited(QString)));
      connect(ledt, SIGNAL(returnPressed()), q, SIGNAL(returnPressed()));
    hlt->addWidget(ledt);
    QSize sz(ledt->sizeHint().height(), ledt->sizeHint().height());
    tbtnSave = new QToolButton(q);
      tbtnSave->setCheckable(true);
      tbtnSave->setIconSize(sz - (tbtnSave->sizeHint() - tbtnSave->iconSize()));
      tbtnSave->setIcon(BApplication::icon("filesave"));
      tbtnSave->setFixedSize(sz);
      connect(tbtnSave, SIGNAL(toggled(bool)), this, SLOT(resetSave(bool)));
    hlt->addWidget(tbtnSave);
    tbtnShow = new QToolButton(q);
      tbtnShow->setCheckable(true);
      tbtnShow->setIconSize(tbtnSave->iconSize());
      tbtnShow->setIcon(BApplication::icon("decrypted"));
      tbtnShow->installEventFilter(this);
      tbtnShow->setFixedSize(sz);
      connect(tbtnShow, SIGNAL(toggled(bool)), this, SLOT(resetShow(bool)));
    hlt->addWidget(tbtnShow);
    tbtnGenerate = new QToolButton(q);
      tbtnGenerate->setIconSize(tbtnSave->iconSize());
      tbtnGenerate->setIcon(BApplication::icon("charset"));
      tbtnGenerate->setVisible(false);
      tbtnGenerate->setFixedSize(sz);
      connect(tbtnGenerate, SIGNAL(clicked()), q, SLOT(generatePassword()));
    hlt->addWidget(tbtnGenerate);
    //
    retranslateUi();
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

void BPasswordWidgetPrivate::updateEdit()
{
    if (pwd.isEncrypted()) {
        ledt->clear();
        if (!pwd.encryptedPassword().isEmpty())
            ledt->setPlaceholderText(QString(pwd.charCountHint() > 0 ? pwd.charCountHint() : 8, '*'));
        else
            ledt->setPlaceholderText("");
    } else {
        ledt->setText(pwd.openPassword());
        ledt->setPlaceholderText("");
    }
}

/*============================== Public slots ==============================*/

void BPasswordWidgetPrivate::passwordChanged(const QString &password)
{
    bool bvalid = valid;
    valid = q_func()->hasAcceptableInput();
    B_Q(BPasswordWidget);
    if (wasEmpty && !password.isEmpty() && pwd.isEncrypted()) {
        pwdBackup = pwd;
        wasEmpty = false;
        pwd.setPassword(password);
    } else if (!wasEmpty && password.isEmpty() && pwdBackup.isEncrypted()) {
        pwd = pwdBackup;
        wasEmpty = true;
    } else {
        pwd.setPassword(password);
    }
    QMetaObject::invokeMethod(q, "passwordChanged");
    if (pwd.isEncrypted())
        QMetaObject::invokeMethod(q, "passwordChanged", Q_ARG(QByteArray, pwd.encryptedPassword()));
    else
        QMetaObject::invokeMethod(q, "passwordChanged", Q_ARG(QString, pwd.openPassword()));
    if (bvalid != valid)
        QMetaObject::invokeMethod(q, "inputValidityChanged", Q_ARG(bool, valid));
}

void BPasswordWidgetPrivate::resetSave(bool b)
{
    QMetaObject::invokeMethod(q_func(), "savePasswordChanged", Q_ARG(bool, b));
}

void BPasswordWidgetPrivate::resetShow(bool b)
{
    ledt->setEchoMode(b ? QLineEdit::Normal : QLineEdit::Password);
    QMetaObject::invokeMethod(q_func(), "showPasswordChanged", Q_ARG(bool, b));
}

void BPasswordWidgetPrivate::retranslateUi()
{
    tbtnSave->setToolTip(tr("Save password", "tbtn toolTip"));
    tbtnShow->setToolTip(tr("Show password", "tbtn toolTip"));
    tbtnGenerate->setToolTip(tr("Generate password", "tbtn toolTip"));
}

/*============================================================================
================================ BPasswordWidget =============================
============================================================================*/

/*============================== Public constructors =======================*/

BPasswordWidget::BPasswordWidget(QWidget *parent) :
    QWidget(parent), BBaseObject(*new BPasswordWidgetPrivate(this))
{
    d_func()->init();
}

BPasswordWidget::BPasswordWidget(const BPassword &password, QWidget *parent) :
    QWidget(parent), BBaseObject(*new BPasswordWidgetPrivate(this))
{
    d_func()->init();
    setPassword(password);
}

BPasswordWidget::~BPasswordWidget()
{
    //
}

/*============================== Protected constructors ====================*/

BPasswordWidget::BPasswordWidget(BPasswordWidgetPrivate &d, QWidget *parent) :
    QWidget(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QCryptographicHash::Algorithm BPasswordWidget::algorithm() const
{
    return d_func()->pwd.algorithm();
}

int BPasswordWidget::charCountHint(BPassword::Mode mode) const
{
    return d_func()->pwd.charCountHint(mode);
}

QByteArray BPasswordWidget::encryptedPassword(int *charCountHint) const
{
    return d_func()->pwd.encryptedPassword(charCountHint);
}

QByteArray BPasswordWidget::encryptedPassword(QCryptographicHash::Algorithm a, int *charCountHint) const
{
    return d_func()->pwd.encryptedPassword(a, charCountHint);
}

int BPasswordWidget::generatedPasswordLength() const
{
    return d_func()->generatedLength;
}

BPasswordWidget::GeneratePasswordFunction BPasswordWidget::generatePasswordFunction() const
{
    return d_func()->generateFunction;
}

bool BPasswordWidget::generatePasswordVisible() const
{
    return d_func()->tbtnGenerate->isVisibleTo(const_cast<BPasswordWidget *>(this));
}

bool BPasswordWidget::hasAcceptableInput() const
{
    return d_func()->ledt->hasAcceptableInput();
}

QString BPasswordWidget::inputMask() const
{
    return d_func()->ledt->inputMask();
}

bool BPasswordWidget::isEmpty() const
{
    return d_func()->ledt->text().isEmpty();
}

int BPasswordWidget::maxLength() const
{
    return d_func()->ledt->maxLength();
}

BPassword::Mode BPasswordWidget::mode() const
{
    return d_func()->pwd.mode();
}

QString BPasswordWidget::openPassword() const
{
    return d_func()->pwd.openPassword();
}

BPassword BPasswordWidget::password() const
{
    return d_func()->pwd;
}

void BPasswordWidget::restorePasswordState(const QByteArray &ba)
{
    BPassword pwd;
    pwd.restore(ba);
    if (pwd == d_func()->pwd)
        return;
    d_func()->pwd = pwd;
    d_func()->updateEdit();
    Q_EMIT passwordChanged();
    if (d_func()->pwd.isEncrypted())
        Q_EMIT passwordChanged(d_func()->pwd.encryptedPassword());
    else
        Q_EMIT passwordChanged(d_func()->pwd.openPassword());
}

void BPasswordWidget::restoreState(const QByteArray &ba)
{
    QVariantMap m = BeQt::deserialize(ba).toMap();
    restorePasswordState(m.value("password_state").toByteArray());
    restoreWidgetState(m.value("widget_state").toByteArray());
}

void BPasswordWidget::restoreWidgetState(const QByteArray &ba)
{
    QVariantMap m = BeQt::deserialize(ba).toMap();
    setSavePassword(m.value("save_password").toBool());
    setShowPassword(m.value("show_password").toBool());
}

bool BPasswordWidget::savePassword() const
{
    return d_func()->tbtnSave->isChecked();
}

QByteArray BPasswordWidget::savePasswordState(BPassword::Mode mode) const
{
    return d_func()->pwd.save(mode);
}

bool BPasswordWidget::savePasswordVisible() const
{
    return d_func()->tbtnSave->isVisibleTo(const_cast<BPasswordWidget *>(this));
}

QByteArray BPasswordWidget::saveState(BPassword::Mode mode) const
{
    QVariantMap m;
    m.insert("password_state", savePasswordState(mode));
    m.insert("widget_state", saveWidgetState());
    return BeQt::serialize(m);
}

QByteArray BPasswordWidget::saveWidgetState() const
{
    QVariantMap m;
    m.insert("save_password", savePassword());
    m.insert("show_password", showPassword());
    return BeQt::serialize(m);
}

void BPasswordWidget::setGeneratedPasswordLength(int len)
{
    if (len < 1)
        return;
    d_func()->generatedLength = len;
}

void BPasswordWidget::setGeneratePasswordFunction(GeneratePasswordFunction f)
{
    d_func()->generateFunction = f;
}

void BPasswordWidget::setGeneratePasswordVisible(bool visible)
{
    d_func()->tbtnGenerate->setVisible(visible);
}

void BPasswordWidget::setInputMask(const QString &mask)
{
    d_func()->ledt->setInputMask(mask);
}

void BPasswordWidget::setMaxLength(int length)
{
    d_func()->ledt->setMaxLength(length);
}

void BPasswordWidget::setMode(BPassword::Mode mode)
{
    if (d_func()->pwd.mode() == mode)
        return;
    d_func()->pwd.setMode(mode);
    d_func()->updateEdit();
    Q_EMIT passwordChanged();
    if (d_func()->pwd.isEncrypted())
        Q_EMIT passwordChanged(d_func()->pwd.encryptedPassword());
    else
        Q_EMIT passwordChanged(d_func()->pwd.openPassword());
}

void BPasswordWidget::setPassword(QCryptographicHash::Algorithm a, const QByteArray &password, int charCount)
{
    BPassword pwd(d_func()->pwd.mode());
    pwd.setPassword(a, password, charCount);
    if (pwd == d_func()->pwd)
        return;
    d_func()->pwd = pwd;
    d_func()->updateEdit();
    Q_EMIT passwordChanged();
    if (d_func()->pwd.isEncrypted())
        Q_EMIT passwordChanged(d_func()->pwd.encryptedPassword());
    else
        Q_EMIT passwordChanged(d_func()->pwd.openPassword());
}

void BPasswordWidget::setValidator(const QValidator *v)
{
    d_func()->ledt->setValidator(v);
}

bool BPasswordWidget::showPassword() const
{
    return d_func()->tbtnShow->isChecked();
}

bool BPasswordWidget::showPasswordVisible() const
{
    return d_func()->tbtnShow->isVisibleTo(const_cast<BPasswordWidget *>(this));
}

const QValidator *BPasswordWidget::validator() const
{
    return d_func()->ledt->validator();
}

/*============================== Public slots ==============================*/

void BPasswordWidget::clear()
{
    BPassword pwd = d_func()->pwd;
    pwd.clear();
    if (pwd == d_func()->pwd)
        return;
    d_func()->pwd = pwd;
    d_func()->updateEdit();
    Q_EMIT passwordChanged();
    if (d_func()->pwd.isEncrypted())
        Q_EMIT passwordChanged(d_func()->pwd.encryptedPassword());
    else
        Q_EMIT passwordChanged(d_func()->pwd.openPassword());
}

void BPasswordWidget::encrypt(QCryptographicHash::Algorithm a)
{
    BPassword pwd = d_func()->pwd;
    pwd.encrypt(a);
    if (pwd == d_func()->pwd)
        return;
    d_func()->pwd = pwd;
    d_func()->updateEdit();
    Q_EMIT passwordChanged();
    if (d_func()->pwd.isEncrypted())
        Q_EMIT passwordChanged(d_func()->pwd.encryptedPassword());
    else
        Q_EMIT passwordChanged(d_func()->pwd.openPassword());
}

void BPasswordWidget::generatePassword()
{
    B_D(BPasswordWidget);
    if (!d->generateFunction)
        return;
    QString pwd = d->generateFunction(d->generatedLength);
    Q_EMIT passwordGenerated(pwd);
    setPassword(pwd);
}

void BPasswordWidget::setPassword(const BPassword &password)
{
    if (password == d_func()->pwd)
        return;
    d_func()->pwd = password;
    d_func()->updateEdit();
    Q_EMIT passwordChanged();
    if (d_func()->pwd.isEncrypted())
        Q_EMIT passwordChanged(d_func()->pwd.encryptedPassword());
    else
        Q_EMIT passwordChanged(d_func()->pwd.openPassword());
}

void BPasswordWidget::setPassword(const QString &password)
{
    BPassword pwd(d_func()->pwd.mode());
    pwd.setPassword(password, d_func()->pwd.algorithm());
    if (pwd == d_func()->pwd)
        return;
    d_func()->pwd = pwd;
    d_func()->updateEdit();
    Q_EMIT passwordChanged();
    if (d_func()->pwd.isEncrypted())
        Q_EMIT passwordChanged(d_func()->pwd.encryptedPassword());
    else
        Q_EMIT passwordChanged(d_func()->pwd.openPassword());
}

void BPasswordWidget::setSavePassword(bool b)
{
    d_func()->tbtnSave->setChecked(b);
}

void BPasswordWidget::setSavePasswordVisible(bool visible)
{
    d_func()->tbtnSave->setVisible(visible);
}

void BPasswordWidget::setShowPasswordVisible(bool visible)
{
    d_func()->tbtnShow->setVisible(visible);
}

void BPasswordWidget::setShowPassword(bool b)
{
    d_func()->tbtnShow->setChecked(b);
}
