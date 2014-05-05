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

#ifndef BABSTRACTSETTINGSTAB_H
#define BABSTRACTSETTINGSTAB_H

class QString;
class QIcon;
class QFormLayout;
class QLayout;

#include <BeQtCore/BeQtGlobal>
#include "bapplication.h"

#include <QWidget>
#include <QtGlobal>

/*============================================================================
================================ BAbstractSettingsTab ========================
============================================================================*/

class B_WIDGETS_EXPORT BAbstractSettingsTab : public QWidget
{
    Q_OBJECT
public:
    explicit BAbstractSettingsTab();
    ~BAbstractSettingsTab();
public:
    template <typename T> static T *labelForField(QWidget *field);
    template <typename T> static T *labelForField(QLayout *field);
public:
    static void setRowVisible(QWidget *field, bool visible);
    static void setRowVisible(QLayout *field, bool visible);
    static void setRowEnabled(QWidget *field, bool enabled);
    static void setRowEnabled(QLayout *field, bool enabled);
public:
    virtual QString title() const = 0;
    virtual QIcon icon() const;
    virtual bool hasAdvancedMode() const;
    virtual bool isInAdvancedMode() const;
    virtual void setAdvancedMode(bool enabled);
    virtual bool hasDefault() const;
    virtual bool restoreDefault();
    virtual QString restoreDefaultHint() const;
    virtual bool saveSettings();
private:
    Q_DISABLE_COPY(BAbstractSettingsTab)
};

/*============================================================================
================================ BAbstractSettingsTab ========================
============================================================================*/

/*============================== Static public template methods=============*/

template <typename T> T *BAbstractSettingsTab::labelForField(QWidget *field)
{
    return BApplication::labelForField<T>(field);
}

template <typename T> T *BAbstractSettingsTab::labelForField(QLayout *field)
{
    return BApplication::labelForField<T>(field);
}

#endif // BABSTRACTSETTINGSTAB_H
