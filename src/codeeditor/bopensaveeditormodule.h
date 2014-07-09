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

class BOpenSaveEditorModulePrivate;

class BAbstractCodeEditorDocument;
class BCodeEditor;

class QAction;
class QMenu;
class QString;

#include "babstracteditormodule.h"

#include <BeQtCore/BBase>

#include <QList>
#include <QObject>

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
        CloseAllFilesAction = 1,
        CloseFileAction,
        NewFileAction,
        OpenFilesAction,
        ReopenAction,
        SaveAllFilesAction,
        SaveFileAction,
        SaveFileAsAction
    };
    enum ActionGroup
    {
        CloseActionGroup = 1,
        OpenActionGroup,
        SaveActionGroup
    };
public:
    explicit BOpenSaveEditorModule(QObject *parent = 0);
    ~BOpenSaveEditorModule();
protected:
    explicit BOpenSaveEditorModule(BOpenSaveEditorModulePrivate &d, QObject *parent = 0);
public:
    QAction *action(int type);
    QList<QAction *> actions(int group, bool extended = false);
    QList<QAction *> actions(bool extended = false);
    QList<QAction *> fileHistoryActions();
    QMenu *fileHistoryMenu();
    QString id() const;
protected:
    void currentDocumentChanged(BAbstractCodeEditorDocument *doc);
    void documentBuisyChanged(bool buisy);
    void documentModificationChanged(bool modified);
    void documentReadOnlyChanged(bool ro);
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void fileHistoryChanged(const QStringList &list);
private:
    Q_DISABLE_COPY(BOpenSaveEditorModule)
};

#endif // BOPENSAVEEDITORMODULE_H
