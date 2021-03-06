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

#include "bsimplecodeeditordocument.h"
#include "bsimplecodeeditordocument_p.h"

#include "bcodeeditor.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BTextTools>
#include <BeQtWidgets/BClipboardNotifier>
#include <BeQtWidgets/BLineNumberWidget>

#include <QApplication>
#include <QChar>
#include <QDebug>
#include <QList>
#include <QObject>
#include <QPlainTextEdit>
#include <QPoint>
#include <QString>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ BSimpleCodeEditorDocumentPrivate ============
============================================================================*/

/*============================== Public constructors =======================*/

BSimpleCodeEditorDocumentPrivate::BSimpleCodeEditorDocumentPrivate(BSimpleCodeEditorDocument *q, BCodeEditor *editor) :
    BAbstractCodeEditorDocumentPrivate(q, editor)
{
    //
}

BSimpleCodeEditorDocumentPrivate::~BSimpleCodeEditorDocumentPrivate()
{
    //
}

/*============================== Static public methods =====================*/

BAbstractCodeEditorDocument::TextProcessingResult BSimpleCodeEditorDocumentPrivate::procFunc(const QString &text,
                                                                                             const QVariant &v)
{
    static const QList<BeQt::TabWidth> WidthList = QList<BeQt::TabWidth>() << BeQt::TabWidth2 << BeQt::TabWidth4
                                                                           << BeQt::TabWidth8;
    BAbstractCodeEditorDocument::TextProcessingResult r;
    r.text = BTextTools::withoutUnsuppottedSymbols(text);
    BeQt::TabWidth tw = enum_cast<BeQt::TabWidth>(v, WidthList, BeQt::TabWidth4);
    BTextTools::replaceTabs(&r.text, static_cast<BeQt::TabWidth>(tw));
    return r;
}

/*============================== Public methods ============================*/

QWidget *BSimpleCodeEditorDocumentPrivate::createEdit(QTextDocument **doc)
{
    connect(BClipboardNotifier::instance(), SIGNAL(hasTextChanged(bool)), this, SLOT(updatePasteAvailable(bool)));
    B_Q(BSimpleCodeEditorDocument);
    tabWidth = BeQt::TabWidth4;
    ptedt = new QPlainTextEdit;
      ptedt->setLineWrapMode(QPlainTextEdit::NoWrap);
      ptedt->setWordWrapMode(QTextOption::NoWrap);
      ptedt->installEventFilter(this);
    lnwgt = new BLineNumberWidget(ptedt);
    bool hasSel = ptedt->textCursor().hasSelection();
    q->setReadOnlyInternal(ptedt->isReadOnly());
    q->setModificationInternal(ptedt->document()->isModified());
    q->setHasSelection(hasSel);
    q->setCutAvailable(hasSel && !ptedt->isReadOnly());
    q->setCopyAvailable(hasSel);
    q->setPasteAvailable(!ptedt->isReadOnly() && BClipboardNotifier::hasText());
    q->setUndoAvailable(ptedt->document()->isUndoAvailable());
    q->setRedoAvailable(ptedt->document()->isRedoAvailable());
    q->setCursorPosition(QPoint());
    q->setBuisy(false);
    connect(ptedt, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
    connect(ptedt, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(ptedt, SIGNAL(selectionChanged()), q, SLOT(emitSelectionChanged()));
    connect(ptedt, SIGNAL(textChanged()), q, SLOT(emitTextChanged()));
    connect(ptedt, SIGNAL(modificationChanged(bool)), q, SLOT(setModificationInternal(bool)));
    connect(ptedt, SIGNAL(undoAvailable(bool)), q, SLOT(setUndoAvailable(bool)));
    connect(ptedt, SIGNAL(redoAvailable(bool)), q, SLOT(setRedoAvailable(bool)));
    if (doc)
        *doc = ptedt->document();
    return ptedt;
}

bool BSimpleCodeEditorDocumentPrivate::eventFilter(QObject *obj, QEvent *e)
{
    if (blockFilter)
        return QObject::eventFilter(obj, e);
    switch (e->type()) {
    case QEvent::KeyPress:
        return keyPressEvent(static_cast<QKeyEvent *>(e));
    default:
        return QObject::eventFilter(obj, e);
    }
}

void BSimpleCodeEditorDocumentPrivate::handleReturn(QKeyEvent *e)
{
    if (ptedt->isReadOnly())
        return;
    QTextCursor tc = ptedt->textCursor();
    int indent = (autoIndentation && q_func()->fileType()) ? q_func()->fileType()->indentation(tc.block()) : 0;
    blockFilter = true;
    QApplication::sendEvent(ptedt, e);
    blockFilter = false;
    if (indent <= 0)
        return;
    tc = ptedt->textCursor();
    tc.beginEditBlock();
    tc.insertText(QString().fill(' ', indent));
    tc.endEditBlock();
}

void BSimpleCodeEditorDocumentPrivate::handleTab()
{
    if (ptedt->isReadOnly())
        return;
    QTextCursor tc = ptedt->textCursor();
    tc = ptedt->textCursor();
    int posb = tc.positionInBlock();
    int spcount = tabWidth - (posb < tabWidth ? posb : posb % tabWidth);
    if (!spcount)
        spcount = tabWidth;
    q_func()->insertText(QString().fill(' ', spcount));
}

void BSimpleCodeEditorDocumentPrivate::init()
{
    ptedt = 0;
    autoIndentation = true;
    blockFilter = false;
}

bool BSimpleCodeEditorDocumentPrivate::keyPressEvent(QKeyEvent *e)
{
    int modifiers = e->modifiers();
    int key = e->key();
    switch (modifiers) {
    case Qt::NoModifier:
        switch (key) {
        case Qt::Key_Tab:
            handleTab();
            return true;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            handleReturn(e);
            return true;
        }
        break;
    case Qt::ControlModifier:
    case Qt::ShiftModifier:
    case Qt::AltModifier:
    case BeQt::ContorlShiftModifier:
    case BeQt::KeypadShiftModifier:
    case BeQt::KeypadAltModifier:
    case BeQt::KeypadControlShiftModifier:
    case BeQt::KeypadControlAltModifier:
    case Qt::KeypadModifier:
    case BeQt::KeypadControlModifier:
        switch (key) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            handleReturn(e);
            return true;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return false;
}

/*============================== Public slots ==============================*/

void BSimpleCodeEditorDocumentPrivate::cursorPositionChanged()
{
    QTextCursor tc = ptedt->textCursor();
    q_func()->setCursorPosition(tc.position());
}

void BSimpleCodeEditorDocumentPrivate::selectionChanged()
{
    bool hasSel = ptedt->textCursor().hasSelection();
    q_func()->setHasSelection(hasSel);
    q_func()->setCutAvailable(hasSel && !ptedt->isReadOnly());
    q_func()->setCopyAvailable(hasSel);
}

void BSimpleCodeEditorDocumentPrivate::updatePasteAvailable(bool available)
{
    q_func()->setPasteAvailable(available && !ptedt->isReadOnly());
}

/*============================================================================
================================ BSimpleCodeEditorDocument ===================
============================================================================*/

/*============================== Public constructors =======================*/

BSimpleCodeEditorDocument::BSimpleCodeEditorDocument(BCodeEditor *editor, QWidget *parent) :
    BAbstractCodeEditorDocument(*new BSimpleCodeEditorDocumentPrivate(this, editor), parent)
{
    d_func()->init();
}

BSimpleCodeEditorDocument::~BSimpleCodeEditorDocument()
{
    //
}

/*============================== Protected constructors ====================*/

BSimpleCodeEditorDocument::BSimpleCodeEditorDocument(BSimpleCodeEditorDocumentPrivate &d, QWidget *parent) :
    BAbstractCodeEditorDocument(d, parent)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

int BSimpleCodeEditorDocument::cursorPositionForRowColumn(const QPoint &pos) const
{
    if (pos.x() < 0 || pos.y() < 0)
        return -1;
    const B_D(BSimpleCodeEditorDocument);
    QTextBlock tb = d->ptedt->document()->findBlockByLineNumber(pos.y());
    if (!tb.isValid())
        return -1;
    if (pos.x() >= tb.length())
        return -1;
    return tb.position() + pos.x();
}

QFont BSimpleCodeEditorDocument::editFont() const
{
    return d_func()->ptedt->font();
}

BeQt::TabWidth BSimpleCodeEditorDocument::editTabWidth() const
{
    return d_func()->tabWidth;
}

void BSimpleCodeEditorDocument::installInnerEventFilter(QObject *filter)
{
    d_func()->ptedt->installEventFilter(filter);
}

void BSimpleCodeEditorDocument::installInnerViewportEventFilter(QObject *filter)
{
    d_func()->ptedt->viewport()->installEventFilter(filter);
}

bool BSimpleCodeEditorDocument::isEditAutoIndentationEnabled() const
{
    return d_func()->autoIndentation;
}

bool BSimpleCodeEditorDocument::isLineNumberWidgetVisible() const
{
    return d_func()->lnwgt->isVisible();
}

void BSimpleCodeEditorDocument::removeInnerEventFilter(QObject *filter)
{
    d_func()->ptedt->removeEventFilter(filter);
}

int BSimpleCodeEditorDocument::replaceInDocument(const QString &txt, const QString &newText,
                                                 QTextDocument::FindFlags flags)
{
    if (isReadOnly() || txt.isEmpty() || txt == newText)
        return -1;
    Qt::CaseSensitivity cs = (QTextDocument::FindCaseSensitively & flags) ? Qt::CaseSensitive : Qt::CaseInsensitive;
    bool w = (QTextDocument::FindWholeWords & flags);
    QString ptext = text(true);
    int count = 0;
    int ind = BTextTools::indexOf(ptext, txt, 0, cs, w);
    while (ind >= 0) {
        ++count;
        ind = BTextTools::indexOf(ptext, txt, ind + txt.length(), cs, w);
    }
    if (!count)
        return 0;
    int find = BTextTools::indexOf(ptext, txt, 0, cs, w);
    int lind = BTextTools::lastIndexOf(ptext, txt, -1, cs, w) + txt.length();
    selectText(find, lind);
    QString ntext = ptext.mid(find, lind - find);
    BTextTools::replace(ntext, txt, newText, cs, w);
    insertText(ntext);
    return count;
}

int BSimpleCodeEditorDocument::replaceInDocumentRegexp(const QRegExp &rx, const QString &newText)
{
    if (isReadOnly() || !rx.isValid() || rx.isEmpty())
        return -1;
    QString ptext = text(true);
    int count = 0;
    int ind = rx.indexIn(ptext);
    while (ind >= 0) {
        ++count;
        ind = rx.indexIn(ptext, ind + rx.matchedLength());
    }
    if (!count)
        return 0;
    int find = rx.indexIn(ptext);
    int lind = rx.lastIndexIn(ptext) + rx.matchedLength();
    selectText(find, lind);
    QString ntext = ptext.mid(find, lind - find);
    ntext.replace(rx, newText);
    insertText(ntext);
    return count;
}

int BSimpleCodeEditorDocument::replaceInSelection(const QString &txt, const QString &newText,
                                                  QTextDocument::FindFlags flags)
{
    if (isReadOnly() || !hasSelection() || txt.isEmpty() || txt == newText)
        return -1;
    B_D(BSimpleCodeEditorDocument);
    QString ntext = selectedText(true);
    Qt::CaseSensitivity cs = (QTextDocument::FindCaseSensitively & flags) ? Qt::CaseSensitive : Qt::CaseInsensitive;
    bool w = (QTextDocument::FindWholeWords & flags);
    int count = 0;
    int ind = BTextTools::indexOf(ntext, txt, 0, cs, w);
    while (ind >= 0) {
        ++count;
        ind = BTextTools::indexOf(ntext, txt, ind + txt.length(), cs, w);
    }
    if (!count)
        return 0;
    QTextCursor tc = d->ptedt->textCursor();
    tc.beginEditBlock();
    int start = tc.selectionStart();
    int end = tc.selectionEnd();
    int len = ntext.length();
    BTextTools::replace(ntext, txt, newText, cs, w);
    tc.insertText(ntext);
    if (start < end)
        end += (ntext.length() - len);
    else
        start += (ntext.length() - len);
    selectText(start, end);
    tc.endEditBlock();
    QTextBlock tb = d->ptedt->document()->findBlock(qMin<int>(start, end));
    QTextBlock tbe = d->ptedt->document()->findBlock(qMax<int>(start, end));
    while (tb.isValid() && tb.blockNumber() < tbe.blockNumber()) {
        rehighlightBlock(tb);
        tb = tb.next();
    }
    return count;
}

int BSimpleCodeEditorDocument::replaceInSelectionRegexp(const QRegExp &rx, const QString &newText)
{
    if (isReadOnly() || !hasSelection() || !rx.isValid() || rx.isEmpty())
        return -1;
    B_D(BSimpleCodeEditorDocument);
    QString ntext = selectedText(true);
    int count = 0;
    int ind = rx.indexIn(ntext);
    while (ind >= 0) {
        ++count;
        ind = rx.indexIn(ntext, ind + rx.matchedLength());
    }
    if (!count)
        return 0;
    QTextCursor tc = d->ptedt->textCursor();
    tc.beginEditBlock();
    int start = tc.selectionStart();
    int end = tc.selectionEnd();
    int len = ntext.length();
    ntext.replace(rx, newText);
    tc.insertText(ntext);
    if (start < end)
        end += (ntext.length() - len);
    else
        start += (ntext.length() - len);
    selectText(start, end);
    tc.endEditBlock();
    QTextBlock tb = d->ptedt->document()->findBlock(qMin<int>(start, end));
    QTextBlock tbe = d->ptedt->document()->findBlock(qMax<int>(start, end));
    while (tb.isValid() && tb.blockNumber() < tbe.blockNumber()) {
        rehighlightBlock(tb);
        tb = tb.next();
    }
    return count;
}

bool BSimpleCodeEditorDocument::replaceNext(const QString &newText)
{
    if (isReadOnly() || !hasSelection())
        return false;
    insertText(newText);
    return true;
}

QString BSimpleCodeEditorDocument::selectedText(bool) const
{
    return d_func()->ptedt->textCursor().selectedText().replace(QChar::ParagraphSeparator, '\n');
}

int BSimpleCodeEditorDocument::selectionEnd() const
{
    if (!hasSelection())
        return cursorPosition();
    const B_D(BSimpleCodeEditorDocument);
    QTextCursor tc = d->ptedt->textCursor();
    return tc.selectionEnd();
}

QPoint BSimpleCodeEditorDocument::selectionEndRowColumn() const
{
    if (!hasSelection())
        return cursorPositionRowColumn();
    const B_D(BSimpleCodeEditorDocument);
    QTextCursor tc = d->ptedt->textCursor();
    QTextBlock tb = d->ptedt->document()->findBlock(tc.selectionEnd());
    return QPoint(tc.selectionEnd() - tb.position(), tb.blockNumber());
}

int BSimpleCodeEditorDocument::selectionStart() const
{
    if (!hasSelection())
        return cursorPosition();
    const B_D(BSimpleCodeEditorDocument);
    QTextCursor tc = d->ptedt->textCursor();
    return tc.selectionStart();
}

QPoint BSimpleCodeEditorDocument::selectionStartRowColumn() const
{
    if (!hasSelection())
        return cursorPositionRowColumn();
    const B_D(BSimpleCodeEditorDocument);
    QTextCursor tc = d->ptedt->textCursor();
    QTextBlock tb = d->ptedt->document()->findBlock(tc.selectionStart());
    return QPoint(tc.selectionStart() - tb.position(), tb.blockNumber());
}

void BSimpleCodeEditorDocument::setEditAutoIndentationEnabled(bool enabled)
{
    d_func()->autoIndentation = enabled;
}

void BSimpleCodeEditorDocument::setEditFont(const QFont &fnt)
{
    d_func()->ptedt->setFont(fnt);
}

void BSimpleCodeEditorDocument::setEditTabWidth(BeQt::TabWidth tw)
{
    d_func()->tabWidth = tw;
}

void BSimpleCodeEditorDocument::setLineNumberWidgetVisible(bool b)
{
    d_func()->lnwgt->setVisible(b);
}

void BSimpleCodeEditorDocument::setModification(bool modified)
{
    d_func()->ptedt->document()->setModified(modified);
}

void BSimpleCodeEditorDocument::setReadOnly(bool ro)
{
    bool b = d_func()->ptedt->isReadOnly() != ro;
    d_func()->ptedt->setReadOnly(ro);
    if (b)
        setReadOnlyInternal(ro);
}

QString BSimpleCodeEditorDocument::text(bool) const
{
    return d_func()->ptedt->toPlainText().replace(QChar::ParagraphSeparator, '\n');
}

/*============================== Protected methods =========================*/

void BSimpleCodeEditorDocument::activateWindowImplementation()
{
    d_func()->ptedt->activateWindow();
}

void BSimpleCodeEditorDocument::clearImplementation()
{
    d_func()->ptedt->clear();
}

void BSimpleCodeEditorDocument::copyImplementation()
{
    d_func()->ptedt->copy();
}

QWidget *BSimpleCodeEditorDocument::createEdit(QTextDocument **doc)
{
    return d_func()->createEdit(doc);
}

QPoint BSimpleCodeEditorDocument::cursorPositionRowColumnImplementation() const
{
    QTextCursor tc = d_func()->ptedt->textCursor();
    return QPoint(tc.positionInBlock(), tc.blockNumber());
}

void BSimpleCodeEditorDocument::cutImplementation()
{
    d_func()->ptedt->cut();
}

void BSimpleCodeEditorDocument::deleteSelectionImplementation()
{
    if (isReadOnly() || !hasSelection())
        return;
    QTextCursor tc = d_func()->ptedt->textCursor();
    tc.beginEditBlock();
    tc.removeSelectedText();
    tc.endEditBlock();
}

void BSimpleCodeEditorDocument::deselectTextImplementation()
{
    QTextCursor tc = d_func()->ptedt->textCursor();
    if (!tc.hasSelection())
        return;
    tc.setPosition(tc.selectionEnd());
    d_func()->ptedt->setTextCursor(tc);
}

bool BSimpleCodeEditorDocument::findNextImplementation(const QString &txt, QTextDocument::FindFlags flags, bool cyclic)
{
    if (txt.isEmpty())
        return false;
    bool b = d_func()->ptedt->find(txt, flags);
    if (!b && cyclic) {
        if (flags & QTextDocument::FindBackward)
            d_func()->ptedt->moveCursor(QTextCursor::End);
        else
            d_func()->ptedt->moveCursor(QTextCursor::Start);
        b = d_func()->ptedt->find(txt, flags);
    }
    return b;
}

bool BSimpleCodeEditorDocument::findNextRegexpImplementation(const QRegExp &rx, QTextDocument::FindFlags flags,
                                                            bool cyclic)
{
    if (!rx.isValid() || rx.isEmpty())
        return false;
    QTextDocument *doc = d_func()->ptedt->document();
    QTextCursor tc = doc->find(rx, d_func()->ptedt->textCursor());
    bool b = !tc.isNull();
    if (!b && cyclic) {
        if (flags & QTextDocument::FindBackward)
            d_func()->ptedt->moveCursor(QTextCursor::End);
        else
            d_func()->ptedt->moveCursor(QTextCursor::Start);
        tc = doc->find(rx, d_func()->ptedt->textCursor());
        b = !tc.isNull();
    }
    if (b)
        d_func()->ptedt->setTextCursor(tc);
    return b;
}

void BSimpleCodeEditorDocument::insertTextImplementation(const QString &txt)
{
    d_func()->ptedt->insertPlainText(txt);
}

void BSimpleCodeEditorDocument::moveCursorImplementation(int pos)
{
    B_D(BSimpleCodeEditorDocument);
    QTextBlock tb = d->ptedt->document()->findBlock(pos);
    if (!tb.isValid())
        return;
    QTextCursor tc = d->ptedt->textCursor();
    tc.setPosition(pos);
    d->ptedt->setTextCursor(tc);
}

void BSimpleCodeEditorDocument::moveCursorImplementation(const QPoint &pos)
{
    B_D(BSimpleCodeEditorDocument);
    QTextBlock tb = d->ptedt->document()->findBlockByLineNumber(pos.y());
    if (!tb.isValid())
        return;
    QTextCursor tc = d->ptedt->textCursor();
    tc.setPosition(tb.position() + (pos.x() > 0 ? pos.x() : 0));
    d->ptedt->setTextCursor(tc);
}

void BSimpleCodeEditorDocument::pasteImplementation()
{
    d_func()->ptedt->paste();
}

void BSimpleCodeEditorDocument::redoImplementation()
{
    d_func()->ptedt->redo();
}

void BSimpleCodeEditorDocument::selectAllImplementation()
{
    d_func()->ptedt->selectAll();
}

void BSimpleCodeEditorDocument::selectLinesImplementation(int firstLine, int lastLine)
{
    int bc = d_func()->ptedt->blockCount();
    if (firstLine < 0 || firstLine >= bc)
        return;
    QPoint start(0, firstLine);
    QPoint end(0, firstLine);
    if (lastLine > 0 && lastLine < bc)
        end.setY(lastLine);
    QTextBlock tb = d_func()->ptedt->document()->findBlockByNumber(lastLine);
    if (tb.isValid())
        end.setX(tb.length() - 1);
    selectText(start, end);
}

void BSimpleCodeEditorDocument::selectTextImplementation(const QPoint &start, const QPoint &end)
{
    B_D(BSimpleCodeEditorDocument);
    QTextBlock tbs = d->ptedt->document()->findBlockByLineNumber(start.y());
    if (!tbs.isValid())
        return;
    QTextBlock tbe = d->ptedt->document()->findBlockByLineNumber(end.y());
    if (!tbe.isValid())
        return;
    QTextCursor tc = d->ptedt->textCursor();
    tc.setPosition(tbs.position() + (start.x() > 0 ? start.x() : 0));
    tc.setPosition(tbe.position() + (end.x() > 0 ? end.x() : 0), QTextCursor::KeepAnchor);
    d->ptedt->setTextCursor(tc);
}

void BSimpleCodeEditorDocument::selectTextImplementation(int start, int end)
{
    if (start < 0 || end < 0)
        return;
    QTextCursor tc = d_func()->ptedt->textCursor();
    tc.setPosition(start);
    tc.setPosition(end, QTextCursor::KeepAnchor);
    d_func()->ptedt->setTextCursor(tc);
}

void BSimpleCodeEditorDocument::setExtraSelectionsImplementation(const ExtraSelectionList &list)
{
    d_func()->ptedt->setExtraSelections(list);
}

void BSimpleCodeEditorDocument::setFocusImplementation()
{
    d_func()->ptedt->setFocus();
}

void BSimpleCodeEditorDocument::setTextImplementation(const QString &txt)
{
    d_func()->ptedt->setPlainText(txt);
}

void BSimpleCodeEditorDocument::undoImplementation()
{
    d_func()->ptedt->undo();
}

BAbstractCodeEditorDocument::TextProcessingFunction BSimpleCodeEditorDocument::textPreprocessingFunction() const
{
    return &BSimpleCodeEditorDocumentPrivate::procFunc;
}

QVariant BSimpleCodeEditorDocument::textPreprocessingUserData()
{
    return QVariant((int) editTabWidth());
}
