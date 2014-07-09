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

class QAction;
class QPoint;
class QString;
class QStringList;
class QWidget;

#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"

#include "bcodeedit.h"
#include "bcodeeditor.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QByteArray>
#include <QDebug>
#include <QList>
#include <QObject>

/*============================================================================
================================ BAbstractEditorModulePrivate ================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractEditorModulePrivate::BAbstractEditorModulePrivate(BAbstractEditorModule *q) :
    BBaseObjectPrivate(q)
{
    //
}

BAbstractEditorModulePrivate::~BAbstractEditorModulePrivate()
{
    //
}

/*============================== Public methods ============================*/

void BAbstractEditorModulePrivate::init()
{
    editor = 0;
}

void BAbstractEditorModulePrivate::setEditor(BCodeEditor *edr)
{
    if (edr == editor)
        return;
    B_Q(BAbstractEditorModule);
    q->editorUnset(editor);
    editor = edr;
    q->editorSet(edr);
}

/*============================================================================
================================ BAbstractEditorModule =======================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractEditorModule::BAbstractEditorModule(QObject *parent) :
    QObject(parent), BBaseObject(*new BAbstractEditorModulePrivate(this))
{
    d_func()->init();
}

BAbstractEditorModule::~BAbstractEditorModule()
{
    //
}

/*============================== Protected constructors ====================*/

BAbstractEditorModule::BAbstractEditorModule(BAbstractEditorModulePrivate &d, QObject *parent) :
    QObject(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QAction *BAbstractEditorModule::action(int)
{
    return 0;
}

QList<QAction *> BAbstractEditorModule::actions(int, bool)
{
    return QList<QAction *>();
}

QList<QAction *> BAbstractEditorModule::actions(bool)
{
    return QList<QAction *>();
}

BCodeEditor *BAbstractEditorModule::editor() const
{
    return d_func()->editor;
}

bool BAbstractEditorModule::isBuisy() const
{
    return false;
}

void BAbstractEditorModule::restoreState(const QByteArray &)
{
    //
}

QByteArray BAbstractEditorModule::saveState() const
{
    return QByteArray();
}

QWidget *BAbstractEditorModule::widget(int)
{
    return 0;
}

QList<QWidget *> BAbstractEditorModule::widgets(int, bool)
{
    return QList<QWidget *>();
}

QList<QWidget *> BAbstractEditorModule::widgets(bool)
{
    return QList<QWidget *>();
}

/*============================== Protected methods =========================*/

BAbstractCodeEditorDocument *BAbstractEditorModule::currentDocument() const
{
    return editor() ? editor()->currentDocument() : 0;
}

void BAbstractEditorModule::currentDocumentChanged(BAbstractCodeEditorDocument *)
{
    //
}

void BAbstractEditorModule::defaultCodecChanged(const QString &)
{
    //
}

void BAbstractEditorModule::documentAboutToBeAdded(BAbstractCodeEditorDocument *)
{
    //
}

void BAbstractEditorModule::documentAboutToBeRemoved(BAbstractCodeEditorDocument *)
{
    //
}

void BAbstractEditorModule::documentAdded(BAbstractCodeEditorDocument *)
{
    //
}

void BAbstractEditorModule::documentBuisyChanged(bool)
{
    //
}

void BAbstractEditorModule::documentCodecChanged(const QString &)
{
    //
}

void BAbstractEditorModule::documentCopyAvailableChanged(bool)
{
    //
}

void BAbstractEditorModule::documentCursorPositionChanged(int)
{
    //
}

void BAbstractEditorModule::documentCursorPositionChanged(const QPoint &)
{
    //
}

void BAbstractEditorModule::documentCutAvailableChanged(bool)
{
    //
}

void BAbstractEditorModule::documentEditModeChanged(BCodeEdit::EditMode)
{
    //
}

void BAbstractEditorModule::documentFileNameChanged(const QString &)
{
    //
}

void BAbstractEditorModule::documentFileTypeChanged(BAbstractFileType *)
{
    //
}

void BAbstractEditorModule::documentHasSelectionChanged(bool)
{
    //
}

void BAbstractEditorModule::documentModificationChanged(bool)
{
    //
}

void BAbstractEditorModule::documentPasteAvailableChanged(bool)
{
    //
}

void BAbstractEditorModule::documentReadOnlyChanged(bool)
{
    //
}

void BAbstractEditorModule::documentRedoAvailableChanged(bool)
{
    //
}

void BAbstractEditorModule::documentRemoved(const QString &)
{
    //
}

QList<BAbstractCodeEditorDocument *> BAbstractEditorModule::documents() const
{
    return editor() ? editor()->documents() : QList<BAbstractCodeEditorDocument *>();
}

void BAbstractEditorModule::documentSelectionChanged()
{
    //
}

void BAbstractEditorModule::documentUndoAvailableChanged(bool)
{
    //
}

void BAbstractEditorModule::editModeChanged(BCodeEdit::EditMode)
{
    //
}

void BAbstractEditorModule::editorSet(BCodeEditor *)
{
    //
}

void BAbstractEditorModule::editorUnset(BCodeEditor *)
{
    //
}

void BAbstractEditorModule::fileHistoryChanged(const QStringList &)
{
    //
}

void BAbstractEditorModule::fileTypesChanged()
{
    //
}
