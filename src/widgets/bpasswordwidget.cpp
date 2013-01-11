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
================================ BPasswordWidgetPrivate ======================
============================================================================*/

/*============================== Static public constants ===================*/

const QDataStream::Version BPasswordWidgetPrivate::DSVersion = QDataStream::Qt_5_0;

/*============================== Public constructors =======================*/

BPasswordWidgetPrivate::BPasswordWidgetPrivate(BPasswordWidget *q) :
    BBasePrivate(q)
{
    //
}

BPasswordWidgetPrivate::~BPasswordWidgetPrivate()
{
    //
}

/*============================== Static public methods =====================*/

BPasswordWidget::PasswordWidgetData BPasswordWidgetPrivate::createPasswordWidgetData()
{
    BPasswordWidget::PasswordWidgetData pwd;
    pwd.charCount = -1;
    pwd.save = false;
    pwd.show = false;
    return pwd;
}

/*============================== Public methods ============================*/

void BPasswordWidgetPrivate::init()
{
    B_Q(BPasswordWidget);
    save = true; //Is reset to false, so it's false by default
    show = true; //Is reset to false, so it's false by default
    charCount = -1;
    hlt = new QHBoxLayout(q);
    hlt->setContentsMargins(0, 0, 0, 0);
    ledt = new QLineEdit(q);
    hlt->addWidget(ledt);
    tbtnSave = new QToolButton(q);
      tbtnSave->setIcon( BApplication::icon("filesave") );
      connect( tbtnSave, SIGNAL( clicked() ), this, SLOT( resetSave() ) );
      resetSave();
    hlt->addWidget(tbtnSave);
    tbtnShow = new QToolButton(q);
      tbtnShow->setIcon( BApplication::icon("decrypted") );
      connect( tbtnShow, SIGNAL( clicked() ), this, SLOT( resetShow() ) );
      resetShow();
    hlt->addWidget(tbtnShow);
    //
    retranslateUi();
    connect( bApp, SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

/*============================== Public slots ==============================*/

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

/*============================================================================
================================ BPasswordWidget =============================
============================================================================*/

/*============================== Public constructors =======================*/

BPasswordWidget::BPasswordWidget(QWidget *parent) :
    QWidget(parent), BBase( *new BPasswordWidgetPrivate(this) )
{
    d_func()->init();
}

BPasswordWidget::~BPasswordWidget()
{
    //
}

/*============================== Protected constructors ====================*/

BPasswordWidget::BPasswordWidget(BPasswordWidgetPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

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

/*============================== Public methods ============================*/

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
