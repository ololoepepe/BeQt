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

#ifndef BGUIPLUGININTERFACE_H
#define BGUIPLUGININTERFACE_H

class BAboutDialog;
class BAbstractSettingsTab;

class QPixmap;
class QString;
class QStringList;

#include <BeQtCore/BeQtGlobal>

#include <QtPlugin>

/*============================================================================
================================ BGuiPluginInterface =========================
============================================================================*/

class B_WIDGETS_EXPORT BGuiPluginInterface
{
public:
    virtual ~BGuiPluginInterface() {}
public:
    virtual QPixmap pixmap() const = 0;
    virtual BAbstractSettingsTab *createSettingsTab() = 0;
    virtual QStringList helpSearchPaths() const = 0;
    virtual QString helpIndex() const = 0;
    virtual BAboutDialog *createAboutDialog() = 0;
    virtual void processStandardAboutDialog(BAboutDialog *dlg) const = 0;
};

Q_DECLARE_INTERFACE(BGuiPluginInterface, "BeQt.BGuiPluginInterface")

#endif // BGUIPLUGININTERFACE_H
