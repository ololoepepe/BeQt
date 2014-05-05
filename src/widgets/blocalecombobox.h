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

#ifndef BLOCALECOMBOBOX_H
#define BLOCALECOMBOBOX_H

class BLocaleComboBoxPrivate;

class QWidget;
class QLocale;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QComboBox>

/*============================================================================
================================ BLocaleComboBox =============================
============================================================================*/

class B_WIDGETS_EXPORT BLocaleComboBox : public QComboBox, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BLocaleComboBox)
public:
    explicit BLocaleComboBox(QWidget *parent = 0);
    explicit BLocaleComboBox(bool alwaysIncludeEnglish, QWidget *parent = 0);
    ~BLocaleComboBox();
protected:
    explicit BLocaleComboBox(BLocaleComboBoxPrivate &d, QWidget *parent = 0);
public:
    QLocale currentLocale() const;
public Q_SLOTS:
    void setCurrentLocale(const QLocale &locale);
    void updateAvailableLocales();
Q_SIGNALS:
    void currentLocaleChanged(const QLocale &locale);
private:
    Q_DISABLE_COPY(BLocaleComboBox)
};

#endif // BLOCALECOMBOBOX_H
