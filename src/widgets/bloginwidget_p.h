#ifndef BLOGINWIDGET_P_H
#define BLOGINWIDGET_P_H

class BPasswordWidget;

class QLineEdit;
class QComboBox;
class QFormLayout;
class QHBoxLayout;
class QToolButton;

#include "bloginwidget.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BTranslation>

#include <QObject>
#include <QStringList>

/*============================================================================
================================ BLoginWidgetPrivate =========================
============================================================================*/

class B_WIDGETS_EXPORT BLoginWidgetPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BLoginWidget)
public:
    explicit BLoginWidgetPrivate(BLoginWidget *q);
    ~BLoginWidgetPrivate();
public:
    void init();
public:
    bool valid;
    BLoginWidget::AddressType addressType;
    BLoginWidget::PasswordType passwordType;
    bool addressRequired;
    bool portRequired;
    bool loginRequired;
    bool passwordRequired;
    BTranslation addressLabel;
    BTranslation portLabel;
    BTranslation loginLabel;
    BTranslation passwordLabel;
    BTranslation addressPlaceholderText;
    BTranslation portPlaceholderText;
    BTranslation loginPlaceholderText;
    BTranslation passwordPlaceholderText;
    BTranslation removeAddressButtonToolTip;
    QStringList persistentAddresses;
    QFormLayout *flt;
      QLineEdit *ledtAddress;
      QHBoxLayout *hltAddress;
        QComboBox *cmboxAddress;
        QToolButton *tbtnRemoveAddress;
      QLineEdit *ledtPort;
      QLineEdit *ledtLogin;
      QLineEdit *ledtPassword;
      BPasswordWidget *pwdwgt;
public Q_SLOTS:
    void retranslateUi();
    void checkInputs();
    void cmboxAddressCurrentIndexChanged(int index);
    void removeCurrentAddress();
private:
    Q_DISABLE_COPY(BLoginWidgetPrivate)
};

#endif // BLOGINWIDGET_P_H
