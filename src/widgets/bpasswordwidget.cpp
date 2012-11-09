#include "bpasswordwidget.h"
#include "bpasswordwidget_p.h"
#include "bapplication.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QWidget>
#include <QObject>
#include <QCryptographicHash>
#include <QString>
#include <QByteArray>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QIcon>

#include <QDebug>

BPasswordWidgetPrivateObject::BPasswordWidgetPrivateObject(BPasswordWidgetPrivate *p) :
    QObject(0), _m_p(p)
{
    //
}

BPasswordWidgetPrivateObject::~BPasswordWidgetPrivateObject()
{
    //
}

//

void BPasswordWidgetPrivateObject::languageChanged()
{
    _m_p->retranslateUi();
}

void BPasswordWidgetPrivateObject::tbtnSaveClicked()
{
    _m_p->resetSave();
}

void BPasswordWidgetPrivateObject::tbtnShowClicked()
{
    _m_p->resetShow();
}

//

BPasswordWidgetPrivate::BPasswordWidgetPrivate(BPasswordWidget *q) :
    BBasePrivate(q), _m_o( new BPasswordWidgetPrivateObject(this) )
{
    save = true;
    show = true;
    charCount = -1;
    hlt = new QHBoxLayout(q);
    hlt->setContentsMargins(0, 0, 0, 0);
    ledt = new QLineEdit(q);
    hlt->addWidget(ledt);
    tbtnSave = new QToolButton(q);
      QObject::connect( tbtnSave, SIGNAL( clicked() ), _m_o, SLOT( tbtnSaveClicked() ) );
      resetSave();
    hlt->addWidget(tbtnSave);
    tbtnShow = new QToolButton(q);
      QObject::connect( tbtnShow, SIGNAL( clicked() ), _m_o, SLOT( tbtnShowClicked() ) );
      resetShow();
    hlt->addWidget(tbtnShow);
    //
    retranslateUi();
    QObject::connect( bApp, SIGNAL( languageChanged() ), _m_o, SLOT( languageChanged() ) );
}

BPasswordWidgetPrivate::~BPasswordWidgetPrivate()
{
    _m_o->deleteLater();
}

//

void BPasswordWidgetPrivate::retranslateUi()
{
    tbtnSave->setToolTip( tr("Save password", "tbtn toolTip") );
    tbtnShow->setToolTip( tr("Show password", "tbtn toolTip") );
}

void BPasswordWidgetPrivate::resetSave()
{
    save = !save;
    tbtnSave->setIcon( BApplication::beqtIcon(save ? "password_save" : "password_dont_save") );
}

void BPasswordWidgetPrivate::resetShow()
{
    show = !show;
    tbtnShow->setIcon( BApplication::beqtIcon(show ? "decrypted" : "encrypted") );
    ledt->setEchoMode(show ? QLineEdit::Normal : QLineEdit::Password);
}

//

QByteArray BPasswordWidget::encrypt(const QString &string, QCryptographicHash::Algorithm method)
{
    return QCryptographicHash::hash(string.toUtf8(), method);
}

//

BPasswordWidget::BPasswordWidget(QWidget *parent) :
    QWidget(parent), BBase( *new BPasswordWidgetPrivate(this) )
{
    //
}

BPasswordWidget::~BPasswordWidget()
{
    //
}

//

void BPasswordWidget::setPassword(const QString &password)
{
    B_D(BPasswordWidget);
    d->ledt->setText(password);
    d->encPassword.clear();
}

void BPasswordWidget::setEncryptedPassword(const QByteArray &password, int charCount)
{
    B_D(BPasswordWidget);
    d->encPassword = password;
    d->ledt->clear();
    if ( password.isEmpty() )
        charCount = 0;
    QString ph;
    if ( !password.isEmpty() )
        ph.fill('*', charCount > 0 ? charCount : 8);
    d->ledt->setPlaceholderText(ph);
    d->charCount = charCount;
}

void BPasswordWidget::setSavePassword(bool b)
{
    B_D(BPasswordWidget);
    if (b == d->save)
        return;
    d->resetSave();
}

void BPasswordWidget::setShowPassword(bool b)
{
    B_D(BPasswordWidget);
    if (b == d->show)
        return;
    d->resetShow();
}

void BPasswordWidget::clear()
{
    B_D(BPasswordWidget);
    d->encPassword.clear();
    d->ledt->clear();
}

QString BPasswordWidget::password() const
{
    return d_func()->ledt->text();
}

QByteArray BPasswordWidget::encryptedPassword(QCryptographicHash::Algorithm method) const
{
    const B_D(BPasswordWidget);
    QString pwd = d->ledt->text();
    return !pwd.isEmpty() ? encrypt(pwd, method) : d->encPassword;
}

int BPasswordWidget::passwordCharCount() const
{
    const B_D(BPasswordWidget);
    QString pwd = d->ledt->text();
    return !pwd.isEmpty() ? pwd.length() : d->charCount;
}

bool BPasswordWidget::savePassword() const
{
    return d_func()->save;
}

bool BPasswordWidget::showPassword() const
{
    return d_func()->show;
}

//

BPasswordWidget::BPasswordWidget(BPasswordWidgetPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    //
}

