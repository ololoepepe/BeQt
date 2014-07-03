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
class BCodeEditor;
class BCodeEditorPrivate;
class BAbstractCodeEditorDocument;
class BAbstractFileType;

class QString;
class QStringList;
class QAction;
class QWidget;
class QByteArray;

#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBaseObject>

#include <QObject>
#include <QPoint>
#include <QList>

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
    virtual QString id() const = 0;
    virtual bool isBuisy() const;
    virtual QAction *action(int type);
    virtual QList<QAction *> actions(int group, bool extended = false);
    virtual QList<QAction *> actions(bool extended = false);
    virtual QWidget *widget(int type);
    virtual QList<QWidget *> widgets(int group, bool extended = false);
    virtual QList<QWidget *> widgets(bool extended = false);
    virtual QByteArray saveState() const;
    virtual void restoreState(const QByteArray &state);
    BCodeEditor *editor() const;
protected:
    virtual void editorSet(BCodeEditor *edr);
    virtual void editorUnset(BCodeEditor *edr);
    virtual void documentReadOnlyChanged(bool ro);
    virtual void documentModificationChanged(bool modified);
    virtual void documentSelectionChanged();
    virtual void documentHasSelectionChanged(bool hasSelection);
    virtual void documentCutAvailableChanged(bool available);
    virtual void documentCopyAvailableChanged(bool available);
    virtual void documentPasteAvailableChanged(bool available);
    virtual void documentUndoAvailableChanged(bool available);
    virtual void documentRedoAvailableChanged(bool available);
    virtual void documentEditModeChanged(BCodeEdit::EditMode mode);
    virtual void documentCursorPositionChanged(const QPoint &pos);
    virtual void documentBuisyChanged(bool buisy);
    virtual void documentFileNameChanged(const QString &fn);
    virtual void documentCodecChanged(const QString &codecName);
    virtual void documentFileTypeChanged(BAbstractFileType *ft);
    virtual void defaultCodecChanged(const QString &codecName);
    virtual void editModeChanged(BCodeEdit::EditMode mode);
    virtual void documentAboutToBeAdded(BAbstractCodeEditorDocument *doc);
    virtual void documentAdded(BAbstractCodeEditorDocument *doc);
    virtual void documentAboutToBeRemoved(BAbstractCodeEditorDocument *doc);
    virtual void documentRemoved(const QString &fileName);
    virtual void currentDocumentChanged(BAbstractCodeEditorDocument *doc);
    virtual void fileTypesChanged();
    virtual void fileHistoryChanged(const QStringList &list);
    BAbstractCodeEditorDocument *currentDocument() const;
    QList<BAbstractCodeEditorDocument *> documents() const;
private:
    Q_DISABLE_COPY(BAbstractEditorModule)
    friend class BCodeEditorPrivate;
};

#endif // BABSTRACTEDITORMODULE_H
