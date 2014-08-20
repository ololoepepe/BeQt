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

#ifndef BSETTINGSDIALOG_P_H
#define BSETTINGSDIALOG_P_H

class QCheckBox;
class QDialogButtonBox;
class QListWidget;
class QPushButton;
class QSplitter;
class QStackedWidget;
class QTabWidget;

#include "bsettingsdialog.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QList>
#include <QObject>

/*============================================================================
================================ BSettingsDialogPrivate ======================
============================================================================*/

class B_WIDGETS_EXPORT BSettingsDialogPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSettingsDialog)
public:
    const BSettingsDialog::TabNavigation Navigation;
    const QList<BAbstractSettingsTab *> Tabs;
public:
    QPushButton *btnRestoreDefault;
    QCheckBox *cboxAdvancedMode;
    QDialogButtonBox *dlgbbox;
    QSplitter *hspltr;
    QListWidget *lstwgt;
    QStackedWidget *stkdwgt;
    QTabWidget *twgt;
    bool valid;
public:
    explicit BSettingsDialogPrivate(BSettingsDialog *q, const QList<BAbstractSettingsTab *> &tabs,
                                    BSettingsDialog::TabNavigation navigation = BSettingsDialog::ListNavigation);
    ~BSettingsDialogPrivate();
public:
    void init();
public Q_SLOTS:
    void accepted();
    void btnRestoreDefaultClicked();
    void cboxAdvancedModeStateChanged(int state);
    void currentChanged(int index = 0);
    void updateSize();
private:
    Q_DISABLE_COPY(BSettingsDialogPrivate)
};

#endif // BSETTINGSDIALOG_P_H
