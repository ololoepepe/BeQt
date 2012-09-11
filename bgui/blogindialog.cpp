#include "blogindialog.h"
#include "bpasswordwidget.h"

#include <QDialog>
#include <QString>
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QByteArray>

BLoginDialog::BLoginDialog(QWidget *parent) :
    QDialog(parent)
{
    _m_vlt = new QVBoxLayout(this);
      _m_lblText = new QLabel(this);
        _m_lblText->setWordWrap(true);
      _m_vlt->addWidget(_m_lblText);
      _m_lblInformativeText = new QLabel(this);
        _m_lblInformativeText->setWordWrap(true);
      _m_vlt->addWidget(_m_lblInformativeText);
      _m_flt = new QFormLayout;
        _m_flt->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
        _m_ledtLogin = new QLineEdit(this);
        _m_flt->addRow(tr("Login:", "lbl text"), _m_ledtLogin);
        _m_pwdwgt = new BPasswordWidget(this);
        _m_flt->addRow(tr("Password:", "lbl text"), _m_pwdwgt);
      _m_vlt->addLayout(_m_flt);
      _m_hlt = new QHBoxLayout;
        _m_hlt->addStretch();
        _m_btnCancel = new QPushButton(this);
          _m_btnCancel->setText( tr("Cancel", "btn text") );
          connect( _m_btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
        _m_hlt->addWidget(_m_btnCancel);
        _m_btnOk = new QPushButton(this);
          _m_btnOk->setDefault(true);
          _m_btnOk->setText( tr("Ok", "btn text") );
          connect( _m_btnOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
        _m_hlt->addWidget(_m_btnOk);
      _m_vlt->addLayout(_m_hlt);
}

//

void BLoginDialog::setText(const QString &text)
{
    _m_lblText->setText(text);
}

void BLoginDialog::setInformativeText(const QString &text)
{
    _m_lblInformativeText->setText(text);
}

void BLoginDialog::setLogin(const QString &text)
{
    _m_ledtLogin->setText(text);
}

void BLoginDialog::setPassword(const QString &text)
{
    _m_pwdwgt->setPassword(text);
}

void BLoginDialog::setEncryptedPassword(const QByteArray &password, const QByteArray &key)
{
    _m_pwdwgt->setEncryptedPassword(password, key);
}

void BLoginDialog::setShowPassword(bool show)
{
    _m_pwdwgt->setShowPassword(show);
}

void BLoginDialog::setSavePassword(bool save)
{
    _m_pwdwgt->setSavePassword(save);
}

QString BLoginDialog::login() const
{
    return _m_ledtLogin->text();
}

QString BLoginDialog::password() const
{
    return _m_pwdwgt->password();
}

QByteArray BLoginDialog::encryptedPassword(const QByteArray &key) const
{
    return _m_pwdwgt->encryptedPassword(key);
}

bool BLoginDialog::showPassword() const
{
    return _m_pwdwgt->showPassword();
}

bool BLoginDialog::savePassword() const
{
    return _m_pwdwgt->savePassword();
}
