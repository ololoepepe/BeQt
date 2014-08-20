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

class QListWidget;
class QListWidgetItem;
class QPushButton;

#include "bpluginssettingstab.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QObject>
#include <QString>

/*============================================================================
================================ BPluginsSettingsTabPrivate ==================
============================================================================*/

class B_WIDGETS_EXPORT BPluginsSettingsTabPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BPluginsSettingsTab)
public:
    QPushButton *btnAbout;
    QPushButton *btnHelp;
    QPushButton *btnSettings;
    QListWidget *lstwgt;
    QList<BPluginWrapper *> plugins;
    QMap<QString, QByteArray> pluginSettingsDialogStates;
public:
    explicit BPluginsSettingsTabPrivate(BPluginsSettingsTab *q);
    ~BPluginsSettingsTabPrivate();
public:
    void init();
public Q_SLOTS:
    void btnAboutClicked();
    void btnHelpClicked();
    void btnSettingsClicked();
    void lstwgtCurrentRowChanged(int currentRow);
    void lstwgtItemChanged(QListWidgetItem *item);
private:
    Q_DISABLE_COPY(BPluginsSettingsTabPrivate)
};

#endif // BPLUGINSSETTINGSTAB_P_H
