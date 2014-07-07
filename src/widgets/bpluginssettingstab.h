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

#ifndef BPLUGINSSETTINGSTAB_H
#define BPLUGINSSETTINGSTAB_H

class BPluginsSettingsTabPrivate;

class QIcon;
class QString;

#include "babstractsettingstab.h"

#include <BeQtCore/BBaseObject>

/*============================================================================
================================ BPluginsSettingsTab =========================
============================================================================*/

class B_WIDGETS_EXPORT BPluginsSettingsTab : public BAbstractSettingsTab, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPluginsSettingsTab)
public:
    explicit BPluginsSettingsTab();
    ~BPluginsSettingsTab();
protected:
    explicit BPluginsSettingsTab(BPluginsSettingsTabPrivate &d);
public:
    QIcon icon() const;
    QString title() const;
private:
    Q_DISABLE_COPY(BPluginsSettingsTab)
};

#endif // BPLUGINSSETTINGSTAB_H
