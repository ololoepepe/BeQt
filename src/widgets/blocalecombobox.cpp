/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtWidgets module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "blocalecombobox.h"

#include "bapplication.h"

#include <BeQtCore/private/bbase_p.h>

#include <QDebug>
#include <QIcon>
#include <QList>
#include <QLocale>
#include <QMetaObject>
#include <QString>
#include <QVariant>

/*============================================================================
================================ BLocaleComboBoxPrivate ======================
============================================================================*/

class BLocaleComboBoxPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BLocaleComboBox)
public:
    QList<BApplicationBase::LocaleSupportInfo> availableLocales;
public:
    explicit BLocaleComboBoxPrivate(BLocaleComboBox *q);
    ~BLocaleComboBoxPrivate();
public:
    static QIcon iconForLocale(const BApplication::LocaleSupportInfo &info);
    static QString localeToString(const BApplication::LocaleSupportInfo &info);
public:
    void init();
private:
    Q_DISABLE_COPY(BLocaleComboBoxPrivate)
};

/*============================================================================
================================ BLocaleComboBoxPrivate ======================
============================================================================*/

/*============================== Public constructors =======================*/

BLocaleComboBoxPrivate::BLocaleComboBoxPrivate(BLocaleComboBox *q) :
    BBasePrivate(q)
{
    //
}

BLocaleComboBoxPrivate::~BLocaleComboBoxPrivate()
{
    //
}

/*============================== Static public methods =====================*/

QIcon BLocaleComboBoxPrivate::iconForLocale(const BApplication::LocaleSupportInfo &info)
{
    return (info.total > 0 && info.supports >= info.total) ? BApplication::icon("ok") :
                                                             BApplication::icon("messagebox_warning");
}

QString BLocaleComboBoxPrivate::localeToString(const BApplication::LocaleSupportInfo &info)
{
    QString language = info.locale.nativeLanguageName();
    QString country = (info.locale.country() != QLocale::AnyCountry) ? info.locale.nativeCountryName(): QString("");
    QString name = info.locale.name();
    return language + (!country.isEmpty() ? " (" + country + ")" : "") + " [" + name + "]";
}

/*============================== Public methods ============================*/

void BLocaleComboBoxPrivate::init()
{
    q_func()->setAvailableLocales(availableLocales);
}

/*============================================================================
================================ BLocaleComboBox =============================
============================================================================*/

/*============================== Public constructors =======================*/

BLocaleComboBox::BLocaleComboBox(QWidget *parent) :
    QComboBox(parent), BBase(*new BLocaleComboBoxPrivate(this))
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

QList<BApplicationBase::LocaleSupportInfo> BLocaleComboBox::availableLocales() const
{
    return d_func()->availableLocales;
}

QLocale BLocaleComboBox::currentLocale() const
{
    QVariant v = (currentIndex() >= 0) ? itemData(currentIndex()) : QVariant();
    return v.canConvert(QVariant::Locale) ? v.toLocale() : QLocale(QLocale::English);
}

/*============================== Public slots ==============================*/

void BLocaleComboBox::setAvailableLocales(const QList<BApplicationBase::LocaleSupportInfo> &list)
{
    blockSignals(true);
    QLocale l = currentLocale();
    clear();
    bool b = false;
    d_func()->availableLocales = list;
    foreach (const BApplication::LocaleSupportInfo &info, list) {
        if (info.locale == l) {
            b = true;
            break;
        }
    }
    if (!b)
        blockSignals(false);
    foreach (const BApplication::LocaleSupportInfo &info, list) {
        QIcon icon = BLocaleComboBoxPrivate::iconForLocale(info);
        QString text = BLocaleComboBoxPrivate::localeToString(info);
        addItem(icon, text, info.locale);
    }
    setCurrentLocale(l);
    blockSignals(false);
}

void BLocaleComboBox::setCurrentLocale(const QLocale &locale)
{
    int ind = findData(locale);
    if (ind < 0)
        return;
    setCurrentIndex(ind);
}
