class QAction;
class QWidget;

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
#include <QByteArray>

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
