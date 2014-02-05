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
    wasEmpty = true;
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
      tbtnSave->setCheckable(true);
      tbtnSave->setIcon(BApplication::icon("filesave"));
      connect(tbtnSave, SIGNAL(toggled(bool)), this, SLOT(resetSave(bool)));
    hlt->addWidget(tbtnSave);
    tbtnShow = new QToolButton(q);
      tbtnShow->setCheckable(true);
      tbtnShow->setIcon(BApplication::icon("decrypted"));
      tbtnShow->installEventFilter(this);
      connect(tbtnShow, SIGNAL(toggled(bool)), this, SLOT(resetShow(bool)));
    hlt->addWidget(tbtnShow);
    tbtnGenerate = new QToolButton(q);
      tbtnGenerate->setIcon(BApplication::icon("charset"));
      tbtnGenerate->setVisible(false);
      connect(tbtnGenerate, SIGNAL(clicked()), q, SLOT(generatePassword()));
    hlt->addWidget(tbtnGenerate);
    //
    retranslateUi();
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

void BPasswordWidgetPrivate::updateEdit()
{
    if (pwd.isEncrypted())
    {
        ledt->clear();
        if (!pwd.encryptedPassword().isEmpty())
            ledt->setPlaceholderText(QString(pwd.charCountHint() > 0 ? pwd.charCountHint() : 8, '*'));
        else
            ledt->setPlaceholderText("");
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
    tbtnSave->setToolTip(tr("Save password", "tbtn toolTip"));
    tbtnShow->setToolTip(tr("Show password", "tbtn toolTip"));
    tbtnGenerate->setToolTip(tr("Generate password", "tbtn toolTip"));
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

void BPasswordWidgetPrivate::passwordChanged(const QString &password)
{
    B_Q(BPasswordWidget);
    if (wasEmpty && !password.isEmpty() && pwd.isEncrypted())
    {
        pwdBackup = pwd;
        wasEmpty = false;
        pwd.setPassword(password);
    }
    else if (!wasEmpty && password.isEmpty() && pwdBackup.isEncrypted())
    {
        pwd = pwdBackup;
        wasEmpty = true;
    }
    else
    {
        pwd.setPassword(password);
    }
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
    return d_func()->tbtnSave->isChecked();
}

bool BPasswordWidget::showPassword() const
{
    return d_func()->tbtnShow->isChecked();
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
    d_func()->tbtnSave->setChecked(b);
}

void BPasswordWidget::setShowPassword(bool b)
{
    d_func()->tbtnShow->setChecked(b);
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
