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

/*============================================================================
================================ Password Widget Private Object
============================================================================*/

BPasswordWidgetPrivateObject::BPasswordWidgetPrivateObject(BPasswordWidgetPrivate *p) :
    BBasePrivateObject(p)
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
    p_func()->retranslateUi();
}

void BPasswordWidgetPrivateObject::tbtnSaveClicked()
{
    p_func()->resetSave();
}

void BPasswordWidgetPrivateObject::tbtnShowClicked()
{
    p_func()->resetShow();
}

/*============================================================================
================================ Password Widget Private
============================================================================*/

const QDataStream::Version BPasswordWidgetPrivate::DSVersion = QDataStream::Qt_4_8;

//

BPasswordWidgetPrivate::BPasswordWidgetPrivate(BPasswordWidget *q) :
    BBasePrivate( *q, *new BPasswordWidgetPrivateObject(this) )
{
    save = true; //Is reset to false, so it's false by default
    show = true; //Is reset to false, so it's false by default
    charCount = -1;
    hlt = new QHBoxLayout(q);
    hlt->setContentsMargins(0, 0, 0, 0);
    ledt = new QLineEdit(q);
    hlt->addWidget(ledt);
    tbtnSave = new QToolButton(q);
      tbtnSave->setIcon( BApplication::beqtIcon("password_save") );
      QObject::connect( tbtnSave, SIGNAL( clicked() ), o_func(), SLOT( tbtnSaveClicked() ) );
      resetSave();
    hlt->addWidget(tbtnSave);
    tbtnShow = new QToolButton(q);
      tbtnShow->setIcon( BApplication::beqtIcon("decrypted") );
      QObject::connect( tbtnShow, SIGNAL( clicked() ), o_func(), SLOT( tbtnShowClicked() ) );
      resetShow();
    hlt->addWidget(tbtnShow);
    //
    retranslateUi();
    QObject::connect( bApp, SIGNAL( languageChanged() ), o_func(), SLOT( languageChanged() ) );
}

BPasswordWidgetPrivate::~BPasswordWidgetPrivate()
{
    //
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

BPasswordWidgetPrivate::BPasswordWidgetPrivate(BPasswordWidget &q, BPasswordWidgetPrivateObject &o) :
    BBasePrivate(q, o)
{
    //
}

/*============================================================================
================================ Password Widget
============================================================================*/

QByteArray BPasswordWidget::encrypt(const QString &string, QCryptographicHash::Algorithm method)
{
    return QCryptographicHash::hash(string.toUtf8(), method);
}

BPasswordWidget::PasswordWidgetData BPasswordWidget::stateToData(const QByteArray &ba)
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
    return pd;
}

QByteArray BPasswordWidget::dataToState(const PasswordWidgetData &dt)
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(BPasswordWidgetPrivate::DSVersion);
    bool enc = !dt.encryptedPassword.isEmpty();
    out << enc;
    if (enc)
    {
        out << dt.encryptedPassword;
        out << dt.charCount;
    }
    else
    {
        out << dt.password;
    }
    out << dt.save;
    out << dt.show;
    return ba;
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
    setData( stateToData(ba) );
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

BPasswordWidget::PasswordWidgetData BPasswordWidget::data() const
{
    PasswordWidgetData pd;
    pd.password = password();
    pd.save = savePassword();
    pd.show = showPassword();
    return pd;
}

BPasswordWidget::PasswordWidgetData BPasswordWidget::encryptedData(QCryptographicHash::Algorithm method) const
{
    PasswordWidgetData pd;
    pd.encryptedPassword = encryptedPassword(method);
    pd.charCount = passwordCharCount();
    pd.save = savePassword();
    pd.show = showPassword();
    return pd;
}

QByteArray BPasswordWidget::saveState() const
{
    return dataToState( data() );
}

QByteArray BPasswordWidget::saveStateEncrypted(QCryptographicHash::Algorithm method) const
{
    return dataToState( encryptedData(method) );
}

//

BPasswordWidget::BPasswordWidget(BPasswordWidgetPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    //
}

