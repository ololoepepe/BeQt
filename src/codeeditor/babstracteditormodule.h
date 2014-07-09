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

#ifndef BABSTRACTEDITORMODULE_H
#define BABSTRACTEDITORMODULE_H

class BAbstractEditorModulePrivate;

class BAbstractCodeEditorDocument;
class BAbstractFileType;
class BCodeEditor;
class BCodeEditorPrivate;

class QAction;
class QByteArray;
class QPoint;
class QString;
class QStringList;
class QWidget;

#include "bcodeedit.h"

#include <BeQtCore/BBaseObject>

#include <QList>
#include <QObject>

/*============================================================================
================================ BAbstractEditorModule =======================
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractEditorModule : public QObject, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BAbstractEditorModule)
public:
    explicit BAbstractEditorModule(QObject *parent = 0);
    ~BAbstractEditorModule();
protected:
    explicit BAbstractEditorModule(BAbstractEditorModulePrivate &d, QObject *parent = 0);
public:
    virtual QAction *action(int type);
    virtual QList<QAction *> actions(int group, bool extended = false);
    virtual QList<QAction *> actions(bool extended = false);
    BCodeEditor *editor() const;
    virtual QString id() const = 0;
    virtual bool isBuisy() const;
    virtual void restoreState(const QByteArray &state);
    virtual QByteArray saveState() const;
    virtual QWidget *widget(int type);
    virtual QList<QWidget *> widgets(int group, bool extended = false);
    virtual QList<QWidget *> widgets(bool extended = false);
protected:
    BAbstractCodeEditorDocument *currentDocument() const;
    virtual void currentDocumentChanged(BAbstractCodeEditorDocument *doc);
    virtual void defaultCodecChanged(const QString &codecName);
    virtual void documentAboutToBeAdded(BAbstractCodeEditorDocument *doc);
    virtual void documentAboutToBeRemoved(BAbstractCodeEditorDocument *doc);
    virtual void documentAdded(BAbstractCodeEditorDocument *doc);
    virtual void documentBuisyChanged(bool buisy);
    virtual void documentCodecChanged(const QString &codecName);
    virtual void documentCopyAvailableChanged(bool available);
    virtual void documentCursorPositionChanged(int pos);
    virtual void documentCursorPositionChanged(const QPoint &pos);
    virtual void documentCutAvailableChanged(bool available);
    virtual void documentEditModeChanged(BCodeEdit::EditMode mode);
    virtual void documentFileNameChanged(const QString &fn);
    virtual void documentFileTypeChanged(BAbstractFileType *ft);
    virtual void documentHasSelectionChanged(bool hasSelection);
    virtual void documentModificationChanged(bool modified);
    virtual void documentPasteAvailableChanged(bool available);
    virtual void documentReadOnlyChanged(bool ro);
    virtual void documentRedoAvailableChanged(bool available);
    virtual void documentRemoved(const QString &fileName);
    QList<BAbstractCodeEditorDocument *> documents() const;
    virtual void documentSelectionChanged();
    virtual void documentUndoAvailableChanged(bool available);
    virtual void editModeChanged(BCodeEdit::EditMode mode);
    virtual void editorSet(BCodeEditor *edr);
    virtual void editorUnset(BCodeEditor *edr);
    virtual void fileHistoryChanged(const QStringList &list);
    virtual void fileTypesChanged();
private:
    Q_DISABLE_COPY(BAbstractEditorModule)
    friend class BCodeEditorPrivate;
};

#endif // BABSTRACTEDITORMODULE_H
