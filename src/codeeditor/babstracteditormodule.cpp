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
#include <QStringList>

#include <QDebug>

/*============================================================================
================================ BAbstractEditorModulePrivate ================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractEditorModulePrivate::BAbstractEditorModulePrivate(BAbstractEditorModule *q) :
    BBasePrivate(q)
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
    QObject(parent), BBase( *new BAbstractEditorModulePrivate(this) )
{
    d_func()->init();
}

BAbstractEditorModule::~BAbstractEditorModule()
{
    //
}

/*============================== Protected constructors ====================*/

BAbstractEditorModule::BAbstractEditorModule(BAbstractEditorModulePrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

bool BAbstractEditorModule::isBuisy() const
{
    return false;
}

BCodeEditor *BAbstractEditorModule::editor() const
{
    return d_func()->editor;
}

/*============================== Protected methods =========================*/

void BAbstractEditorModule::editorSet(BCodeEditor *edr)
{
    //
}

void BAbstractEditorModule::editorUnset(BCodeEditor *edr)
{
    //
}

void BAbstractEditorModule::documentReadOnlyChanged(bool ro)
{
    //
}

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

void BAbstractEditorModule::documentBuisyChanged(bool buisy)
{
    //
}

void BAbstractEditorModule::documentFileNameChanged(const QString &fn)
{
    //
}

void BAbstractEditorModule::documentCodecChanged(const QString &codecName)
{
    //
}

void BAbstractEditorModule::documentFileTypeChanged(BAbstractFileType *ft)
{
    //
}

void BAbstractEditorModule::defaultCodecChanged(const QString &codecName)
{
    //
}

void BAbstractEditorModule::editModeChanged(BCodeEdit::EditMode mode)
{
    //
}

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

void BAbstractEditorModule::fileTypesChanged()
{
    //
}

void BAbstractEditorModule::fileHistoryChanged(const QStringList &list)
{
    //
}

BCodeEditorDocument *BAbstractEditorModule::currentDocument() const
{
    return editor() ? editor()->currentDocument() : 0;
}

QList<BCodeEditorDocument *> BAbstractEditorModule::documents() const
{
    return editor() ? editor()->documents() : QList<BCodeEditorDocument *>();
}
