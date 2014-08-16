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

class QByteArray;
class QIcon;
class QString;

#include <BeQtCore/BeQtGlobal>

#include <QWidget>

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
    virtual bool hasAdvancedMode() const;
    virtual bool hasDefault() const;
    virtual QIcon icon() const;
    virtual QString id() const = 0;
    virtual bool isInAdvancedMode() const;
    virtual bool restoreDefault();
    virtual QString restoreDefaultHint() const;
    virtual void restoreState(const QByteArray &state);
    virtual bool saveSettings();
    virtual QByteArray saveState() const;
    virtual void setAdvancedMode(bool enabled);
    virtual QString title() const = 0;
private:
    Q_DISABLE_COPY(BAbstractSettingsTab)
};

#endif // BABSTRACTSETTINGSTAB_H
