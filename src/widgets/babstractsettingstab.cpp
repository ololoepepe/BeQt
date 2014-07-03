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

#include <QWidget>
#include <QFormLayout>
#include <QList>
#include <QIcon>

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

QIcon BAbstractSettingsTab::icon() const
{
    return BApplication::icon("configure");
}

bool BAbstractSettingsTab::hasAdvancedMode() const
{
    return false;
}

bool BAbstractSettingsTab::isInAdvancedMode() const
{
    return false;
}

void BAbstractSettingsTab::setAdvancedMode(bool)
{
    //
}

bool BAbstractSettingsTab::hasDefault() const
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

bool BAbstractSettingsTab::saveSettings()
{
    return true;
}
