#include "bloginwidget.h"
#include "bloginwidget_p.h"
#include "bpasswordwidget.h"
#include "bapplication.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BPassword>
#include <BeQtCore/BTranslation>

#include <QObject>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QValidator>
#include <QByteArray>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QMetaObject>
#include <QHBoxLayout>
#include <QToolButton>
#include <QIcon>
#include <QIntValidator>
#include <QLabel>

#include <QDebug>

/*============================================================================
================================ BLoginWidgetPrivate =========================
============================================================================*/

/*============================== Public constructors =======================*/

BLoginWidgetPrivate::BLoginWidgetPrivate(BLoginWidget *q) :
    BBasePrivate(q)
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
      flt->addRow(loginLabel, ledtLogin);
      ledtPassword = 0;
      pwdwgt = new BPasswordWidget;
        connect(pwdwgt, SIGNAL(passwordChanged()), this, SLOT(checkInputs()));
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

void BLoginWidgetPrivate::retranslateUi()
{
    if (hltAddress)
        BApplication::labelForField<QLabel>(hltAddress)->setText(addressLabel);
    if (ledtAddress)
        ledtAddress->setPlaceholderText(addressPlaceholderText);
    if (cmboxAddress && cmboxAddress->lineEdit())
        cmboxAddress->lineEdit()->setPlaceholderText(addressPlaceholderText);
    if (tbtnRemoveAddress)
        tbtnRemoveAddress->setToolTip(removeAddressButtonToolTip);
    if (ledtPort)
    {
        BApplication::labelForField<QLabel>(ledtPort)->setText(portLabel);
        ledtPort->setPlaceholderText(portPlaceholderText);
    }
    if (ledtLogin)
    {
        BApplication::labelForField<QLabel>(ledtLogin)->setText(loginLabel);
        ledtLogin->setPlaceholderText(loginPlaceholderText);
    }
    if (ledtPassword)
    {
        BApplication::labelForField<QLabel>(ledtPassword)->setText(passwordLabel);
        ledtPassword->setPlaceholderText(passwordPlaceholderText);
    }
    if (pwdwgt)
        BApplication::labelForField<QLabel>(pwdwgt)->setText(passwordLabel);
}

void BLoginWidgetPrivate::checkInputs()
{
    bool nvalid = true;
    if (addressRequired)
    {
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
    if (passwordRequired)
    {
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

/*============================================================================
================================ BLoginWidget ===============================
============================================================================*/

/*============================== Public constructors =======================*/

BLoginWidget::BLoginWidget(QWidget *parent) :
    QWidget(parent), BBase(*new BLoginWidgetPrivate(this))
{
    d_func()->init();
}

BLoginWidget::~BLoginWidget()
{
    //
}

/*============================== Protected constructors ====================*/

BLoginWidget::BLoginWidget(BLoginWidgetPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BLoginWidget::setAddressType(AddressType t, bool required)
{
    if (t == d_func()->addressType)
        return setAddressRequired(required);
    d_func()->addressType = t;
    if (d_func()->ledtAddress)
    {
        QWidget *lbl = BApplication::labelForField<QWidget>(d_func()->ledtAddress);
        delete d_func()->ledtAddress;
        d_func()->ledtAddress = 0;
        delete lbl;
    }
    if (d_func()->cmboxAddress)
    {
        QWidget *lbl = BApplication::labelForField<QWidget>(d_func()->hltAddress);
        delete d_func()->hltAddress;
        d_func()->hltAddress = 0;
        delete d_func()->cmboxAddress;
        d_func()->cmboxAddress = 0;
        delete d_func()->tbtnRemoveAddress;
        d_func()->tbtnRemoveAddress = 0;
        delete lbl;
        d_func()->persistentAddresses.clear();
    }
    if (SimpleAddress == t)
    {
        d_func()->ledtAddress = new QLineEdit;
        d_func()->flt->insertRow(0, d_func()->addressLabel, d_func()->ledtAddress);
        connect(d_func()->ledtAddress, SIGNAL(textChanged(QString)), d_func(), SLOT(checkInputs()));
    }
    else if (StaticComboAddress == t)
    {
        d_func()->cmboxAddress = new QComboBox;
        d_func()->cmboxAddress->setEditable(false);
        d_func()->flt->insertRow(0, d_func()->addressLabel, d_func()->cmboxAddress);
        connect(d_func()->cmboxAddress, SIGNAL(currentIndexChanged(int)), d_func(), SLOT(checkInputs()));
    }
    else if (EditableComboAddress == t)
    {
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
    }
    else
    {
        required = false;
    }
    d_func()->updateTabOrder();
    setAddressRequired(required);
}

void BLoginWidget::setPortEnabled(bool b, bool required)
{
    if ((b && d_func()->ledtPort) || (!b && !d_func()->ledtPort))
        return;
    if (b)
    {
        d_func()->ledtPort = new QLineEdit;
        d_func()->ledtPort->setValidator(new QIntValidator(this));
        int ind = (NoAddress == d_func()->addressType) ? 0 : 1;
        d_func()->flt->insertRow(ind, d_func()->portLabel, d_func()->ledtPort);
        connect(d_func()->ledtPort, SIGNAL(textChanged(QString)), d_func(), SLOT(checkInputs()));
    }
    else
    {
        QWidget *lbl = BApplication::labelForField<QWidget>(d_func()->ledtPort);
        delete d_func()->ledtPort;
        d_func()->ledtPort = 0;
        delete lbl;
        required = false;
    }
    d_func()->updateTabOrder();
    setPortRequired(required);
}

void BLoginWidget::setLoginEnabled(bool b, bool required)
{
    if ((b && d_func()->ledtLogin) || (!b && !d_func()->ledtLogin))
        return;
    if (b)
    {
        d_func()->ledtLogin = new QLineEdit;
        int ind = (NoPassword == d_func()->passwordType) ? d_func()->flt->rowCount() : (d_func()->flt->rowCount() - 1);
        d_func()->flt->insertRow(ind, d_func()->loginLabel, d_func()->ledtLogin);
        connect(d_func()->ledtLogin, SIGNAL(textChanged(QString)), d_func(), SLOT(checkInputs()));
    }
    else
    {
        QWidget *lbl = BApplication::labelForField<QWidget>(d_func()->ledtLogin);
        delete d_func()->ledtLogin;
        d_func()->ledtLogin = 0;
        delete lbl;
        required = false;
    }
    d_func()->updateTabOrder();
    setLoginRequired(required);
}

void BLoginWidget::setPasswordType(PasswordType t, bool required)
{
    if (t == d_func()->passwordType)
        return setPasswordRequired(required);
    d_func()->passwordType = t;
    if (d_func()->ledtPassword)
    {
        QWidget *lbl = BApplication::labelForField<QWidget>(d_func()->ledtPassword);
        delete d_func()->ledtPassword;
        d_func()->ledtPassword = 0;
        delete lbl;
    }
    if (d_func()->pwdwgt)
    {
        QWidget *lbl = BApplication::labelForField<QWidget>(d_func()->pwdwgt);
        delete d_func()->pwdwgt;
        d_func()->pwdwgt = 0;
        delete lbl;
    }
    if (SimplePassword == t)
    {
        d_func()->ledtPassword = new QLineEdit;
        d_func()->flt->addRow(d_func()->passwordLabel, d_func()->ledtPassword);
        connect(d_func()->ledtPassword, SIGNAL(textChanged(QString)), d_func(), SLOT(checkInputs()));
    }
    else if (SecurePassword == t)
    {
        d_func()->pwdwgt = new BPasswordWidget;
        d_func()->flt->addRow(d_func()->passwordLabel, d_func()->pwdwgt);
        connect(d_func()->pwdwgt, SIGNAL(passwordChanged()), d_func(), SLOT(checkInputs()));
    }
    else
    {
        required = false;
    }
    d_func()->updateTabOrder();
    setPasswordRequired(required);
}

void BLoginWidget::setAddressRequired(bool b)
{
    d_func()->addressRequired = b;
    d_func()->checkInputs();
}

void BLoginWidget::setPortRequired(bool b)
{
    d_func()->portRequired = b;
    d_func()->checkInputs();
}

void BLoginWidget::setLoginRequired(bool b)
{
    d_func()->loginRequired = b;
    d_func()->checkInputs();
}

void BLoginWidget::setPasswordRequired(bool b)
{
    d_func()->passwordRequired = b;
    d_func()->checkInputs();
}

void BLoginWidget::setAddressLabel(const BTranslation &t)
{
    d_func()->addressLabel = t;
}

void BLoginWidget::setPortLabel(const BTranslation &t)
{
    d_func()->portLabel = t;
}

void BLoginWidget::setLoginLabel(const BTranslation &t)
{
    d_func()->loginLabel = t;
}

void BLoginWidget::setPasswordLabelconst (BTranslation &t)
{
    d_func()->passwordLabel = t;
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

void BLoginWidget::setAddressInputMask(const QString &mask)
{
    QLineEdit *ledt = d_func()->cmboxAddress ? d_func()->cmboxAddress->lineEdit() : d_func()->ledtAddress;
    if (!ledt)
        return;
    ledt->setInputMask(mask);
}

void BLoginWidget::setAddressValidator(QValidator *v)
{
    QLineEdit *ledt = d_func()->cmboxAddress ? d_func()->cmboxAddress->lineEdit() : d_func()->ledtAddress;
    if (!ledt)
        return;
    ledt->setValidator(v);
}

void BLoginWidget::setAddressPlaceholderText(const BTranslation &t)
{
    d_func()->addressPlaceholderText = t;
    QLineEdit *ledt = d_func()->cmboxAddress ? d_func()->cmboxAddress->lineEdit() : d_func()->ledtAddress;
    if (!ledt)
        return;
    ledt->setPlaceholderText(t);
}

void BLoginWidget::setAddress(const QString &a)
{
    switch (d_func()->addressType)
    {
    case SimpleAddress:
        d_func()->ledtAddress->setText(a);
        break;
    case StaticComboAddress:
    {
        int ind = d_func()->cmboxAddress->findText(a, Qt::MatchFixedString);
        if (ind >= 0)
            d_func()->cmboxAddress->setCurrentIndex(ind);
        break;
    }
    case EditableComboAddress:
    {
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

void BLoginWidget::setPortInputMask(const QString &mask)
{
    if (!d_func()->ledtPort)
        return;
    d_func()->ledtPort->setInputMask(mask);
}

void BLoginWidget::setPortPlaceholderText(const BTranslation &t)
{
    d_func()->portPlaceholderText = t;
    if (!d_func()->ledtPort)
        return;
    d_func()->ledtPort->setPlaceholderText(t);
}

void BLoginWidget::setPort(int port)
{
    if (port < 0 || !d_func()->ledtPort)
        return;
    d_func()->ledtPort->setText(QString::number(port));
}

void BLoginWidget::setLoginInputMask(const QString &mask)
{
    if (!d_func()->ledtLogin)
        return;
    d_func()->ledtLogin->setInputMask(mask);
}

void BLoginWidget::setLoginValidator(QValidator *v)
{
    if (!d_func()->ledtLogin)
        return;
    d_func()->ledtLogin->setValidator(v);
}

void BLoginWidget::setLoginPlaceholderText(const BTranslation &t)
{
    d_func()->loginPlaceholderText = t;
    if (!d_func()->ledtLogin)
        return;
    d_func()->ledtLogin->setPlaceholderText(t);
}

void BLoginWidget::setLogin(const QString &l)
{
    if (!d_func()->ledtLogin)
        return;
    d_func()->ledtLogin->setText(l);
}

void BLoginWidget::setPasswordPlaceholderText(const BTranslation &t)
{
    d_func()->passwordPlaceholderText = t;
    if (!d_func()->ledtPassword)
        return;
    d_func()->ledtPassword->setPlaceholderText(t);
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

void BLoginWidget::setGeneratePasswordVisible(bool visible)
{
    if (!d_func()->pwdwgt)
        return;
    d_func()->pwdwgt->setGeneratePasswordVisible(visible);
}

void BLoginWidget::setGeneratePasswordFunction(BPasswordWidget::GeneratePasswordFunction f)
{
    if (!d_func()->pwdwgt)
        return;
    d_func()->pwdwgt->setGeneratePasswordFunction(f);
}

void BLoginWidget::setGeneratedPasswordLength(int len)
{
    if (!d_func()->pwdwgt)
        return;
    d_func()->pwdwgt->setGeneratedPasswordLength(len);
}

void BLoginWidget::restorePasswordWidgetState(const QByteArray &state)
{
    if (!d_func()->pwdwgt)
        return;
    d_func()->pwdwgt->restoreWidgetState(state);
}

QByteArray BLoginWidget::savePasswordWidgetState() const
{
    return d_func()->pwdwgt ? d_func()->pwdwgt->saveWidgetState() : QByteArray();
}

BLoginWidget::AddressType BLoginWidget::addressType() const
{
    return d_func()->addressType;
}

bool BLoginWidget::portEnabled() const
{
    return d_func()->ledtPort;
}

bool BLoginWidget::loginEnabled() const
{
    return d_func()->ledtLogin;
}

BLoginWidget::PasswordType BLoginWidget::passwordType() const
{
    return d_func()->passwordType;
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

QString BLoginWidget::address() const
{
    switch (d_func()->addressType)
    {
    case SimpleAddress:
        return d_func()->ledtAddress->text();
    case StaticComboAddress:
    case EditableComboAddress:
        return d_func()->cmboxAddress->currentText();
    default:
        return QString();
    }
}

int BLoginWidget::port() const
{
    return d_func()->ledtPort ? d_func()->ledtPort->text().toInt() : 0;
}

QString BLoginWidget::login() const
{
    return d_func()->ledtLogin ? d_func()->ledtLogin->text() : QString();
}

QString BLoginWidget::simplePassword() const
{
    return (SimplePassword == d_func()->passwordType) ? d_func()->ledtPassword->text() : QString();
}

BPassword BLoginWidget::securePassword() const
{
    return (SecurePassword == d_func()->passwordType) ? d_func()->pwdwgt->password() : BPassword();
}

bool BLoginWidget::hasValidInput() const
{
    return d_func()->valid;
}
