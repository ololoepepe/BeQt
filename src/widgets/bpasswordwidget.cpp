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
#include <QDataStream>
#include <QIODevice>

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

const QDataStream::Version BPasswordWidgetPrivate::DSVersion = QDataStream::Qt_4_8;

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
      tbtnSave->setIcon( BApplication::beqtIcon("password_save") );
      QObject::connect( tbtnSave, SIGNAL( clicked() ), _m_o, SLOT( tbtnSaveClicked() ) );
      resetSave();
    hlt->addWidget(tbtnSave);
    tbtnShow = new QToolButton(q);
      tbtnShow->setIcon( BApplication::beqtIcon("decrypted") );
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
    tbtnSave->setDown(save);
}

void BPasswordWidgetPrivate::resetShow()
{
    show = !show;
    tbtnShow->setDown(show);
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

void BPasswordWidget::setData(const PasswordWidgetData &pd)
{
    if ( !pd.password.isEmpty() )
        setPassword(pd.password);
    else
        setEncryptedPassword(pd.encryptedPassword, pd.charCount);
    setSavePassword(pd.save);
    setShowPassword(pd.show);
}

void BPasswordWidget::clear()
{
    B_D(BPasswordWidget);
    d->encPassword.clear();
    d->ledt->clear();
}

void BPasswordWidget::restoreState(const QByteArray &ba)
{
    QDataStream in(ba);
    in.setVersion(BPasswordWidgetPrivate::DSVersion);
    bool enc = false;
    in >> enc;
    PasswordWidgetData pd;
    if (enc)
    {
        in >> pd.encryptedPassword;
        in >> pd.charCount;
    }
    else
    {
        in >> pd.password;
    }
    in >> pd.save;
    in >> pd.show;
    setData(pd);
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

QByteArray BPasswordWidget::saveState() const
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(BPasswordWidgetPrivate::DSVersion);
    out << false;
    out << password();
    out << savePassword();
    out << showPassword();
    return ba;
}

QByteArray BPasswordWidget::saveStateEncrypted(QCryptographicHash::Algorithm method) const
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(BPasswordWidgetPrivate::DSVersion);
    out << true;
    out << encryptedPassword(method);
    out << d_func()->charCount;
    out << savePassword();
    out << showPassword();
    return ba;
}

//

BPasswordWidget::BPasswordWidget(BPasswordWidgetPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    //
}

