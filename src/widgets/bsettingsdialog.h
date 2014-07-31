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

#ifndef BSETTINGSDIALOG_H
#define BSETTINGSDIALOG_H

class BSettingsDialogPrivate;

class BAbstractSettingsTab;

class QStringList;

#include <BeQtCore/BBaseObject>

#include <QDialog>
#include <QList>

/*============================================================================
================================ BSettingsDialog =============================
============================================================================*/

class B_WIDGETS_EXPORT BSettingsDialog : public QDialog, public BBaseObject
{
    Q_OBJECT
    Q_PROPERTY(bool valid READ isValid STORED false)
    B_DECLARE_PRIVATE(BSettingsDialog)
public:
    enum TabNavigation
    {
        ListNavigation = 1,
        TabbedNavigation
    };
public:
    explicit BSettingsDialog(const QList<BAbstractSettingsTab *> &tabs, QWidget *parent = 0);
    explicit BSettingsDialog(const QList<BAbstractSettingsTab *> &tabs, TabNavigation navigation, QWidget *parent = 0);
    explicit BSettingsDialog(BAbstractSettingsTab *tab, QWidget *parent = 0);
    ~BSettingsDialog();
protected:
    explicit BSettingsDialog(BSettingsDialogPrivate &d, QWidget *parent = 0);
public:
    bool isValid() const;
private:
    Q_DISABLE_COPY(BSettingsDialog)
};

#endif // BSETTINGSDIALOG_H
