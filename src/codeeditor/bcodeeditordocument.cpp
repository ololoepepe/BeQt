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

class BSplittedLinesDialog;

#include "bcodeeditordocument.h"
#include "bcodeeditordocument_p.h"

#include "bcodeedit.h"
#include "bcodeeditor.h"
#include "bplaintextedit.h"

#include <BeQtCore/BeQt>

#include <QDebug>
#include <QList>
#include <QObject>
#include <QString>
#include <QTextDocument>

/*============================================================================
================================ BCodeEditorDocumentPrivate ==================
============================================================================*/

/*============================== Public constructors =======================*/

BCodeEditorDocumentPrivate::BCodeEditorDocumentPrivate(BCodeEditorDocument *q, BCodeEditor *editor) :
    BAbstractCodeEditorDocumentPrivate(q, editor)
{
    //
}

BCodeEditorDocumentPrivate::~BCodeEditorDocumentPrivate()
{
    //
}

/*============================== Public methods ============================*/

QWidget *BCodeEditorDocumentPrivate::createEdit(QTextDocument **doc)
{
    B_Q(BCodeEditorDocument);
    cedt = new BCodeEdit;
    q->setReadOnlyInternal(cedt->isReadOnly());
    q->setModificationInternal(cedt->isModified());
    q->setHasSelection(cedt->hasSelection());
    q->setCutAvailable(cedt->isCutAvailable());
    q->setCopyAvailable(cedt->isCopyAvailable());
    q->setPasteAvailable(cedt->isPasteAvailable());
    q->setUndoAvailable(cedt->isUndoAvailable());
    q->setRedoAvailable(cedt->isRedoAvailable());
    q->setCursorPosition(cedt->cursorPosition());
    q->setBuisy(cedt->isBuisy());
    connect(cedt, SIGNAL(selectionChanged()), q, SLOT(emitSelectionChanged()));
    connect(cedt, SIGNAL(textChanged()), q, SLOT(emitTextChanged()));
    connect(cedt, SIGNAL(readOnlyChanged(bool)), q, SLOT(setReadOnlyInternal(bool)));
    connect(cedt, SIGNAL(modificationChanged(bool)), q, SLOT(setModificationInternal(bool)));
    connect(cedt, SIGNAL(hasSelectionChanged(bool)), q, SLOT(setHasSelection(bool)));
    connect(cedt, SIGNAL(cutAvailableChanged(bool)), q, SLOT(setCutAvailable(bool)));
    connect(cedt, SIGNAL(copyAvailableChanged(bool)), q, SLOT(setCopyAvailable(bool)));
    connect(cedt, SIGNAL(pasteAvailableChanged(bool)), q, SLOT(setPasteAvailable(bool)));
    connect(cedt, SIGNAL(undoAvailableChanged(bool)), q, SLOT(setUndoAvailable(bool)));
    connect(cedt, SIGNAL(redoAvailableChanged(bool)), q, SLOT(setRedoAvailable(bool)));
    connect(cedt, SIGNAL(cursorPositionChanged(QPoint)), q, SLOT(setCursorPosition(QPoint)));
    connect(cedt, SIGNAL(buisyChanged(bool)), q, SLOT(setBuisy(bool)));
    connect(cedt, SIGNAL(editModeChanged(BCodeEdit::EditMode)), q, SIGNAL(editModeChanged(BCodeEdit::EditMode)));
    connect(cedt, SIGNAL(lineSplitted(BCodeEdit::SplittedLinesRange)),
            q, SIGNAL(lineSplitted(BCodeEdit::SplittedLinesRange)));
    connect(cedt, SIGNAL(linesSplitted(QList<BCodeEdit::SplittedLinesRange>)),
            q, SIGNAL(linesSplitted(QList<BCodeEdit::SplittedLinesRange>)));
    if (doc)
        *doc = cedt->innerDocument();
    return cedt;
}

void BCodeEditorDocumentPrivate::init()
{
    cedt = 0;
    sld = 0;
}

/*============================================================================
================================ BCodeEditorDocument =========================
============================================================================*/

/*============================== Public constructors =======================*/

BCodeEditorDocument::BCodeEditorDocument(BCodeEditor *editor, QWidget *parent) :
    BAbstractCodeEditorDocument(*new BCodeEditorDocumentPrivate(this, editor), parent)
{
    d_func()->init();
}

BCodeEditorDocument::~BCodeEditorDocument()
{
    //
}

/*============================== Protected constructors ====================*/

BCodeEditorDocument::BCodeEditorDocument(BCodeEditorDocumentPrivate &d, QWidget *parent) :
    BAbstractCodeEditorDocument(d, parent)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

int BCodeEditorDocument::cursorPositionForRowColumn(const QPoint &pos) const
{
    return d_func()->cedt->cursorPositionForRowColumn(pos);
}

QFont BCodeEditorDocument::editFont() const
{
    return d_func()->cedt->editFont();
}

int BCodeEditorDocument::editLineLength() const
{
    return d_func()->cedt->editLineLength();
}

BCodeEdit::EditMode BCodeEditorDocument::editMode() const
{
    return d_func()->cedt->editMode();
}

BeQt::TabWidth BCodeEditorDocument::editTabWidth() const
{
    return d_func()->cedt->editTabWidth();
}

void BCodeEditorDocument::installInnerEventFilter(QObject *filter)
{
    d_func()->cedt->innerEdit()->installEventFilter(filter);
}

void BCodeEditorDocument::installInnerViewportEventFilter(QObject *filter)
{
    d_func()->cedt->innerEdit()->viewport()->installEventFilter(filter);
}

bool BCodeEditorDocument::isEditAutoIndentationEnabled() const
{
    return d_func()->cedt->isEditAutoIndentationEnabled();
}

bool BCodeEditorDocument::isLineNumberWidgetVisible() const
{
    return d_func()->cedt->isLineNumberWidgetVisible();
}

void BCodeEditorDocument::removeInnerEventFilter(QObject *filter)
{
    d_func()->cedt->innerEdit()->removeEventFilter(filter);
}

int BCodeEditorDocument::replaceInDocument(const QString &txt, const QString &newText, QTextDocument::FindFlags flags)
{
    return d_func()->cedt->replaceInDocument(txt, newText, flags);
}

int BCodeEditorDocument::replaceInDocumentRegexp(const QRegExp &rx, const QString &newText)
{
    return d_func()->cedt->replaceInDocumentRegexp(rx, newText);
}

int BCodeEditorDocument::replaceInSelection(const QString &txt, const QString &newText, QTextDocument::FindFlags flags)
{
    return d_func()->cedt->replaceInSelection(txt, newText, flags);
}

int BCodeEditorDocument::replaceInSelectionRegexp(const QRegExp &rx, const QString &newText)
{
    return d_func()->cedt->replaceInSelectionRegexp(rx, newText);
}

bool BCodeEditorDocument::replaceNext(const QString &newText)
{
    return d_func()->cedt->replaceNext(newText);
}

QString BCodeEditorDocument::selectedText(bool full) const
{
    return d_func()->cedt->selectedText(full);
}

int BCodeEditorDocument::selectionEnd() const
{
    return d_func()->cedt->selectionEnd();
}

QPoint BCodeEditorDocument::selectionEndRowColumn() const
{
    return d_func()->cedt->selectionEndRowColumn();
}

int BCodeEditorDocument::selectionStart() const
{
    return d_func()->cedt->selectionStart();
}

QPoint BCodeEditorDocument::selectionStartRowColumn() const
{
    return d_func()->cedt->selectionStartRowColumn();
}

void BCodeEditorDocument::setEditAutoIndentationEnabled(bool enabled)
{
    d_func()->cedt->setEditAutoIndentationEnabled(enabled);
}

void BCodeEditorDocument::setEditFont(const QFont &fnt)
{
    d_func()->cedt->setEditFont(fnt);
}

void BCodeEditorDocument::setEditLineLength(int ll)
{
    d_func()->cedt->setEditLineLength(ll);
}

void BCodeEditorDocument::setEditMode(BCodeEdit::EditMode mode)
{
    d_func()->cedt->setEditMode(mode);
}

void BCodeEditorDocument::setEditTabWidth(BeQt::TabWidth tw)
{
    d_func()->cedt->setEditTabWidth(tw);
}

void BCodeEditorDocument::setLineNumberWidgetVisible(bool b)
{
    d_func()->cedt->setLineNumberWidgetVisible(b);
}

void BCodeEditorDocument::setModification(bool modified)
{
    d_func()->cedt->setModification(modified);
}

void BCodeEditorDocument::setReadOnly(bool ro)
{
    d_func()->cedt->setReadOnly(ro);
}

void BCodeEditorDocument::setSplittedLinesDialog(BSplittedLinesDialog *dlg)
{
    d_func()->sld = dlg;
}

BSplittedLinesDialog *BCodeEditorDocument::splittedLinesDialog() const
{
    return d_func()->sld;
}

QString BCodeEditorDocument::text(bool full) const
{
    return d_func()->cedt->text(full);
}

/*============================== Public slots ==============================*/

void BCodeEditorDocument::switchMode()
{
    d_func()->cedt->switchMode();
}

/*============================== Protected methods =========================*/

void BCodeEditorDocument::activateWindowImplementation()
{
    d_func()->cedt->activateWindow();
}

void BCodeEditorDocument::clearImplementation()
{
    d_func()->cedt->clear();
}

void BCodeEditorDocument::copyImplementation()
{
    d_func()->cedt->copy();
}

QWidget *BCodeEditorDocument::createEdit(QTextDocument **doc)
{
    return d_func()->createEdit(doc);
}

QPoint BCodeEditorDocument::cursorPositionRowColumnImplementation() const
{
    return d_func()->cedt->cursorPositionRowColumn();
}

void BCodeEditorDocument::cutImplementation()
{
    d_func()->cedt->cut();
}

void BCodeEditorDocument::deleteSelectionImplementation()
{
    d_func()->cedt->deleteSelection();
}

void BCodeEditorDocument::deselectTextImplementation()
{
    d_func()->cedt->deselectText();
}

BCodeEditorDocument::ExtraSelectionList BCodeEditorDocument::extraSelectionsImplementation() const
{
    return d_func()->cedt->extraSelections();
}

bool BCodeEditorDocument::findNextImplementation(const QString &txt, QTextDocument::FindFlags flags, bool cyclic)
{
    return d_func()->cedt->findNext(txt, flags, cyclic);
}

bool BCodeEditorDocument::findNextRegexpImplementation(const QRegExp &rx, QTextDocument::FindFlags flags, bool cyclic)
{
    return d_func()->cedt->findNextRegexp(rx, flags, cyclic);
}

void BCodeEditorDocument::insertTextImplementation(const QString &txt)
{
    d_func()->cedt->insertText(txt);
}

void BCodeEditorDocument::moveCursorImplementation(int pos)
{
    d_func()->cedt->moveCursor(pos);
}

void BCodeEditorDocument::moveCursorImplementation(const QPoint &pos)
{
    d_func()->cedt->moveCursor(pos);
}

void BCodeEditorDocument::pasteImplementation()
{
    d_func()->cedt->paste();
}

void BCodeEditorDocument::redoImplementation()
{
    d_func()->cedt->redo();
}

void BCodeEditorDocument::selectAllImplementation()
{
    d_func()->cedt->selectAll();
}

void BCodeEditorDocument::selectLinesImplementation(int firstLine, int lastLine)
{
    d_func()->cedt->selectLines(firstLine, lastLine);
}

void BCodeEditorDocument::selectTextImplementation(const QPoint &start, const QPoint &end)
{
    d_func()->cedt->selectText(start, end);
}

void BCodeEditorDocument::selectTextImplementation(int start, int end)
{
    d_func()->cedt->selectText(start, end);
}

void BCodeEditorDocument::setExtraSelectionsImplementation(const ExtraSelectionList &list)
{
    d_func()->cedt->setExtraSelections(list);
}

void BCodeEditorDocument::setFocusImplementation()
{
    d_func()->cedt->setFocus();
}

void BCodeEditorDocument::setTextImplementation(const QString &txt)
{
    d_func()->cedt->setText(txt, asyncProcessingMinimumLength());
}

void BCodeEditorDocument::undoImplementation()
{
    d_func()->cedt->undo();
}
