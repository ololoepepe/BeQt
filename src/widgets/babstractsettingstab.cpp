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

#include "babstractsettingstab.h"

#include "bapplication.h"

#include <QByteArray>
#include <QIcon>
#include <QString>
#include <QWidget>

/*============================================================================
================================ BAbstractSettingsTab ========================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractSettingsTab::BAbstractSettingsTab() :
    QWidget(0)
{
    //
}

BAbstractSettingsTab::~BAbstractSettingsTab()
{
    //
}

/*============================== Public methods ============================*/

bool BAbstractSettingsTab::hasAdvancedMode() const
{
    return false;
}

bool BAbstractSettingsTab::hasDefault() const
{
    return false;
}

QIcon BAbstractSettingsTab::icon() const
{
    return BApplication::icon("configure");
}

bool BAbstractSettingsTab::isInAdvancedMode() const
{
    return false;
}

bool BAbstractSettingsTab::restoreDefault()
{
    return false;
}

QString BAbstractSettingsTab::restoreDefaultHint() const
{
    return "";
}

void BAbstractSettingsTab::restoreState(const QByteArray &)
{
    //
}

bool BAbstractSettingsTab::saveSettings()
{
    return true;
}

QByteArray BAbstractSettingsTab::saveState() const
{
    return QByteArray();
}

void BAbstractSettingsTab::setAdvancedMode(bool)
{
    //
}
