/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCodeEditor module of the BeQt library.
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

#ifndef BOPENSAVEEDITORMODULE_P_H
#define BOPENSAVEEDITORMODULE_P_H

class QAction;
class QMenu;
class QString;

#include "bopensaveeditormodule.h"

#include "babstracteditormodule_p.h"

#include <BeQtCore/private/bbase_p.h>

#include <QList>
#include <QObject>
#include <QStringList>

/*============================================================================
================================ BOpenSaveEditorModulePrivate ================
============================================================================*/

class B_CODEEDITOR_EXPORT BOpenSaveEditorModulePrivate : public BAbstractEditorModulePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BOpenSaveEditorModule)
public:
    QAction *actCloseAllFiles;
    QAction *actCloseFile;
    QAction *actNewFile;
    QAction *actOpenFiles;
    QAction *actReopenFile;
    QAction *actSaveAllFiles;
    QAction *actSaveFile;
    QAction *actSaveFileAs;
    QMenu *mnuFileHistory;
public:
    explicit BOpenSaveEditorModulePrivate(BOpenSaveEditorModule *q);
    ~BOpenSaveEditorModulePrivate();
public:
    void checkActions();
    void init();
    void resetFileHistory(const QStringList &list = QStringList());
public Q_SLOTS:
    void codecTriggered(const QString &codecName);
    void fileTriggered();
    void resetFileHistoryMenuToolTip();
    void retranslateUi();
private:
    Q_DISABLE_COPY(BOpenSaveEditorModulePrivate)
};

#endif // BOPENSAVEEDITORMODULE_P_H
