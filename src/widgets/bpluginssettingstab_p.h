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

#ifndef BPLUGINSSETTINGSTAB_P_H
#define BPLUGINSSETTINGSTAB_P_H

class BPluginWrapper;

class QHBoxLayout;
class QListWidget;
class QVBoxLayout;
class QPushButton;
class QListWidgetItem;

#include "bpluginssettingstab.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QApplication>
#include <QList>
#include <QObject>

/*============================================================================
================================ BPluginsSettingsTabPrivate ==================
============================================================================*/

class B_WIDGETS_EXPORT BPluginsSettingsTabPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BPluginsSettingsTab)
public:
    explicit BPluginsSettingsTabPrivate(BPluginsSettingsTab *q);
    ~BPluginsSettingsTabPrivate();
public:
    void init();
public Q_SLOTS:
    void lstwgtCurrentRowChanged(int currentRow);
    void lstwgtItemChanged(QListWidgetItem *item);
    void btnSettingsClicked();
    void btnAboutClicked();
    void btnHelpClicked();
public:
    QList<BPluginWrapper *> plugins;
    QHBoxLayout *hlt;
      QListWidget *lstwgt;
      QVBoxLayout *vlt;
        QPushButton *btnSettings;
        QPushButton *btnAbout;
        QPushButton *btnHelp;
private:
    Q_DISABLE_COPY(BPluginsSettingsTabPrivate)
};

#endif // BPLUGINSSETTINGSTAB_P_H
