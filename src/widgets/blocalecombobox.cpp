#include "blocalecombobox.h"
#include "bapplication.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbase_p.h>

#include <QCoreApplication>
#include <QLocale>
#include <QIcon>
#include <QString>
#include <QList>
#include <QVariant>
#include <QMetaObject>

#include <QDebug>

/*============================================================================
================================ BLocaleComboBoxPrivate ======================
============================================================================*/

class BLocaleComboBoxPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BLocaleComboBox)
public:
    BLocaleComboBoxPrivate(BLocaleComboBox *q, bool alwaysIncludeEnglish);
    ~BLocaleComboBoxPrivate();
public:
    static QString localeToString(const BApplication::LocaleSupportInfo &info);
    static QIcon iconForLocale(const BApplication::LocaleSupportInfo &info);
public:
    void init();
    void updateAvailableLocales();
public:
    const bool AlwaysIncludeEnglish;
private:
    Q_DISABLE_COPY(BLocaleComboBoxPrivate)
};

/*============================================================================
================================ BLocaleComboBoxPrivate ======================
============================================================================*/

/*============================== Public constructors =======================*/

BLocaleComboBoxPrivate::BLocaleComboBoxPrivate(BLocaleComboBox *q, bool alwaysIncludeEnglish) :
    BBasePrivate(q), AlwaysIncludeEnglish(alwaysIncludeEnglish)
{
    //
}

BLocaleComboBoxPrivate::~BLocaleComboBoxPrivate()
{
    //
}

/*============================== Static public methods =====================*/

QString BLocaleComboBoxPrivate::localeToString(const BApplication::LocaleSupportInfo &info)
{
    QString language = QLocale::languageToString( info.locale.language() );
    QString country = (info.locale.country() != QLocale::AnyCountry) ?
                QLocale::countryToString( info.locale.country() ) : "";
    QString name = info.locale.name();
    return language + (!country.isEmpty() ? " (" + country + ")" : "") + " [" + name + "]";
}

QIcon BLocaleComboBoxPrivate::iconForLocale(const BApplication::LocaleSupportInfo &info)
{
    return (info.supports == info.total) ? BApplication::icon("ok") : BApplication::icon("messagebox_warning");
}

/*============================== Public methods ============================*/

void BLocaleComboBoxPrivate::init()
{
    updateAvailableLocales();
}

void BLocaleComboBoxPrivate::updateAvailableLocales()
{
    B_Q(BLocaleComboBox);
    q->clear();
    QList<BApplication::LocaleSupportInfo> list = BApplication::availableLocales(AlwaysIncludeEnglish);
    foreach (const BApplication::LocaleSupportInfo &info, list)
        q->addItem(iconForLocale(info), localeToString(info), info.locale);
}

/*============================================================================
================================ BLocaleComboBox =============================
============================================================================*/

/*============================== Public constructors =======================*/

BLocaleComboBox::BLocaleComboBox(bool alwaysIncludeEnglish, QWidget *parent) :
    QComboBox(parent), BBase( *new BLocaleComboBoxPrivate(this, alwaysIncludeEnglish) )
{
    d_func()->init();
}

BLocaleComboBox::BLocaleComboBox(QWidget *parent) :
    QComboBox(parent), BBase( *new BLocaleComboBoxPrivate(this, false) )
{
    d_func()->init();
}

BLocaleComboBox::~BLocaleComboBox()
{
    //
}

/*============================== Protected constructors ====================*/

BLocaleComboBox::BLocaleComboBox(BLocaleComboBoxPrivate &d, QWidget *parent) :
    QComboBox(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QLocale BLocaleComboBox::currentLocale() const
{
    return itemData( currentIndex() ).toLocale();
}

/*============================== Public slots ==============================*/

void BLocaleComboBox::setCurrentLocale(const QLocale &locale)
{
    int ind = findData(locale);
    if (ind < 0)
        return;
    setCurrentIndex(ind);
}

void BLocaleComboBox::updateAvailableLocales()
{
    d_func()->updateAvailableLocales();
}
