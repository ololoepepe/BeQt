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

class BCodeEditor;

class QAction;
class QString;
class QMenu;

#include "bopensaveeditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditor.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QPointer>
#include <QList>
#include <QStringList>

/*============================================================================
================================ BOpenSaveEditorModulePrivate ================
============================================================================*/

class B_CODEEDITOR_EXPORT BOpenSaveEditorModulePrivate : public BAbstractEditorModulePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BOpenSaveEditorModule)
public:
    explicit BOpenSaveEditorModulePrivate(BOpenSaveEditorModule *q);
    ~BOpenSaveEditorModulePrivate();
public:
    void init();
    void checkActions();
    void resetFileHistory( const QStringList &list = QStringList() );
public Q_SLOTS:
    void retranslateUi();
    void codecTriggered(const QString &codecName);
    void fileTriggered();
    void resetFileHistoryMenuToolTip();
public:
    QPointer<QAction> actNewFile;
    QPointer<QAction> actOpenFiles;
    QPointer<QAction> actReopenFile;
    QPointer<QAction> actSaveFile;
    QPointer<QAction> actSaveFileAs;
    QPointer<QAction> actSaveAllFiles;
    QPointer<QAction> actCloseFile;
    QPointer<QAction> actCloseAllFiles;
    QPointer<QMenu> mnuFileHistory;
private:
    Q_DISABLE_COPY(BOpenSaveEditorModulePrivate)
};

#endif // BOPENSAVEEDITORMODULE_P_H
