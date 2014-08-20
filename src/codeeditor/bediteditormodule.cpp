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

#include "bediteditormodule.h"
#include "bediteditormodule_p.h"

#include "babstractcodeeditordocument.h"
#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeedit.h"
#include "bcodeeditor.h"
#include "bcodeeditordocument.h"

#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BApplication>

#include <QAction>
#include <QKeySequence>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QString>

/*============================================================================
================================ BEditEditorModulePrivate ====================
============================================================================*/

/*============================== Public constructors =======================*/

BEditEditorModulePrivate::BEditEditorModulePrivate(BEditEditorModule *q) :
    BAbstractEditorModulePrivate(q)
{
    //
}

BEditEditorModulePrivate::~BEditEditorModulePrivate()
{
    //
}

/*============================== Public methods ============================*/

void BEditEditorModulePrivate::checkActions()
{
    BAbstractCodeEditorDocument *doc = q_func()->currentDocument();
    if (!actCut.isNull())
        actCut->setEnabled(doc && doc->isCutAvailable());
    if (!actCopy.isNull())
        actCopy->setEnabled(doc && doc->isCopyAvailable());
    if (!actPaste.isNull())
        actPaste->setEnabled(doc && doc->isPasteAvailable());
    if (!actUndo.isNull())
        actUndo->setEnabled(doc && doc->isUndoAvailable());
    if (!actRedo.isNull())
        actRedo->setEnabled(doc && doc->isRedoAvailable());
    checkSwitchModeAction();
}

void BEditEditorModulePrivate::checkSwitchModeAction()
{
    BCodeEditorDocument *doc = qobject_cast<BCodeEditorDocument *>(q_func()->currentDocument());
    if (!actSwitchMode.isNull())
        actSwitchMode->setEnabled(doc);
    if (doc)
        resetSwitchModeAction(doc->editMode() == BCodeEdit::BlockMode);
}

void BEditEditorModulePrivate::init()
{
    document = 0;
    //
    actCut = new QAction(this);
      actCut->setIcon(BApplication::icon("editcut"));
      actCut->setShortcut(QKeySequence::Cut);
    actCopy = new QAction(this);
      actCopy->setIcon(BApplication::icon("editcopy"));
      actCopy->setShortcut(QKeySequence::Copy);
    actPaste = new QAction(this);
      actPaste->setIcon(BApplication::icon("editpaste"));
      actPaste->setShortcut(QKeySequence::Paste);
    actUndo = new QAction(this);
      actUndo->setIcon(BApplication::icon("undo"));
      actUndo->setShortcut(QKeySequence::Undo);
    actRedo = new QAction(this);
      actRedo->setIcon(BApplication::icon("redo"));
      actRedo->setShortcut(QKeySequence::Redo);
    actSwitchMode = new QAction(this);
      connect(actSwitchMode, SIGNAL(triggered()), this, SLOT(actSwitchModeTriggered()));
    //
    resetSwitchModeAction(false);
    checkActions();
    retranslateUi();
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

void BEditEditorModulePrivate::resetSwitchModeAction(bool bm)
{
    if (actSwitchMode.isNull())
        return;
    actSwitchMode->setIcon(BApplication::icon(bm ? "edit_mode_block" : "edit_mode_normal"));
    actSwitchMode->setText(bm ? tr("Mode: blocks", "act text") : tr("Mode: lines", "act text"));
    actSwitchMode->setToolTip(bm ? tr("Edit mode: blocks", "act toolTip") : tr("Edit mode: lines", "act toolTip"));
    actSwitchMode->setWhatsThis(tr("Use this action to switch between normal and block edit modes", "act whatsThis"));
}

void BEditEditorModulePrivate::setDocument(BAbstractCodeEditorDocument *doc)
{
    if (document) {
        disconnect(actCut, SIGNAL(triggered()), document, SLOT(cut()));
        disconnect(actCopy, SIGNAL(triggered()), document, SLOT(copy()));
        disconnect(actPaste, SIGNAL(triggered()), document, SLOT(paste()));
        disconnect(actUndo, SIGNAL(triggered()), document, SLOT(undo()));
        disconnect(actRedo, SIGNAL(triggered()), document, SLOT(redo()));
    }
    document = doc;
    if (document) {
        connect(actCut, SIGNAL(triggered()), document, SLOT(cut()));
        connect(actCopy, SIGNAL(triggered()), document, SLOT(copy()));
        connect(actPaste, SIGNAL(triggered()), document, SLOT(paste()));
        connect(actUndo, SIGNAL(triggered()), document, SLOT(undo()));
        connect(actRedo, SIGNAL(triggered()), document, SLOT(redo()));
    }
}

/*============================== Public slots ==============================*/

void BEditEditorModulePrivate::retranslateUi()
{
    if (!actCut.isNull()) {
        actCut->setText(tr("Cut", "act text"));
        actCut->setToolTip(tr("Cut selected text", "act toolTip"));
        actCut->setWhatsThis(tr("Use this action to delete current selected text and move it to clipboard",
                                "act whatsThis"));
    }
    if (!actCopy.isNull()) {
        actCopy->setText(tr("Copy", "act text"));
        actCopy->setToolTip(tr("Copy selected text", "act toolTip"));
        actCopy->setWhatsThis(tr("Use this action to copy current selected text to clipboard", "act whatsThis"));
    }
    if (!actPaste.isNull()) {
        actPaste->setText(tr("Paste", "act text"));
        actPaste->setToolTip(tr("Paste text from clipboard", "act toolTip"));
        actPaste->setWhatsThis(tr("Paste text from clipboard to current cursor position", "act whatsThis"));
    }
    if (!actUndo.isNull()) {
        actUndo->setText(tr("Undo", "act text"));
        actUndo->setToolTip(tr("Undo last action", "act toolTip"));
        actUndo->setWhatsThis(tr("Use this action to undo last operation (text insertion, selection deletion, etc.)",
                                 "act whatsThis"));
    }
    if (!actRedo.isNull()) {
        actRedo->setText(tr("Redo", "act text"));
        actRedo->setToolTip(tr("Redo canceled action", "act toolTip"));
        actRedo->setWhatsThis(tr("Use this action to repeat last operation, cancelled using the Undo action",
                                 "act whatsThis"));
    }
    checkSwitchModeAction();
}

void BEditEditorModulePrivate::actSwitchModeTriggered()
{
    if (!editor)
        return;
    editor->setEditMode((editor->editMode() == BCodeEdit::NormalMode) ? BCodeEdit::BlockMode : BCodeEdit::NormalMode);
}

/*============================================================================
================================ BEditEditorModule ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BEditEditorModule::BEditEditorModule(QObject *parent) :
    BAbstractEditorModule(*new BEditEditorModulePrivate(this), parent)
{
    d_func()->init();
}

BEditEditorModule::~BEditEditorModule()
{
    //
}

/*============================== Protected constructors ====================*/

BEditEditorModule::BEditEditorModule(BEditEditorModulePrivate &d, QObject *parent) :
    BAbstractEditorModule(d, parent)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QAction *BEditEditorModule::action(int type)
{
    switch (type) {
    case CutAction:
        return d_func()->actCut;
    case CopyAction:
        return d_func()->actCopy;
    case PasteAction:
        return d_func()->actPaste;
    case UndoAction:
        return d_func()->actUndo;
    case RedoAction:
        return d_func()->actRedo;
    case SwitchModeAction:
        return d_func()->actSwitchMode;
    default:
        return 0;
    }
}

QList<QAction *> BEditEditorModule::actions(int group, bool)
{
    B_D(BEditEditorModule);
    QList<QAction *> list;
    switch (group) {
    case ClipboardActionGroup:
        if (!d->actCut.isNull())
            list << d->actCut;
        if (!d->actCopy.isNull())
            list << d->actCopy;
        if (!d->actPaste.isNull())
            list << d->actPaste;
        break;
    case UndoRedoActionGroup:
        if (!d->actUndo.isNull())
            list << d->actUndo;
        if (!d->actRedo.isNull())
            list << d->actRedo;
        break;
    default:
        break;
    }
    return list;
}

QList<QAction *> BEditEditorModule::actions(bool extended)
{
    QList<QAction *> list;
    list << actions(ClipboardActionGroup, extended);
    list << actions(UndoRedoActionGroup, extended);
    if (!d_func()->actSwitchMode.isNull())
        list << d_func()->actSwitchMode;
    return list;
}

QString BEditEditorModule::id() const
{
    return "beqt/edit";
}

/*============================== Protected methods =========================*/

void BEditEditorModule::currentDocumentChanged(BAbstractCodeEditorDocument *doc)
{
    d_func()->setDocument(doc);
    d_func()->checkActions();
}

void BEditEditorModule::documentCopyAvailableChanged(bool)
{
    d_func()->checkActions();
}

void BEditEditorModule::documentCutAvailableChanged(bool)
{
    d_func()->checkActions();
}

void BEditEditorModule::documentPasteAvailableChanged(bool)
{
    d_func()->checkActions();
}

void BEditEditorModule::documentRedoAvailableChanged(bool)
{
    d_func()->checkActions();
}

void BEditEditorModule::documentUndoAvailableChanged(bool)
{
    d_func()->checkActions();
}

void BEditEditorModule::editModeChanged(BCodeEdit::EditMode)
{
    d_func()->checkSwitchModeAction();
}

void BEditEditorModule::editorSet(BCodeEditor *edr)
{
    d_func()->setDocument(edr ? edr->currentDocument() : 0);
    d_func()->checkActions();
}

void BEditEditorModule::editorUnset(BCodeEditor *)
{
    d_func()->setDocument(0);
    d_func()->checkActions();
}
