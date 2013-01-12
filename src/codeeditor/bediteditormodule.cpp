#include "bediteditormodule.h"
#include "bediteditormodule_p.h"
#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditordocument.h"
#include "bcodeeditor.h"
#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BApplication>

#include <QObject>
#include <QAction>
#include <QList>
#include <QPointer>
#include <QString>
#include <QKeySequence>

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

void BEditEditorModulePrivate::init()
{
    document = 0;
    //
    actCut = new QAction(this);
      actCut->setIcon( BApplication::icon("editcut") );
      actCut->setShortcut(QKeySequence::Cut);
    actCopy = new QAction(this);
      actCopy->setIcon( BApplication::icon("editcopy") );
      actCopy->setShortcut(QKeySequence::Copy);
    actPaste = new QAction(this);
      actPaste->setIcon( BApplication::icon("editpaste") );
      actPaste->setShortcut(QKeySequence::Paste);
    actUndo = new QAction(this);
      actUndo->setIcon( BApplication::icon("undo") );
      actUndo->setShortcut(QKeySequence::Undo);
    actRedo = new QAction(this);
      actRedo->setIcon( BApplication::icon("redo") );
      actRedo->setShortcut(QKeySequence::Redo);
    actSwitchMode = new QAction(this);
      connect( actSwitchMode.data(), SIGNAL( triggered() ), this, SLOT( actSwitchModeTriggered() ) );
    //
    resetSwitchModeAction(false);
    checkActions();
    retranslateUi();
    connect( bApp, SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

void BEditEditorModulePrivate::setDocument(BCodeEditorDocument *doc)
{
    if (document)
    {
        disconnect( actCut.data(), SIGNAL( triggered() ), document, SLOT( cut() ) );
        disconnect( actCopy.data(), SIGNAL( triggered() ), document, SLOT( copy() ) );
        disconnect( actPaste.data(), SIGNAL( triggered() ), document, SLOT( paste() ) );
        disconnect( actUndo.data(), SIGNAL( triggered() ), document, SLOT( undo() ) );
        disconnect( actRedo.data(), SIGNAL( triggered() ), document, SLOT( redo() ) );
    }
    document = doc;
    if (document)
    {
        connect( actCut.data(), SIGNAL( triggered() ), document, SLOT( cut() ) );
        connect( actCopy.data(), SIGNAL( triggered() ), document, SLOT( copy() ) );
        connect( actPaste.data(), SIGNAL( triggered() ), document, SLOT( paste() ) );
        connect( actUndo.data(), SIGNAL( triggered() ), document, SLOT( undo() ) );
        connect( actRedo.data(), SIGNAL( triggered() ), document, SLOT( redo() ) );
    }
}

void BEditEditorModulePrivate::checkActions()
{
    BCodeEditorDocument *doc = q_func()->currentDocument();
    if ( !actCut.isNull() )
        actCut->setEnabled( doc && doc->isCutAvailable() );
    if ( !actCopy.isNull() )
        actCopy->setEnabled( doc && doc->isCopyAvailable() );
    if ( !actPaste.isNull() )
        actPaste->setEnabled( doc && doc->isPasteAvailable() );
    if ( !actUndo.isNull() )
        actUndo->setEnabled( doc && doc->isUndoAvailable() );
    if ( !actRedo.isNull() )
        actRedo->setEnabled( doc && doc->isRedoAvailable() );
    checkSwitchModeAction();
}

void BEditEditorModulePrivate::checkSwitchModeAction()
{
    if ( actSwitchMode.isNull() )
        return;
    BCodeEditorDocument *doc = q_func()->currentDocument();
    actSwitchMode->setEnabled(doc);
    if (doc)
        resetSwitchModeAction(doc->editMode() == BCodeEdit::BlockMode);
}

void BEditEditorModulePrivate::resetSwitchModeAction(bool bm)
{
    if ( actSwitchMode.isNull() )
        return;
    actSwitchMode->setIcon( BApplication::icon(bm ? "edit_mode_block" : "edit_mode_normal") );
    actSwitchMode->setText( bm ? tr("Mode: blocks", "act text") : tr("Mode: lines", "act text") );
    actSwitchMode->setToolTip( bm ? tr("Edit mode: blocks", "act toolTip") : tr("Edit mode: lines", "act toolTip") );
    actSwitchMode->setWhatsThis( tr("Use this action to switch between normal and block edit modes",
                                    "act whatsThis") );
}

/*============================== Public slots ==============================*/

void BEditEditorModulePrivate::retranslateUi()
{
    if ( !actCut.isNull() )
    {
        actCut->setText( tr("Cut", "act text") );
        actCut->setToolTip( tr("Cut selected text", "act toolTip") );
        actCut->setWhatsThis( tr("Use this action to delete current selected text and move it to clipboard",
                                 "act whatsThis") );
    }
    if ( !actCopy.isNull() )
    {
        actCopy->setText( tr("Copy", "act text") );
        actCopy->setToolTip( tr("Copy selected text", "act toolTip") );
        actCopy->setWhatsThis( tr("Use this action to copy current selected text to clipboard", "act whatsThis") );
    }
    if ( !actPaste.isNull() )
    {
        actPaste->setText( tr("Paste", "act text") );
        actPaste->setToolTip( tr("Paste text from clipboard", "act toolTip") );
        actPaste->setWhatsThis( tr("Paste text from clipboard to current cursor position", "act whatsThis") );
    }
    if ( !actUndo.isNull() )
    {
        actUndo->setText( tr("Undo", "act text") );
        actUndo->setToolTip( tr("Undo last action", "act toolTip") );
        actUndo->setWhatsThis( tr("Use this action to undo last action (text insertion, selection deletion, etc.)",
                                  "act whatsThis") );
    }
    if ( !actRedo.isNull() )
    {
        actRedo->setText( tr("Redo", "act text") );
        actRedo->setToolTip( tr("Redo canceled action", "act toolTip") );
        actRedo->setWhatsThis( tr("Use this action to repeat last action, cancelled using the Undo action",
                                  "act whatsThis") );
    }
    checkSwitchModeAction();
}

void BEditEditorModulePrivate::actSwitchModeTriggered()
{
    if (!editor)
        return;
    editor->setEditMode( (editor->editMode() == BCodeEdit::NormalMode) ? BCodeEdit::BlockMode :
                                                                         BCodeEdit::NormalMode );
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

QString BEditEditorModule::id() const
{
    return "beqt/edit";
}

QAction *BEditEditorModule::action(Action type) const
{
    switch (type)
    {
    case CutAction:
        return d_func()->actCut.data();
    case CopyAction:
        return d_func()->actCopy.data();
    case PasteAction:
        return d_func()->actPaste.data();
    case UndoAction:
        return d_func()->actUndo.data();
    case RedoAction:
        return d_func()->actRedo.data();
    case SwitchModeAction:
        return d_func()->actSwitchMode.data();
    default:
        return 0;
    }
}

QList<QAction *> BEditEditorModule::clipboardActions() const
{
    const B_D(BEditEditorModule);
    QList<QAction *> list;
    if ( !d->actCut.isNull() )
        list << d->actCut.data();
    if ( !d->actCopy.isNull() )
        list << d->actCopy.data();
    if ( !d->actPaste.isNull() )
        list << d->actPaste.data();
    return list;
}

QList<QAction *> BEditEditorModule::undoRedoActions() const
{
    const B_D(BEditEditorModule);
    QList<QAction *> list;
    if ( !d->actUndo.isNull() )
        list << d->actUndo.data();
    if ( !d->actRedo.isNull() )
        list << d->actRedo.data();
    return list;
}

QList<QAction *> BEditEditorModule::actions() const
{
    QList<QAction *> list;
    list << clipboardActions();
    list << undoRedoActions();
    list << d_func()->actSwitchMode.data();
    return list;
}

/*============================== Protected methods =========================*/

void BEditEditorModule::editorSet(BCodeEditor *edr)
{
    d_func()->setDocument(edr ? edr->currentDocument() : 0);
    d_func()->checkActions();
}

void BEditEditorModule::editorUnset(BCodeEditor *edr)
{
    d_func()->setDocument(0);
    d_func()->checkActions();
}

void BEditEditorModule::documentCutAvailableChanged(bool available)
{
    d_func()->checkActions();
}

void BEditEditorModule::documentCopyAvailableChanged(bool available)
{
    d_func()->checkActions();
}

void BEditEditorModule::documentPasteAvailableChanged(bool available)
{
    d_func()->checkActions();
}

void BEditEditorModule::documentUndoAvailableChanged(bool available)
{
    d_func()->checkActions();
}

void BEditEditorModule::documentRedoAvailableChanged(bool available)
{
    d_func()->checkActions();
}

void BEditEditorModule::editModeChanged(BCodeEdit::EditMode mode)
{
    d_func()->checkSwitchModeAction();
}

void BEditEditorModule::currentDocumentChanged(BCodeEditorDocument *doc)
{
    d_func()->setDocument(doc);
    d_func()->checkActions();
}
