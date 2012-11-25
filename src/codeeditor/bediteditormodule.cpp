#include "bediteditormodule.h"
#include "bediteditormodule_p.h"
#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditordocument.h"
#include "bcodeeditor.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BApplication>

#include <QObject>
#include <QAction>
#include <QList>
#include <QPointer>
#include <QString>

/*============================================================================
================================ Edit Editor Module Private
============================================================================*/

BEditEditorModulePrivate::BEditEditorModulePrivate(BEditEditorModule *q) :
    BAbstractEditorModulePrivate(q)
{
    //
}

BEditEditorModulePrivate::~BEditEditorModulePrivate()
{
    //
}

//

void BEditEditorModulePrivate::init()
{
    document = 0;
    //
    actCut = new QAction(this);
      actCut->setIcon( BApplication::icon("editcut") );
    actCopy = new QAction(this);
      actCopy->setIcon( BApplication::icon("editcopy") );
    actPaste = new QAction(this);
      actPaste->setIcon( BApplication::icon("editpaste") );
    actUndo = new QAction(this);
      actUndo->setIcon( BApplication::icon("undo") );
    actRedo = new QAction(this);
      actRedo->setIcon( BApplication::icon("redo") );
    //
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
    BCodeEditorDocument *doc = editor ? editor->currentDocument() : 0;
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
}

//

void BEditEditorModulePrivate::retranslateUi()
{
    if ( !actCut.isNull() )
    {
        actCut->setText( trq("Cut", "act text") );
        actCut->setToolTip( trq("Cut", "act toolTip") );
        actCut->setWhatsThis( trq("Cut", "act whatsThis") );
    }
    if ( !actCopy.isNull() )
    {
        actCopy->setText( trq("Copy", "act text") );
        actCopy->setToolTip( trq("Copy", "act toolTip") );
        actCopy->setWhatsThis( trq("Copy", "act whatsThis") );
    }
    if ( !actPaste.isNull() )
    {
        actPaste->setText( trq("Paste", "act text") );
        actPaste->setToolTip( trq("Paste", "act toolTip") );
        actPaste->setWhatsThis( trq("Paste", "act whatsThis") );
    }
    if ( !actUndo.isNull() )
    {
        actUndo->setText( trq("Undo", "act text") );
        actUndo->setToolTip( trq("Undo", "act toolTip") );
        actUndo->setWhatsThis( trq("Undo", "act whatsThis") );
    }
    if ( !actRedo.isNull() )
    {
        actRedo->setText( trq("Redo", "act text") );
        actRedo->setToolTip( trq("Redo", "act toolTip") );
        actRedo->setWhatsThis( trq("Redo", "act whatsThis") );
    }
}

/*============================================================================
================================ Edit Editor Module
============================================================================*/

BEditEditorModule::BEditEditorModule(QObject *parent) :
    BAbstractEditorModule(*new BEditEditorModulePrivate(this), parent)
{
    d_func()->init();
}

BEditEditorModule::~BEditEditorModule()
{
    //
}

//

BEditEditorModule::BEditEditorModule(BEditEditorModulePrivate &d, QObject *parent) :
    BAbstractEditorModule(d, parent)
{
    d_func()->init();
}

//

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
    return list;
}

//

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

void BEditEditorModule::currentDocumentChanged(BCodeEditorDocument *doc)
{
    d_func()->setDocument(doc);
    d_func()->checkActions();
}
