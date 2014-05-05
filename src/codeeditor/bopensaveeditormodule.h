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

#ifndef BOPENSAVEEDITORMODULE_H
#define BOPENSAVEEDITORMODULE_H

#include "babstracteditormodule.h"

class BOpenSaveEditorModulePrivate;
class BCodeEditor;
class BAbstractCodeEditorDocument;

class QString;
class QAction;
class QMenu;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QList>

/*============================================================================
================================ BOpenSaveEditorModule =======================
============================================================================*/

class B_CODEEDITOR_EXPORT BOpenSaveEditorModule : public BAbstractEditorModule
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BOpenSaveEditorModule)
public:
    enum Action
    {
        NewFileAction,
        OpenFilesAction,
        ReopenAction,
        SaveFileAction,
        SaveFileAsAction,
        SaveAllFilesAction,
        CloseFileAction,
        CloseAllFilesAction
    };
    enum ActionGroup
    {
        OpenActionGroup,
        SaveActionGroup,
        CloseActionGroup
    };
public:
    explicit BOpenSaveEditorModule(QObject *parent = 0);
    ~BOpenSaveEditorModule();
protected:
    explicit BOpenSaveEditorModule(BOpenSaveEditorModulePrivate &d, QObject *parent = 0);
public:
    QString id() const;
    QAction *action(int type);
    QList<QAction *> actions(int group, bool extended = false);
    QList<QAction *> actions(bool extended = false);
    QMenu *fileHistoryMenu();
    QList<QAction *> fileHistoryActions();
protected:
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void documentReadOnlyChanged(bool ro);
    void documentModificationChanged(bool modified);
    void documentBuisyChanged(bool buisy);
    void currentDocumentChanged(BAbstractCodeEditorDocument *doc);
    void fileHistoryChanged(const QStringList &list);
private:
    Q_DISABLE_COPY(BOpenSaveEditorModule)
};

#endif // BOPENSAVEEDITORMODULE_H
