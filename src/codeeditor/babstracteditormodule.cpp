#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditor.h"
#include "bcodeedit.h"
#include "bcodeeditor_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QPoint>
#include <QList>
#include <QString>

#include <QDebug>

/*============================================================================
================================ Abstract Editor Module Private
============================================================================*/

BAbstractEditorModulePrivate::BAbstractEditorModulePrivate(BAbstractEditorModule *q) :
    BBasePrivate(q)
{
    //
}

BAbstractEditorModulePrivate::~BAbstractEditorModulePrivate()
{
    //
}

//

void BAbstractEditorModulePrivate::init()
{
    editor = 0;
}

/*============================================================================
================================ Abstract Editor Module
============================================================================*/

BAbstractEditorModule::BAbstractEditorModule(QObject *parent) :
    QObject(parent), BBase( *new BAbstractEditorModulePrivate(this) )
{
    d_func()->init();
}

BAbstractEditorModule::~BAbstractEditorModule()
{
    //
}

//

bool BAbstractEditorModule::isBuisy() const
{
    return false;
}

BCodeEditor *BAbstractEditorModule::editor() const
{
    return d_func()->editor;
}

//

BAbstractEditorModule::BAbstractEditorModule(BAbstractEditorModulePrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

//

void BAbstractEditorModule::editorSet(BCodeEditor *edr)
{
    //
}

void BAbstractEditorModule::editorUnset(BCodeEditor *edr)
{
    //
}

//BCodeEdit events

void BAbstractEditorModule::documentModificationChanged(bool modified)
{
    //
}

void BAbstractEditorModule::documentSelectionChanged()
{
    //
}

void BAbstractEditorModule::documentHasSelectionChanged(bool hasSelection)
{
    //
}

void BAbstractEditorModule::documentCutAvailableChanged(bool available)
{
    //
}

void BAbstractEditorModule::documentCopyAvailableChanged(bool available)
{
    //
}

void BAbstractEditorModule::documentPasteAvailableChanged(bool available)
{
    //
}

void BAbstractEditorModule::documentUndoAvailableChanged(bool available)
{
    //
}

void BAbstractEditorModule::documentRedoAvailableChanged(bool available)
{
    //
}

void BAbstractEditorModule::documentEditModeChanged(BCodeEdit::EditMode mode)
{
    //
}

void BAbstractEditorModule::documentCursorPositionChanged(const QPoint &pos)
{
    //
}

void BAbstractEditorModule::documentLineSplitted(const BCodeEdit::SplittedLinesRange &linesRange)
{
    //
}

void BAbstractEditorModule::documentLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges)
{
    //
}

//BCodeEditorDocument events

void BAbstractEditorModule::documentFileNameChanged(const QString &fn)
{
    //
}

void BAbstractEditorModule::documentCodecChanged(const QString &codecName)
{
    //
}

void BAbstractEditorModule::documentBuisyChanged(bool buisy)
{
    //
}

//BCodeEditor events

void BAbstractEditorModule::documentAboutToBeAdded(BCodeEditorDocument *doc)
{
    //
}

void BAbstractEditorModule::documentAdded(BCodeEditorDocument *doc)
{
    //
}

void BAbstractEditorModule::documentAboutToBeRemoved(BCodeEditorDocument *doc)
{
    //
}

void BAbstractEditorModule::currentDocumentChanged(BCodeEditorDocument *doc)
{
    //
}

//

BCodeEditorDocument *BAbstractEditorModule::currentDocument() const
{
    return editor() ? editor()->currentDocument() : 0;
}

QList<BCodeEditorDocument *> BAbstractEditorModule::documents() const
{
    return editor() ? editor()->documents() : QList<BCodeEditorDocument *>();
}
