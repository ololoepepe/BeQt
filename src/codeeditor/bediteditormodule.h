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

#ifndef BEDITEDITORMODULE_H
#define BEDITEDITORMODULE_H

class BEditEditorModulePrivate;

class BAbstractCodeEditorDocument;
class BCodeEditor;

class QAction;
class QString;

#include "babstracteditormodule.h"
#include "bcodeedit.h"

#include <BeQtCore/BBase>

#include <QList>
#include <QObject>

/*============================================================================
================================ BEditEditorModule ===========================
============================================================================*/

class B_CODEEDITOR_EXPORT BEditEditorModule : public BAbstractEditorModule
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BEditEditorModule)
public:
    enum Action
    {
        CopyAction = 1,
        CutAction,
        PasteAction,
        RedoAction,
        SwitchModeAction,
        UndoAction
    };
    enum ActionGroup
    {
        ClipboardActionGroup = 1,
        UndoRedoActionGroup
    };
public:
    explicit BEditEditorModule(QObject *parent = 0);
    ~BEditEditorModule();
protected:
    explicit BEditEditorModule(BEditEditorModulePrivate &d, QObject *parent = 0);
public:
    QAction *action(int type);
    QList<QAction *> actions(int group, bool extended = false);
    QList<QAction *> actions(bool extended = false);
    QString id() const;
protected:
    void currentDocumentChanged(BAbstractCodeEditorDocument *doc);
    void documentCopyAvailableChanged(bool available);
    void documentCutAvailableChanged(bool available);
    void documentPasteAvailableChanged(bool available);
    void documentRedoAvailableChanged(bool available);
    void documentUndoAvailableChanged(bool available);
    void editModeChanged(BCodeEdit::EditMode mode);
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
private:
    Q_DISABLE_COPY(BEditEditorModule)
};

#endif // BEDITEDITORMODULE_H
