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

class QVBoxLayout;
class QTabWidget;
class QDialogButtonBox;
class QSplitter;
class QListWidget;
class QStackedWidget;
class QCheckBox;
class QPushButton;

#include "bsettingsdialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QList>
#include <QMap>
#include <QApplication>
#include <QStringList>

/*============================================================================
================================ BSettingsDialogPrivate ======================
============================================================================*/

class B_WIDGETS_EXPORT BSettingsDialogPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSettingsDialog)
public:
    explicit BSettingsDialogPrivate(BSettingsDialog *q, const QList<BAbstractSettingsTab *> &tabs,
                                    BSettingsDialog::Navigation navigation = BSettingsDialog::ListNavigation);
    ~BSettingsDialogPrivate();
public:
    void init();
    BAbstractSettingsTab *currentTab() const;
public:
    const QList<BAbstractSettingsTab *> Tabs;
    const BSettingsDialog::Navigation Navigation;
public:
    bool valid;
    QVBoxLayout *vlt;
      QCheckBox *cboxAdvancedMode;
      QPushButton *btnRestoreDefault;
      QSplitter *hspltr;
        QListWidget *lstwgt;
        QStackedWidget *stkdwgt;
      QTabWidget *twgt;
      QDialogButtonBox *dlgbbox;
        //Ok
        //Cancel
public Q_SLOTS:
    void accepted();
    void cboxAdvancedModeStateChanged(int state);
    void currentChanged(int index = 0);
    void btnRestoreDefaultClicked();
    void updateSize();
private:
    Q_DISABLE_COPY(BSettingsDialogPrivate)
};

#endif // BSETTINGSDIALOG_P_H
