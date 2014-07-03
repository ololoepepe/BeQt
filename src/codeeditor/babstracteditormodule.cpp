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
class QWidget;

#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditor.h"
#include "bcodeedit.h"
#include "bcodeeditor_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBaseObject>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QObject>
#include <QPoint>
#include <QList>
#include <QString>
#include <QStringList>
#include <QByteArray>

#include <QDebug>

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
    QObject(parent), BBaseObject( *new BAbstractEditorModulePrivate(this) )
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

bool BAbstractEditorModule::isBuisy() const
{
    return false;
}

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

QByteArray BAbstractEditorModule::saveState() const
{
    return QByteArray();
}

void BAbstractEditorModule::restoreState(const QByteArray &)
{
    //
}

BCodeEditor *BAbstractEditorModule::editor() const
{
    return d_func()->editor;
}

/*============================== Protected methods =========================*/

void BAbstractEditorModule::editorSet(BCodeEditor *)
{
    //
}

void BAbstractEditorModule::editorUnset(BCodeEditor *)
{
    //
}

void BAbstractEditorModule::documentReadOnlyChanged(bool)
{
    //
}

void BAbstractEditorModule::documentModificationChanged(bool)
{
    //
}

void BAbstractEditorModule::documentSelectionChanged()
{
    //
}

void BAbstractEditorModule::documentHasSelectionChanged(bool)
{
    //
}

void BAbstractEditorModule::documentCutAvailableChanged(bool)
{
    //
}

void BAbstractEditorModule::documentCopyAvailableChanged(bool)
{
    //
}

void BAbstractEditorModule::documentPasteAvailableChanged(bool)
{
    //
}

void BAbstractEditorModule::documentUndoAvailableChanged(bool)
{
    //
}

void BAbstractEditorModule::documentRedoAvailableChanged(bool)
{
    //
}

void BAbstractEditorModule::documentEditModeChanged(BCodeEdit::EditMode)
{
    //
}

void BAbstractEditorModule::documentCursorPositionChanged(const QPoint &)
{
    //
}

void BAbstractEditorModule::documentBuisyChanged(bool)
{
    //
}

void BAbstractEditorModule::documentFileNameChanged(const QString &)
{
    //
}

void BAbstractEditorModule::documentCodecChanged(const QString &)
{
    //
}

void BAbstractEditorModule::documentFileTypeChanged(BAbstractFileType *)
{
    //
}

void BAbstractEditorModule::defaultCodecChanged(const QString &)
{
    //
}

void BAbstractEditorModule::editModeChanged(BCodeEdit::EditMode)
{
    //
}

void BAbstractEditorModule::documentAboutToBeAdded(BAbstractCodeEditorDocument *)
{
    //
}

void BAbstractEditorModule::documentAdded(BAbstractCodeEditorDocument *)
{
    //
}

void BAbstractEditorModule::documentAboutToBeRemoved(BAbstractCodeEditorDocument *)
{
    //
}

void BAbstractEditorModule::documentRemoved(const QString &)
{
    //
}

void BAbstractEditorModule::currentDocumentChanged(BAbstractCodeEditorDocument *)
{
    //
}

void BAbstractEditorModule::fileTypesChanged()
{
    //
}

void BAbstractEditorModule::fileHistoryChanged(const QStringList &)
{
    //
}

BAbstractCodeEditorDocument *BAbstractEditorModule::currentDocument() const
{
    return editor() ? editor()->currentDocument() : 0;
}

QList<BAbstractCodeEditorDocument *> BAbstractEditorModule::documents() const
{
    return editor() ? editor()->documents() : QList<BAbstractCodeEditorDocument *>();
}
