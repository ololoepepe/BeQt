#include "bpasswordwidget.h"
#include "bpasswordwidget_p.h"
#include "bapplication.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BPassword>

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
#include <QTimer>
#include <QEvent>
#include <QMetaObject>
#include <QUuid>
#include <QVariant>
#include <QVariantMap>

#include <QDebug>

static QString defaultGeneratePasswordFunction(int len)
{
    return QUuid::createUuid().toString().remove('{').remove('-').remove('}').left(len);
}

/*============================================================================
================================ BPasswordWidgetPrivate ======================
============================================================================*/

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

/*============================== Public methods ============================*/

void BPasswordWidgetPrivate::init()
{
    B_Q(BPasswordWidget);
    save = true; //Is reset to false, so it's false by default
    show = true; //Is reset to false, so it's false by default
    generateFunction = &defaultGeneratePasswordFunction;
    generatedLength = 16;
    hlt = new QHBoxLayout(q);
    hlt->setContentsMargins(0, 0, 0, 0);
    ledt = new QLineEdit(q);
      connect(ledt, SIGNAL(textChanged(QString)), this, SLOT(passwordChanged(QString)));
      connect(ledt, SIGNAL(textEdited(QString)), q, SIGNAL(passwordEdited(QString)));
      connect(ledt, SIGNAL(returnPressed()), q, SIGNAL(returnPressed()));
    hlt->addWidget(ledt);
    tbtnSave = new QToolButton(q);
      tbtnSave->setIcon( BApplication::icon("filesave") );
      tbtnSave->installEventFilter(this);
      connect( tbtnSave, SIGNAL( clicked() ), this, SLOT( resetSave() ) );
      resetSave();
    hlt->addWidget(tbtnSave);
    tbtnShow = new QToolButton(q);
      tbtnShow->setIcon( BApplication::icon("decrypted") );
      tbtnShow->installEventFilter(this);
      connect( tbtnShow, SIGNAL( clicked() ), this, SLOT( resetShow() ) );
      resetShow();
    hlt->addWidget(tbtnShow);
    tbtnGenerate = new QToolButton(q);
      tbtnGenerate->setIcon(BApplication::icon("charset"));
      tbtnGenerate->setVisible(false);
      connect(tbtnGenerate, SIGNAL(clicked()), q, SLOT(generatePassword()));
    hlt->addWidget(tbtnGenerate);
    //
    retranslateUi();
    connect( bApp, SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

bool BPasswordWidgetPrivate::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() != QEvent::FocusIn && e->type() != QEvent::FocusOut)
        return false;
    if (o == tbtnSave)
    {
        save = !save;
        QTimer::singleShot( 0, this, SLOT( resetSave() ) );
    }
    if (o == tbtnShow)
    {
        show = !show;
        QTimer::singleShot( 0, this, SLOT( resetShow() ) );
    }
    return false;
}

void BPasswordWidgetPrivate::updateEdit()
{
    if (pwd.isEncrypted())
    {
        ledt->clear();
        qDebug() << pwd.charCountHint();
        ledt->setPlaceholderText(QString(pwd.charCountHint() > 0 ? pwd.charCountHint() : 8, '*'));
        qDebug() << ledt->placeholderText();
    }
    else
    {
        ledt->setText(pwd.openPassword());
        ledt->setPlaceholderText("");
    }
}

/*============================== Public slots ==============================*/

void BPasswordWidgetPrivate::retranslateUi()
{
    tbtnSave->setToolTip( tr("Save password", "tbtn toolTip") );
    tbtnShow->setToolTip( tr("Show password", "tbtn toolTip") );
    tbtnGenerate->setToolTip(tr("Generate password", "tbtn toolTip"));
}

void BPasswordWidgetPrivate::resetSave()
{
    save = !save;
    tbtnSave->setDown(save);
    QMetaObject::invokeMethod(q_func(), "savePasswordChanged", Q_ARG(bool, save));
}

void BPasswordWidgetPrivate::resetShow()
{
    show = !show;
    tbtnShow->setDown(show);
    ledt->setEchoMode(show ? QLineEdit::Normal : QLineEdit::Password);
    QMetaObject::invokeMethod(q_func(), "showPasswordChanged", Q_ARG(bool, show));
}

void BPasswordWidgetPrivate::passwordChanged(const QString &password)
{
    B_Q(BPasswordWidget);
    pwd.setPassword(password);
    QMetaObject::invokeMethod(q, "passwordChanged");
    if (pwd.isEncrypted())
        QMetaObject::invokeMethod(q, "passwordChanged", Q_ARG(QByteArray, pwd.encryptedPassword()));
    else
        QMetaObject::invokeMethod(q, "passwordChanged", Q_ARG(QString, pwd.openPassword()));
}

/*============================================================================
================================ BPasswordWidget =============================
============================================================================*/

/*============================== Public constructors =======================*/

BPasswordWidget::BPasswordWidget(QWidget *parent) :
    QWidget(parent), BBase(*new BPasswordWidgetPrivate(this))
{
    d_func()->init();
}

BPasswordWidget::BPasswordWidget(const BPassword &password, QWidget *parent) :
    QWidget(parent), BBase(*new BPasswordWidgetPrivate(this))
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
    QWidget(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BPasswordWidget::setMode(BPassword::Mode mode)
{
    d_func()->pwd.setMode(mode);
    d_func()->updateEdit();
}

void BPasswordWidget::setPassword(const BPassword &password)
{
    d_func()->pwd = password;
    d_func()->updateEdit();
}

void BPasswordWidget::setPassword(const QString &password)
{
    d_func()->pwd = BPassword(password);
    d_func()->updateEdit();
}

void BPasswordWidget::setPassword(QCryptographicHash::Algorithm a, const QByteArray &password, int charCount)
{
    d_func()->pwd = BPassword(a, password, charCount);
    d_func()->updateEdit();
}

void BPasswordWidget::encrypt(QCryptographicHash::Algorithm a)
{
    d_func()->pwd.encrypt(a);
    d_func()->updateEdit();
}

void BPasswordWidget::setSavePasswordVisible(bool visible)
{
    d_func()->tbtnSave->setVisible(visible);
}

void BPasswordWidget::setShowPasswordVisible(bool visible)
{
    d_func()->tbtnShow->setVisible(visible);
}

void BPasswordWidget::setGeneratePasswordVisible(bool visible)
{
    d_func()->tbtnGenerate->setVisible(visible);
}

void BPasswordWidget::setGeneratePasswordFunction(GeneratePasswordFunction f)
{
    d_func()->generateFunction = f;
}

void BPasswordWidget::setGeneratedPasswordLength(int len)
{
    if (len < 1)
        return;
    d_func()->generatedLength = len;
}

void BPasswordWidget::clear()
{
    d_func()->pwd.clear();
    d_func()->updateEdit();
}

void BPasswordWidget::restorePasswordState(const QByteArray &ba)
{
    d_func()->pwd.restore(ba);
    d_func()->updateEdit();
}

void BPasswordWidget::restoreWidgetState(const QByteArray &ba)
{
    QVariantMap m = BeQt::deserialize(ba).toMap();
    setSavePassword(m.value("save_password").toBool());
    setShowPassword(m.value("show_password").toBool());
}

void BPasswordWidget::restoreState(const QByteArray &ba)
{
    QVariantMap m = BeQt::deserialize(ba).toMap();
    restorePasswordState(m.value("password_state").toByteArray());
    restoreWidgetState(m.value("widget_state").toByteArray());
}

BPassword::Mode BPasswordWidget::mode() const
{
    return d_func()->pwd.mode();
}

BPassword BPasswordWidget::password() const
{
    return d_func()->pwd;
}

QString BPasswordWidget::openPassword() const
{
    return d_func()->pwd.openPassword();
}

QByteArray BPasswordWidget::encryptedPassword(int *charCountHint) const
{
    return d_func()->pwd.encryptedPassword(charCountHint);
}

QByteArray BPasswordWidget::encryptedPassword(QCryptographicHash::Algorithm a, int *charCountHint) const
{
    return d_func()->pwd.encryptedPassword(a, charCountHint);
}

int BPasswordWidget::charCountHint(BPassword::Mode mode) const
{
    return d_func()->pwd.charCountHint(mode);
}

QCryptographicHash::Algorithm BPasswordWidget::algorithm() const
{
    return d_func()->pwd.algorithm();
}

bool BPasswordWidget::savePassword() const
{
    return d_func()->save;
}

bool BPasswordWidget::showPassword() const
{
    return d_func()->show;
}

bool BPasswordWidget::savePasswordVisible() const
{
    return d_func()->tbtnSave->isVisible();
}

bool BPasswordWidget::showPasswordVisible() const
{
    return d_func()->tbtnShow->isVisible();
}

bool BPasswordWidget::generatePasswordVisible() const
{
    return d_func()->tbtnGenerate->isVisible();
}

BPasswordWidget::GeneratePasswordFunction BPasswordWidget::generatePasswordFunction() const
{
    return d_func()->generateFunction;
}

int BPasswordWidget::generatedPasswordLength() const
{
    return d_func()->generatedLength;
}

QByteArray BPasswordWidget::savePasswordState(BPassword::Mode mode) const
{
    return d_func()->pwd.save(mode);
}

QByteArray BPasswordWidget::saveWidgetState() const
{
    QVariantMap m;
    m.insert("save_password", savePassword());
    m.insert("show_password", showPassword());
    return BeQt::serialize(m);
}

QByteArray BPasswordWidget::saveState(BPassword::Mode mode) const
{
    QVariantMap m;
    m.insert("password_state", savePasswordState(mode));
    m.insert("widget_state", saveWidgetState());
    return BeQt::serialize(m);
}

/*============================== Public slots ==============================*/

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

void BPasswordWidget::generatePassword()
{
    B_D(BPasswordWidget);
    if (!d->generateFunction)
        return;
    QString pwd = d->generateFunction(d->generatedLength);
    setPassword(pwd);
    emit passwordGenerated(pwd);
}
