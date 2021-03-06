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

#include "bcodeedit.h"
#include "bcodeedit_p.h"

#include "babstractcodeeditordocument.h"
#include "bcodeeditordocument.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BTextTools>
#include <BeQtCore/private/bbaseobject_p.h>
#include <BeQtWidgets/BClipboardNotifier>
#include <BeQtWidgets/BGuiTools>
#include <BeQtWidgets/BLineNumberWidget>
#include <BeQtWidgets/BPlainTextEdit>

#include <QAbstractTextDocumentLayout>
#include <QAction>
#include <QApplication>
#include <QBrush>
#include <QChar>
#include <QClipboard>
#include <QDebug>
#include <QEvent>
#include <QFont>
#include <QFontInfo>
#include <QGradient>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMenu>
#include <QMetaObject>
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QPaintEvent>
#include <QPlainTextDocumentLayout>
#include <QPlainTextEdit>
#include <QPoint>
#include <QPointF>
#include <QRect>
#include <QRectF>
#include <QRegExp>
#include <QScrollBar>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QtConcurrentRun>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTextEdit>
#include <QTextLayout>
#include <QTextLine>
#include <QTimer>
#include <QTransform>
#include <QVector>
#include <QWidget>

/*============================================================================
================================ BPlainTextEditExtendedPrivate ===============
============================================================================*/

/*============================== Public constructors =======================*/

BPlainTextEditExtendedPrivate::BPlainTextEditExtendedPrivate(BPlainTextEditExtended *q) :
    BPlainTextEditPrivate(q)
{
    //
}

BPlainTextEditExtendedPrivate::~BPlainTextEditExtendedPrivate()
{
    //
}

/*============================== Static public methods =====================*/

void BPlainTextEditExtendedPrivate::fillBackground(QPainter *painter, const QRectF &rect,
                                                   QBrush brush, QRectF gradientRect)
{
    painter->save();
    Qt::BrushStyle bs = brush.style();
    if (bs >= Qt::LinearGradientPattern && bs <= Qt::ConicalGradientPattern) {
        if (!gradientRect.isNull()) {
            QTransform t = QTransform::fromTranslate(gradientRect.left(), gradientRect.top());
            t.scale(gradientRect.width(), gradientRect.height());
            brush.setTransform(t);
            const_cast<QGradient *>(brush.gradient())->setCoordinateMode(QGradient::LogicalMode);
        }
    } else {
        painter->setBrushOrigin(rect.topLeft());
    }
    painter->fillRect(rect, brush);
    painter->restore();
}

/*============================== Public methods ============================*/

void BPlainTextEditExtendedPrivate::emulateShiftPress()
{
    QKeyEvent e(QKeyEvent::KeyPress, Qt::Key_Shift, Qt::NoModifier);
    QApplication::sendEvent(q_func(), &e);
}

QAbstractTextDocumentLayout::PaintContext BPlainTextEditExtendedPrivate::getPaintContext() const
{
    QAbstractTextDocumentLayout::PaintContext context = q_func()->getPaintContext();
    if (!blockMode || !hasSelection)
        return context;
    QAbstractTextDocumentLayout::Selection sel = context.selections.last();
    context.selections.remove(context.selections.size() - 1);
    foreach (const BPlainTextEditExtended::SelectionRange &sr, selectionRanges) {
        sel.cursor.setPosition(sr.start);
        sel.cursor.setPosition(sr.end, QTextCursor::KeepAnchor);
        context.selections.append(sel);
    }
    return context;
}

void BPlainTextEditExtendedPrivate::init()
{
    B_Q(BPlainTextEditExtended);
    blockMode = false;
    hasSelection = false;
    connect(q, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
}

/*============================== Public slots ==============================*/

void BPlainTextEditExtendedPrivate::selectionChanged()
{
    B_Q(BPlainTextEditExtended);
    QTextCursor tc = q->textCursor();
    hasSelection = tc.hasSelection();
    if (!blockMode)
        return;
    selectionRanges.clear();
    if (!hasSelection)
        return;
    //Assuming all lines have the same length
    //It's also a good idea to set a monospace font
    int start = tc.selectionStart();
    int end = tc.selectionEnd();
    int soffset = start - q->document()->findBlock(start).position();
    int eoffset = end - q->document()->findBlock(end).position();
    if (soffset == eoffset)
        return emulateShiftPress(); //Workaround to update the selection
    int minoffset = qMin<int>(soffset, eoffset);
    int maxoffset = qMax<int>(soffset, eoffset);
    int astart = qMin<int>(start, end);
    int aend = qMax<int>(start, end);
    QTextBlock tb = q->document()->findBlock(astart);
    BPlainTextEditExtended::SelectionRange sr;
    while (tb.isValid()) {
        int bpos = tb.position();
        if (bpos > aend)
            break;
        sr.start = bpos + minoffset;
        sr.end = bpos + qMin(maxoffset, tb.length() - 1);
        selectionRanges.append(sr);
        tb = tb.next();
    }
    //Workaround to update the selection
    emulateShiftPress();
}

/*============================================================================
================================ BPlainTextEditExtended ======================
============================================================================*/

/*============================== Public constructors =======================*/

BPlainTextEditExtended::BPlainTextEditExtended(QWidget *parent) :
    BPlainTextEdit(*new BPlainTextEditExtendedPrivate(this), parent)
{
    d_func()->init();
}

BPlainTextEditExtended::~BPlainTextEditExtended()
{
    //
}

/*============================== Protected constructors ====================*/

BPlainTextEditExtended::BPlainTextEditExtended(BPlainTextEditExtendedPrivate &d, QWidget *parent) :
    BPlainTextEdit(d, parent)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

bool BPlainTextEditExtended::blockMode() const
{
    return d_func()->blockMode;
}

QVector<BPlainTextEditExtended::SelectionRange> BPlainTextEditExtended::selectionRanges() const
{
    return d_func()->selectionRanges;
}

void BPlainTextEditExtended::setBlockMode(bool enabled)
{
    B_D(BPlainTextEditExtended);
    if (enabled == d->blockMode)
        return;
    d->blockMode = enabled;
    updateSelection();
}

void BPlainTextEditExtended::updateSelection()
{
    d_func()->selectionChanged();
    d_func()->emulateShiftPress();
}

/*============================== Protected methods =========================*/

void BPlainTextEditExtended::paintEvent(QPaintEvent *e)
{
    QPainter painter(viewport());
    QPointF offset(contentOffset());
    QRect er = e->rect();
    QRect viewportRect = viewport()->rect();
    bool editable = !isReadOnly();
    QTextBlock block = firstVisibleBlock();
    qreal maximumWidth = document()->documentLayout()->documentSize().width();
    painter.setBrushOrigin(offset);
    int maxX = offset.x() + qMax((qreal) viewportRect.width(), maximumWidth) - document()->documentMargin();
    er.setRight(qMin(er.right(), maxX));
    painter.setClipRect(er);
    QAbstractTextDocumentLayout::PaintContext context = d_func()->getPaintContext(); //Custom paint context
    while (block.isValid()) {
        QRectF r = blockBoundingRect(block).translated(offset);
        QTextLayout *layout = block.layout();
        if (!block.isVisible()) {
            offset.ry() += r.height();
            block = block.next();
            continue;
        }
        if (r.bottom() >= er.top() && r.top() <= er.bottom()) {
            QTextBlockFormat blockFormat = block.blockFormat();
            QBrush bg = blockFormat.background();
            if (bg != Qt::NoBrush) {
                QRectF contentsRect = r;
                contentsRect.setWidth(qMax(r.width(), maximumWidth));
                BPlainTextEditExtendedPrivate::fillBackground(&painter, contentsRect, bg);
            }
            QVector<QTextLayout::FormatRange> selections;
            int blpos = block.position();
            int bllen = block.length();
            for (int i = 0; i < context.selections.size(); ++i) {
                const QAbstractTextDocumentLayout::Selection &range = context.selections.at(i);
                const int selStart = range.cursor.selectionStart() - blpos;
                const int selEnd = range.cursor.selectionEnd() - blpos;
                if (selStart < bllen && selEnd > 0 && selEnd > selStart) {
                    QTextLayout::FormatRange o;
                    o.start = selStart;
                    o.length = selEnd - selStart;
                    o.format = range.format;
                    selections.append(o);
                } else if (!range.cursor.hasSelection() && range.format.hasProperty(QTextFormat::FullWidthSelection)
                           && block.contains(range.cursor.position())) {
                    QTextLayout::FormatRange o;
                    QTextLine l = layout->lineForTextPosition(range.cursor.position() - blpos);
                    o.start = l.textStart();
                    o.length = l.textLength();
                    if (o.start + o.length == bllen - 1)
                        ++o.length;
                    o.format = range.format;
                    selections.append(o);
                }
            }
            bool drawCursor = ((editable || (textInteractionFlags() & Qt::TextSelectableByKeyboard))
                               && context.cursorPosition >= blpos && context.cursorPosition < blpos + bllen);
            bool drawCursorAsBlock = drawCursor && overwriteMode();
            if (drawCursorAsBlock) {
                if (context.cursorPosition == blpos + bllen - 1) {
                    drawCursorAsBlock = false;
                } else {
                    QTextLayout::FormatRange o;
                    o.start = context.cursorPosition - blpos;
                    o.length = 1;
                    o.format.setForeground(palette().base());
                    o.format.setBackground(palette().text());
                    selections.append(o);
                }
            }
            layout->draw(&painter, offset, selections, er);
            if ((drawCursor && !drawCursorAsBlock)
                    || (editable && context.cursorPosition < -1 && !layout->preeditAreaText().isEmpty())) {
                int cpos = context.cursorPosition;
                if (cpos < -1)
                    cpos = layout->preeditAreaPosition() - (cpos + 2);
                else
                    cpos -= blpos;
                layout->drawCursor(&painter, offset, cpos, cursorWidth());
            }
        }
        offset.ry() += r.height();
        if (offset.y() > viewportRect.height())
            break;
        block = block.next();
    }
    if (backgroundVisible() && !block.isValid() && offset.y() <= er.bottom()
            && (centerOnScroll() || verticalScrollBar()->maximum() == verticalScrollBar()->minimum())) {
        painter.fillRect(QRect(QPoint(int(er.left()), int(offset.y())), er.bottomRight()), palette().background());
    }
}

/*============================================================================
================================ BCodeEditPrivate ============================
============================================================================*/

/*============================== Public constructors =======================*/

BCodeEditPrivate::BCodeEditPrivate(BCodeEdit *q) :
    BBaseObjectPrivate(q)
{
    //
}

BCodeEditPrivate::~BCodeEditPrivate()
{
    //
}

/*============================== Static public methods =====================*/

BCodeEdit::SplittedLinesRange BCodeEditPrivate::createSplittedLinesRange()
{
    BCodeEdit::SplittedLinesRange slr;
    slr.firstLineNumber = -1;
    slr.lastLineNumber = -1;
    return slr;
}

QString BCodeEditPrivate::makeBlock(const QString &text, int *length)
{
    QStringList sl = text.split('\n');
    if (sl.isEmpty())
        return text;
    int maxLen = sl.first().length();
    foreach (const QString &line, sl) {
        if (line.length() > maxLen)
            maxLen = line.length();
    }
    foreach (int i, bRangeD(0, sl.size() - 1))
        sl[i].append(QString().fill(' ', maxLen - sl.at(i).length()));
    return bRet(length, maxLen, sl.join("\n"));
}

void BCodeEditPrivate::makeBlock(QString *text, int *length)
{
    if (!text)
        return;
    *text = makeBlock(*text, length);
}

QStringList BCodeEditPrivate::processLine(const QString &line, int ll, BeQt::TabWidth tw)
{
    QStringList sl;
    QString xline = BTextTools::replaceTabs(line, tw);
    int len = xline.length();
    if (len > ll) {
        while (xline.length() > ll) {
            int k = ll;
            while (k > 1 && xline.at(k) != ' ')
                --k;
            int l = k > 1 ? k : ll;
            sl << BTextTools::appendTrailingSpaces(xline.left(l), ll);
            xline.remove(0, l + (k > 1 ? 1 : 0));
        }
        if (!xline.isEmpty())
            sl << BTextTools::appendTrailingSpaces(xline, ll);;
    } else {
        sl << BTextTools::appendTrailingSpaces(xline, ll);
    }
    return sl;
}

BCodeEditPrivate::ProcessTextResult BCodeEditPrivate::processText(const QString &text, int ll, BeQt::TabWidth tw)
{
    ProcessTextResult res;
    QStringList sl = BTextTools::withoutUnsuppottedSymbols(text).split('\n');
    QStringList sln;
    for (int i = 0; i < sl.size(); ++i) {
        QStringList lines = processLine(sl.at(i), ll, tw);
        if (lines.size() > 1) {
            BCodeEdit::SplittedLinesRange r = createSplittedLinesRange();
            r.firstLineNumber = sln.size();
            r.lastLineNumber = r.firstLineNumber + lines.size() - 1;
            if (!res.splittedLinesRanges.isEmpty()
                    && res.splittedLinesRanges.last().lastLineNumber == r.firstLineNumber - 1)
                res.splittedLinesRanges.last().lastLineNumber = r.lastLineNumber;
            else
                res.splittedLinesRanges << r;
        }
        sln << lines;
    }
    res.newText = sln.join("\n");
    return res;
}

bool BCodeEditPrivate::testBlock(const QString &text, int *length)
{
    return !text.isEmpty() ? testBlock(text.split('\n'), length) : false;
}

bool BCodeEditPrivate::testBlock(const QStringList &lines, int *length)
{
    if (lines.isEmpty())
        return false;
    int len = lines.first().length();
    foreach (const QString &line, lines) {
        if (line.length() != len)
            return false;
    }
    return bRet(length, len, true);
}

/*============================== Public methods ============================*/

void BCodeEditPrivate::blockHighlighter(bool block)
{
    BCodeEditorDocument *doc = qobject_cast<BCodeEditorDocument *>(q_func()->parentWidget());
    if (!doc)
        return;
    doc->blockHighlighter(block);
}

BCodeEdit::SplittedLinesRange BCodeEditPrivate::deleteSelection()
{
    QTextCursor tc = ptedt->textCursor();
    if (!tc.hasSelection())
        return createSplittedLinesRange();
    tc.beginEditBlock();
    int pos = qMin(tc.selectionStart(), tc.selectionEnd());
    BCodeEdit::SplittedLinesRange slr = createSplittedLinesRange();
    if (blockMode) {
        QVector<BPlainTextEditExtended::SelectionRange> ranges = ptedt->selectionRanges();
        const BPlainTextEditExtended::SelectionRange r = ranges.first();
        if (r.end == pos)
            pos -= (r.end - r.start);
        foreach(const BPlainTextEditExtended::SelectionRange &range, ranges) {
            tc.setPosition(range.start);
            tc.setPosition(range.end, QTextCursor::KeepAnchor);
            tc.removeSelectedText();
            QString text = BTextTools::appendTrailingSpaces(tc.block().text(), lineLength);
            tc.movePosition(QTextCursor::StartOfBlock);
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            tc.insertText(text);
        }
    } else {
        tc.removeSelectedText();
        QString text = tc.block().text();
        QStringList sl = processLine(text, lineLength, tabWidth);
        if (sl.size() > 1) {
            slr.firstLineNumber = tc.block().blockNumber();
            slr.lastLineNumber = slr.firstLineNumber + sl.size();
        }
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.insertText(sl.join("\n"));
    }
    tc.setPosition(pos);
    ptedt->setTextCursor(tc);
    tc.endEditBlock();
    return slr;
}

void BCodeEditPrivate::emitLineSplitted(const BCodeEdit::SplittedLinesRange &range)
{
    emitLinesSplitted(QList<BCodeEdit::SplittedLinesRange>() << range);
}

void BCodeEditPrivate::emitLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> &ranges)
{
    if (ranges.size() && onceEdited)
        QMetaObject::invokeMethod(ptedt->document(), "setModified", Qt::QueuedConnection, Q_ARG(bool, true));
    if (ranges.size() > 1) {
        QMetaObject::invokeMethod(q_func(), "linesSplitted", Q_ARG(QList<BCodeEdit::SplittedLinesRange>, ranges));
    } else if (ranges.size() == 1 && ranges.first().firstLineNumber >= 0) {
        QMetaObject::invokeMethod(q_func(), "lineSplitted",
                                  Q_ARG(const BCodeEdit::SplittedLinesRange, ranges.first()));
    }
}

bool BCodeEditPrivate::eventFilter(QObject *obj, QEvent *e)
{
    switch (e->type()) {
    case QEvent::KeyPress:
        return keyPressEvent(static_cast<QKeyEvent *>(e));
    case QEvent::MouseButtonDblClick:
        return mouseDoubleClickEvent(static_cast<QMouseEvent *>(e));
    case QEvent::MouseButtonPress:
        return mousePressEvent(static_cast<QMouseEvent *>(e));
    case QEvent::DragEnter:
    case QEvent::DragMove:
    case QEvent::DragLeave:
    case QEvent::Drop:
        return true;
    default:
        return QObject::eventFilter(obj, e);
    }
}

void BCodeEditPrivate::handleBackspace()
{
    if (ptedt->isReadOnly())
        return;
    QTextCursor tc = ptedt->textCursor();
    if (tc.hasSelection())
        return emitLineSplitted(deleteSelection());
    if (tc.position() == 0)
        return;
    tc.beginEditBlock();
    int posb = tc.positionInBlock();
    if (posb) {
        int pos = tc.position();
        tc.setPosition(pos - 1);
        QString text = tc.block().text();
        if (text.at(posb - 1) != ' ' || BTextTools::removeTrailingSpaces(text).length() >= posb) {
            tc.setPosition(pos, QTextCursor::KeepAnchor);
            tc.removeSelectedText();
            tc.movePosition(QTextCursor::EndOfBlock);
            tc.insertText(" ");
            tc.setPosition(pos - 1);
        }
    } else {
        QTextBlock tb = tc.block();
        QTextBlock tbp = tb.previous();
        QString text = BTextTools::removeTrailingSpaces(tb.text());
        int len = text.length();
        int plen = BTextTools::removeTrailingSpaces(tbp.text()).length();
        if (plen + len > lineLength)
            return tc.endEditBlock();
        tc.movePosition(QTextCursor::EndOfBlock);
        int ppos = tbp.position();
        tc.setPosition(ppos + plen, QTextCursor::KeepAnchor);
        tc.insertText(text + QString().fill(' ', lineLength - plen - len));
        tc.setPosition(ppos + plen);
    }
    ptedt->setTextCursor(tc);
    tc.endEditBlock();
}

void BCodeEditPrivate::handleCtrlBackspace()
{
    if (ptedt->isReadOnly())
        return;
    QTextCursor tc = ptedt->textCursor();
    if (tc.hasSelection())
        return emitLineSplitted(deleteSelection());
    tc.beginEditBlock();
    QString text = tc.block().text();
    int posb = tc.positionInBlock();
    int i = posb;
    while (--i >= 0 && text.at(i) == ' ')
        ;
    while (--i >= 0 && text.at(i) != ' ')
        ;
    i += 1;
    if (i < 0) {
        tc.setPosition(tc.block().position());
        ptedt->setTextCursor(tc);
        handleBackspace();
        if (ptedt->textCursor().position() > 0)
            handleCtrlBackspace();
    } else {
        int bpos = tc.block().position();
        tc.setPosition(bpos + i, QTextCursor::KeepAnchor);
        tc.removeSelectedText();
        tc.movePosition(QTextCursor::EndOfBlock);
        tc.insertText(QString().fill(' ', posb - i));
        tc.setPosition(bpos + i);
        ptedt->setTextCursor(tc);
    }
    tc.endEditBlock();
}

void BCodeEditPrivate::handleCtrlDelete()
{
    if (ptedt->isReadOnly())
        return;
    QTextCursor tc = ptedt->textCursor();
    if (tc.hasSelection())
        return emitLineSplitted(deleteSelection());
    tc.beginEditBlock();
    QString text = tc.block().text();
    int posb = tc.positionInBlock();
    int i = posb;
    while (++i < text.length() && text.at(i) == ' ') {
        //Do nothing
    }
    while (++i < text.length() && text.at(i) != ' ') {
        //Do nothing
    }
    if (i >= text.length()) {
        handleDelete();
        if (!BTextTools::removeTrailingSpaces(ptedt->textCursor().block().text().mid(posb)).isEmpty())
            handleCtrlDelete();
    } else {
        int bpos = tc.block().position();
        tc.setPosition(bpos + i, QTextCursor::KeepAnchor);
        tc.removeSelectedText();
        tc.movePosition(QTextCursor::EndOfBlock);
        tc.insertText(QString().fill(' ', i - posb));
        tc.setPosition(bpos + posb);
        ptedt->setTextCursor(tc);
    }
    tc.endEditBlock();
}

void BCodeEditPrivate::handleCtrlLeft()
{
    QTextCursor tc = ptedt->textCursor();
    int posb = tc.positionInBlock();
    QTextBlock tb = tc.block();
    QString text = BTextTools::removeTrailingSpaces(tb.text());
    int bpos = tb.position();
    if (!posb) {
        QTextBlock tbp = tb.previous();
        if (tbp.isValid()) {
            tc.setPosition(tbp.position() + BTextTools::removeTrailingSpaces(tbp.text()).length());
            ptedt->setTextCursor(tc);
        }
        return;
    }
    if (text.length() < posb) {
        tc.setPosition(bpos + text.length());
        ptedt->setTextCursor(tc);
        return;
    }
    if (text.at(posb - 1).isLetterOrNumber()) {
        while (posb > 0 && text.at(posb - 1).isLetterOrNumber())
            --posb;
    } else if (text.at(posb - 1).isSpace()) {
        while (posb > 0 && text.at(posb - 1).isSpace())
            --posb;
    }
    else if (text.at(posb - 1).isSymbol() || text.at(posb - 1).isPunct()) {
        while (posb > 0 && (text.at(posb - 1).isSymbol() || text.at(posb - 1).isPunct()))
            --posb;
    }
    while (posb > 0 && text.at(posb - 1).isSpace())
        --posb;
    tc.setPosition(bpos + posb);
    ptedt->setTextCursor(tc);
}

void BCodeEditPrivate::handleCtrlRight()
{
    QTextCursor tc = ptedt->textCursor();
    int posb = tc.positionInBlock();
    QTextBlock tb = tc.block();
    QString text = BTextTools::removeTrailingSpaces(tb.text());
    int bpos = tb.position();
    if (text.length() <= posb) {
        QTextBlock tbn = tb.next();
        if (tbn.isValid())
            tc.setPosition(tbn.position());
        else
            tc.setPosition(bpos + text.length());
        ptedt->setTextCursor(tc);
        return;
    }
    if (text.at(posb).isLetterOrNumber()) {
        while (posb < text.length() && text.at(posb).isLetterOrNumber())
            ++posb;
    } else if (text.at(posb).isSpace()) {
        while (posb < text.length() && text.at(posb).isSpace())
            ++posb;
    } else if (text.at(posb).isSymbol() || text.at(posb).isPunct()) {
        while (posb < text.length() && (text.at(posb).isSymbol() || text.at(posb).isPunct()))
            ++posb;
    }
    while (posb < text.length() && text.at(posb).isSpace())
        ++posb;
    tc.setPosition(bpos + posb);
    ptedt->setTextCursor(tc);
}

void BCodeEditPrivate::handleDelete()
{
    if (ptedt->isReadOnly())
        return;
    QTextCursor tc = ptedt->textCursor();
    if (tc.hasSelection())
        return emitLineSplitted(deleteSelection());
    int posb = tc.positionInBlock();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    if (posb < BTextTools::removeTrailingSpaces(text).length()) {
        tc.beginEditBlock();
        int pos = tc.position();
        tc.setPosition(pos + 1, QTextCursor::KeepAnchor);
        tc.removeSelectedText();
        tc.movePosition(QTextCursor::EndOfBlock);
        tc.insertText(" ");
        tc.setPosition(pos);
    } else {
        QTextBlock tbn = tb.next();
        if (!tbn.isValid())
            return;
        QString ntext = BTextTools::removeTrailingSpaces(tbn.text());
        if (ntext.isEmpty()) {
            tc.beginEditBlock();
            int pos = tc.position();
            tc.movePosition(QTextCursor::EndOfBlock);
            tc.movePosition(QTextCursor::NextBlock);
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            tc.removeSelectedText();
            tc.deletePreviousChar();
            tc.setPosition(pos);
        } else {
            text = text.left(posb);
            if (text.length() + ntext.length() > lineLength)
                return;
            tc.beginEditBlock();
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            tc.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            tc.removeSelectedText();
            tc.insertText(ntext + QString().fill(' ', lineLength - text.length() - ntext.length()));
            tc.setPosition(tc.block().position() + posb);
        }
    }
    ptedt->setTextCursor(tc);
    tc.endEditBlock();
}

void BCodeEditPrivate::handleEnd(bool ctrl)
{
    QTextCursor tc = ptedt->textCursor();
    QTextBlock tb = ctrl ? ptedt->document()->lastBlock() : tc.block();
    QString text = tb.text();
    int i = text.length() - 1;
    while (i >= 0 && text.at(i) == ' ')
        --i;
    tc.setPosition(tb.position() + i + 1);
    ptedt->setTextCursor(tc);
}

void BCodeEditPrivate::handleLeft(bool shift)
{
    QTextCursor tc = ptedt->textCursor();
    int pos = tc.position();
    if (!pos) {
        if (!shift) {
            tc.setPosition(0);
            ptedt->setTextCursor(tc);
        }
        return;
    }
    QTextCursor::MoveMode mode = shift ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor;
    int npos;
    if (tc.positionInBlock()) {
        npos = pos - 1;
    } else {
        QTextBlock tb = tc.block().previous();
        QString text = tb.text();
        int i = text.length() - 1;
        while (i >= 0 && text.at(i) == ' ')
            --i;
        npos = tb.position() + i + 1;
    }
    tc.setPosition(npos, mode);
    ptedt->setTextCursor(tc);
    ptedt->updateSelection();
}

void BCodeEditPrivate::handleReturn()
{
    if (ptedt->isReadOnly())
        return;
    QTextCursor tc = ptedt->textCursor();
    tc.beginEditBlock();
    deleteSelection();
    tc = ptedt->textCursor();
    int posb = tc.positionInBlock();
    QTextBlock tbp = tc.block();
    QString text = tbp.text();
    int i = 0;
    while (i < posb && text.at(i) == ' ')
        ++i;
    QString ltext = BTextTools::appendTrailingSpaces(text.left(posb), lineLength);
    QString rtext = BTextTools::appendTrailingSpaces(text.right(text.length() - posb).prepend(QString().fill(' ', i)),
                                                     lineLength);
    tc.movePosition(QTextCursor::StartOfBlock);
    tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    tc.insertText(ltext);
    tc.insertBlock();
    tc.insertText(rtext);
    tc.movePosition(QTextCursor::StartOfBlock);
    tc.setPosition(tc.position() + (autoIndentation ? i : 0));
    ptedt->setTextCursor(tc);
    tc.endEditBlock();
    requestRehighlightBlock(tbp); //Not sure if this is necessary
}

void BCodeEditPrivate::handleShiftEnd()
{
    QTextCursor tc = ptedt->textCursor();
    QTextBlock tb = tc.block();
    int d = BTextTools::removeTrailingSpaces(tb.text()).length() - tc.positionInBlock();
    if (d < 1)
        return;
    tc.setPosition(tc.position() + d, QTextCursor::KeepAnchor);
    ptedt->setTextCursor(tc);
}

void BCodeEditPrivate::handleSpace()
{
    if (ptedt->isReadOnly())
        return;
    QTextCursor tc = ptedt->textCursor();
    tc.beginEditBlock();
    if (hasSelection)
        deleteSelection();
    tc = ptedt->textCursor();
    QString text = tc.block().text();
    if (tc.positionInBlock() == text.length()) {
        tc.insertBlock();
        tc.insertText(QString().fill(' ', lineLength));
        tc.movePosition(QTextCursor::StartOfBlock);
        ptedt->setTextCursor(tc);
    } else if (!text.mid(tc.positionInBlock()).contains(QRegExp("\\S"))) {
        tc.setPosition(tc.position() + 1);
        ptedt->setTextCursor(tc);
    } else {
        insertText(" ", true);
    }
    tc.endEditBlock();
}

void BCodeEditPrivate::handleTab()
{
    if (ptedt->isReadOnly())
        return;
    QTextCursor tc = ptedt->textCursor();
    tc.beginEditBlock();
    if (hasSelection)
        deleteSelection();
    tc = ptedt->textCursor();
    QString text = tc.block().text();
    int posb = tc.positionInBlock();
    int spcount = tabWidth - (posb < tabWidth ? posb : posb % tabWidth);
    if (!spcount)
        spcount = tabWidth;
    if (text.length() == posb) {
        tc.insertBlock();
        tc.insertText(QString().fill(' ', lineLength));
        tc.setPosition(tc.block().position() + tabWidth);
        ptedt->setTextCursor(tc);
    } else if (!text.mid(tc.positionInBlock()).contains(QRegExp("\\S"))) {
        tc.setPosition(tc.position() + spcount);
        ptedt->setTextCursor(tc);
    } else {
        insertText(QString().fill(' ', spcount), true);
    }
    tc.endEditBlock();
}

void BCodeEditPrivate::init()
{
    connect(BClipboardNotifier::instance(), SIGNAL(hasTextChanged(bool)), this, SLOT(updatePasteAvailable(bool)));
    onceEdited = false;
    blockMode = false;
    lineLength = 120;
    tmpLineLength = -1;
    tabWidth = BeQt::TabWidth4;
    autoIndentation = true;
    hasSelection = false;
    copyAvailable = false;
    pasteAvailable = BClipboardNotifier::hasText();
    undoAvailable = false;
    redoAvailable = false;
    buisy = false;
    cursorPosition = 0;
    B_Q(BCodeEdit);
    QHBoxLayout *hlt = new QHBoxLayout(q);
      hlt->setContentsMargins(0, 0, 0, 0);
      ptedt = new BPlainTextEditExtended(q);
        ptedt->setLineWrapMode(QPlainTextEdit::NoWrap);
        ptedt->setWordWrapMode(QTextOption::NoWrap);
        ptedt->setDragEnabled(false);
        ptedt->installEventFilter(this);
        ptedt->viewport()->installEventFilter(this);
        ptedt->setFont(BGuiTools::createMonospaceFont());
        setTextToEmptyLine();
        ptedt->document()->setModified(false);
        connect(ptedt, SIGNAL(cursorPositionChanged()), this, SLOT(updateCursorPosition()));
        connect(ptedt->document(), SIGNAL(modificationChanged(bool)), this, SLOT(emitModificationChanged(bool)));
        connect(ptedt, SIGNAL(textChanged()), this, SLOT(emitTextChanged()));
        connect(ptedt, SIGNAL(selectionChanged()), this, SLOT(emitSelectionChanged()));
        connect(ptedt, SIGNAL(selectionChanged()), this, SLOT(updateHasSelection()));
        connect(ptedt, SIGNAL(copyAvailable(bool)), this, SLOT(updateCopyAvailable(bool)));
        connect(ptedt, SIGNAL(undoAvailable(bool)), this, SLOT(updateUndoAvailable(bool)));
        connect(ptedt, SIGNAL(redoAvailable(bool)), this, SLOT(updateRedoAvailable(bool)));
      hlt->addWidget(ptedt);
    lnwgt = new BLineNumberWidget(ptedt);
    connect(q, SIGNAL(buisyChanged(bool)), this, SLOT(delayedSetLineLength()));
}

void BCodeEditPrivate::insertText(const QString &txt, bool asKeyPress)
{
    if (q_func()->isBuisy())
        return;
    QTextCursor tc = ptedt->textCursor();
    tc.beginEditBlock();
    QList<BCodeEdit::SplittedLinesRange> ranges;
    BCodeEdit::SplittedLinesRange range = deleteSelection();
    if (range.firstLineNumber >= 0)
        ranges << range;
    if (txt.isEmpty())
        return tc.endEditBlock();
    tc = ptedt->textCursor();
    int initialPos = tc.position();
    int finalPos = -1;
    int posb = tc.positionInBlock();
    QString btext = tc.block().text();
    int lind = BTextTools::removeTrailingSpaces(btext).lastIndexOf(' ');
    int tcpos = (lind >= posb && (lineLength - lind) >= txt.length()) ? (tc.position() + txt.length()) : -1;
    QString ltext = btext.left(posb);
    QString rtext = BTextTools::removeTrailingSpaces(btext.right(btext.length() - posb));
    QStringList sl = BTextTools::replaceTabs(BTextTools::withoutUnsuppottedSymbols(txt), tabWidth).split('\n');
    int blen = 0;
    if (blockMode && !asKeyPress && testBlock(sl, &blen)) {
        int offset = ltext.length();
        QTextBlock tb = tc.block();
        int  i = 0;
        while (tb.isValid() && i < sl.size()) {
            int bpos = tb.position();
            QString t = tb.text();
            t.insert(offset, sl.at(i));
            if (t.length() > lineLength)
                t.truncate(lineLength);
            BTextTools::appendTrailingSpaces(&t, lineLength);
            tc.setPosition(bpos);
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            tc.insertText(t);
            tb = tb.next();
            ++i;
        }
        QString spaces;
        spaces.fill(' ', offset);
        while (i < sl.size()) {
            tc.insertBlock();
            QString &t = sl[i];
            t.prepend(spaces);
            if (t.length() > lineLength)
                t.truncate(lineLength);
            BTextTools::appendTrailingSpaces(&t, lineLength);
            tc.insertText(t);
            ++i;
        }
        tb = tc.block().next();
        if (!tb.isValid()) {
            tc.movePosition(QTextCursor::EndOfBlock);
            tc.insertBlock();
            tc.insertText(QString().fill(' ', lineLength));
            finalPos = tc.block().position() + offset;
        } else {
            finalPos = tb.position() + offset;
        }
    } else {
        //Workaround for lines containing spaces only
        foreach (int i, bRangeD(0, sl.size() - 1))
        {
            QString l = BTextTools::removeTrailingSpaces(sl.at(i));
            if (!l.isEmpty())
                sl[i] = l;
        }
        //End of the workaround
        bool kostyleeque = sl.last().isEmpty();
        sl.first().prepend(ltext);
        sl.last().append(rtext);
        ProcessTextResult res = processText(sl.join("\n"), lineLength, tabWidth);
        int boffset = tc.block().blockNumber();
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        int posmod = 0;
        //Workaround for correct text splitting
        QString nbt;
        if (asKeyPress && tc.block().next().isValid() && res.newText.length() <= lineLength * 2 + 1)
            nbt = BTextTools::removeTrailingSpaces(tc.block().next().text());
        QStringList ntl;
        if (!nbt.isEmpty())
            ntl = res.newText.split('\n');
        if (ntl.size() > 1 && !sl.last().isEmpty()) {
            QStringList ntl = res.newText.split('\n');
            if (nbt.length() + BTextTools::removeTrailingSpaces(ntl.last()).length() < lineLength) {
                BTextTools::removeTrailingSpaces(&ntl.last());
                ntl.last() += nbt;
                posmod -= nbt.length();
                BTextTools::appendTrailingSpaces(&ntl.last(), lineLength);
                res.newText = ntl.join("\n");
                tc.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
                tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            }
        }
        //End of the workaround
        tc.insertText(res.newText);
        int pos = lineLength;
        QString btext = tc.block().text();
        while (pos > 0 && btext.at(pos - 1) == ' ')
            --pos;
        if (pos > 0)
            pos -= rtext.length();
        if (!res.splittedLinesRanges.isEmpty()) {
            if (ltext.length() + txt.length() <= lineLength)
                pos -= 1;
            ranges = res.splittedLinesRanges;
            foreach (int i, bRangeD(0, ranges.size() - 1)) {
                ranges[i].firstLineNumber += boffset;
                ranges[i].lastLineNumber += boffset;
            }
        }
        if (tcpos > 0 && sl.size() == 1)
            finalPos = tcpos;
        else
            finalPos = tc.block().position() + (!kostyleeque ? (pos + posmod) : 0);
    }
    tc.setPosition(finalPos);
    ptedt->setTextCursor(tc);
    QTextBlock initialBlock = ptedt->document()->findBlock(initialPos);
    QTextBlock finalBlock = ptedt->document()->findBlock(finalPos);
    requestRehighlightBlock(initialBlock);
    while (initialBlock != finalBlock) {
        initialBlock = initialBlock.next();
        requestRehighlightBlock(initialBlock);
    }
    tc.endEditBlock();
    emitLinesSplitted(ranges);
    q_func()->setFocus();
    ptedt->ensureCursorVisible();
    onceEdited = true;
}

bool BCodeEditPrivate::keyPressEvent(QKeyEvent *e)
{
    int modifiers = e->modifiers();
    int key = e->key();
    switch (modifiers) {
    case Qt::NoModifier:
        switch (key) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            handleReturn();
            return true;
        case Qt::Key_Space:
            handleSpace();
            return true;
        case Qt::Key_Tab:
            handleTab();
            return true;
        case Qt::Key_Backspace:
            handleBackspace();
            return true;
        case Qt::Key_Delete:
            handleDelete();
            return true;
        case Qt::Key_End:
            handleEnd();
            return true;
        case Qt::Key_Left:
            handleLeft();
            return true;
        default:
            break;
        }
        break;
    case Qt::ControlModifier:
        switch (key) {
        case Qt::Key_Z:
            q_func()->undo();
            return true;
        case Qt::Key_X:
            q_func()->cut();
            return true;
        case Qt::Key_C:
            q_func()->copy();
            return true;
        case Qt::Key_V:
            q_func()->paste();
            return true;
        case Qt::Key_A:
            seletAll();
            return true;
        case Qt::Key_End:
            handleEnd(true);
            return true;
        case Qt::Key_Backspace:
            handleCtrlBackspace();
            return true;
        case Qt::Key_Delete:
            handleCtrlDelete();
            return true;
        case Qt::Key_Left:
            handleCtrlLeft();
            return true;
        case Qt::Key_Right:
            handleCtrlRight();
            return true;
        case Qt::Key_S:
            return !q_func()->isModified(); //Workaround to prevent entering 'S' when 'Save' action is disabled
            //Very "dirty" workaround. Maybe a Qt bug?
        case Qt::Key_Return:
        case Qt::Key_Enter:
            handleReturn();
            return true;
        default:
            break;
        }
        break;
    case Qt::ShiftModifier:
        switch (key) {
        case Qt::Key_Space:
            handleSpace();
            return true;
        case Qt::Key_Backspace:
            handleBackspace();
            return true;
        case Qt::Key_Left:
            handleLeft(true);
            return true;
        case Qt::Key_End:
            handleShiftEnd();
            return true;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            handleReturn();
            return true;
        default:
            break;
        }
        break;
    case Qt::AltModifier:
        switch (key) {
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            move(key);
            return true;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            handleReturn();
            return true;
        default:
            break;
        }
        break;
    case BeQt::ContorlShiftModifier:
        switch (key) {
        case Qt::Key_Z:
            q_func()->redo();
            return true;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            handleReturn();
            return true;
        default:
            break;
        }
        break;
    case BeQt::KeypadShiftModifier:
    case BeQt::KeypadAltModifier:
    case BeQt::KeypadControlShiftModifier:
    case BeQt::KeypadControlAltModifier:
        switch (key) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            handleReturn();
            return true;
        default:
            break;
        }
        break;
    case Qt::KeypadModifier:
        switch (key) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            handleReturn();
            return true;
        case Qt::Key_End:
            handleEnd();
            return true;
        default:
            break;
        }
        break;
    case BeQt::KeypadControlModifier:
        switch (key) {
        case Qt::Key_End:
            handleEnd(true);
            return true;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            handleReturn();
            return true;
        default:
            break;
        }
        break;
    default:
        break;
    }
    QString text = e->text();
    if (!text.isEmpty()) {
        QChar c = text.at(0);
        if (c.isPrint() || c == '\t')
            insertText(QString(c), true);
        return true;
    }
    return false;
}

bool BCodeEditPrivate::mouseDoubleClickEvent(QMouseEvent *)
{
    QTextCursor tc = ptedt->textCursor();
    QString text = tc.block().text();
    int posb = tc.positionInBlock();
    int lposb = posb - 1;
    return ((lposb < 0 || text.at(lposb) == ' ') && text.at(posb) == ' ');
}

bool BCodeEditPrivate::mousePressEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton)
        return false;
    QTextCursor tc = ptedt->cursorForPosition(e->pos());
    if (tc.positionInBlock() != lineLength)
        return false;
    QTextBlock tb = tc.block();
    if (tb.next().isValid())
        return false;
    tc.setPosition(tb.position() + BTextTools::removeTrailingSpaces(tb.text()).length());
    ptedt->setTextCursor(tc);
    return true;
}

void BCodeEditPrivate::move(int key)
{
    if (ptedt->isReadOnly())
        return;
    QTextCursor tc = ptedt->textCursor();
    if (!tc.hasSelection())
        return;
    tc.beginEditBlock();
    int start = tc.selectionStart();
    int end = tc.selectionEnd();
    if (blockMode) {
        QVector<BPlainTextEditExtended::SelectionRange> ranges = ptedt->selectionRanges();
        if (Qt::Key_Left == key || Qt::Key_Right == key) {
            QTextCursor tcr = tc;
            foreach (const BPlainTextEditExtended::SelectionRange &range, ranges) {
                tcr.setPosition(range.start);
                tcr.setPosition(range.end, QTextCursor::KeepAnchor);
                QString text = tcr.selectedText();
                if (Qt::Key_Left == key)
                    text.remove(0, 1).append(' ');
                else
                    text.remove(text.length() - 1, 1).prepend(' ');
                tcr.insertText(text);
            }
        } else if (Qt::Key_Up == key || Qt::Key_Down == key) {
            QTextCursor tcr = tc;
            const int lb = (Qt::Key_Up == key) ? 0 : ranges.size() - 1;
            const int ub = (Qt::Key_Up == key) ? ranges.size() - 1 : 0;
            const int m = (Qt::Key_Up == key) ? 1 : -1;
            for (int i = lb; i != ub; i += m) {
                tcr.setPosition(ranges.at(i + m).start);
                tcr.setPosition(ranges.at(i + m).end, QTextCursor::KeepAnchor);
                QString text = tcr.selectedText();
                tcr.setPosition(ranges.at(i).start);
                tcr.setPosition(ranges.at(i).end, QTextCursor::KeepAnchor);
                tcr.insertText(text);
            }
            tcr.setPosition(ranges.at(ub).start);
            tcr.setPosition(ranges.at(ub).end, QTextCursor::KeepAnchor);
            tcr.insertText(QString().fill(' ', tcr.selectionEnd() - tcr.selectionStart()));
        }
    } else {
        QString text = tc.selectedText();
        if (Qt::Key_Left == key || Qt::Key_Right == key) {
            QStringList sl = text.split(QChar::ParagraphSeparator);
            if (Qt::Key_Left == key) {
                foreach (int i, bRangeD(0, sl.size() - 1)) {
                    if (!sl.at(i).isEmpty())
                        sl[i].remove(0, 1).append(' ');
                }
            } else {
               foreach (int i, bRangeD(0, sl.size() - 1)) {
                   if (!sl.at(i).isEmpty())
                       sl[i].remove(sl.at(i).length() - 1, 1).prepend(' ');
               }
            }
            tc.insertText(sl.join("\n"));
        } else if (Qt::Key_Up == key) {
            tc.insertText(text.toUpper());
        } else if (Qt::Key_Down == key) {
            tc.insertText(text.toLower());
        }
    }
    tc.setPosition(start);
    tc.setPosition(end, QTextCursor::KeepAnchor);
    ptedt->setTextCursor(tc);
    tc.endEditBlock();
}

void BCodeEditPrivate::requestRehighlightBlock(const QTextBlock &block)
{
    BAbstractCodeEditorDocument *doc = qobject_cast<BAbstractCodeEditorDocument *>(q_func()->parentWidget());
    if (!doc)
        return;
    doc->rehighlightBlock(block);
}

void BCodeEditPrivate::seletAll()
{
    if (blockMode)
        return ptedt->selectAll();
    QTextCursor tc = ptedt->textCursor();
    QTextBlock tbl = ptedt->document()->lastBlock();
    int len = BTextTools::removeTrailingSpaces(tbl.text()).length();
    tc.movePosition(QTextCursor::Start);
    tc.setPosition(tbl.position() + len, QTextCursor::KeepAnchor);
    ptedt->setTextCursor(tc);
}

void BCodeEditPrivate::setBuisy(bool b)
{
    if (b == buisy)
        return;
    buisy = b;
    QMetaObject::invokeMethod(q_func(), "buisyChanged", Q_ARG(bool, b));
}

void BCodeEditPrivate::setText(const QString &txt, int asyncIfLongerThan)
{
    if (txt.isEmpty())
       return setTextToEmptyLine();
    if (asyncIfLongerThan > 0 && txt.length() > asyncIfLongerThan) {
        setBuisy(true);
        ptedt->setEnabled(false);
        ptedt->setPlainText(tr("Processing content, please wait...", "ptedt text"));
        ProcessTextResultFuture future = QtConcurrent::run(&BCodeEditPrivate::processText, txt, lineLength, tabWidth);
        ProcessTextResultFutureWatcher *watcher = new ProcessTextResultFutureWatcher(this);
        watcher->setFuture(future);
        connect(watcher, SIGNAL(finished()), this, SLOT(parceTaskFinished()));
    } else {
        ProcessTextResult res = processText(txt, lineLength, tabWidth);
        blockHighlighter(true);
        ptedt->setPlainText(res.newText);
        QTextCursor tc = ptedt->textCursor();
        tc.movePosition(QTextCursor::Start);
        ptedt->setTextCursor(tc);
        blockHighlighter(false);
        ptedt->setFocus();
        emitLinesSplitted(res.splittedLinesRanges);
        onceEdited = true;
    }
}

/*============================== Public slots ==============================*/

void BCodeEditPrivate::delayedSetLineLength()
{
    if (buisy || tmpLineLength < 1)
        return;
    q_func()->setEditLineLength(tmpLineLength);
    tmpLineLength = -1;
}

void BCodeEditPrivate::emitModificationChanged(bool modified)
{
    QMetaObject::invokeMethod(q_func(), "modificationChanged", Q_ARG(bool, modified));
}

void BCodeEditPrivate::emitSelectionChanged()
{
    QMetaObject::invokeMethod(q_func(), "selectionChanged");
}

void BCodeEditPrivate::emitTextChanged()
{
    QMetaObject::invokeMethod(q_func(), "textChanged");
}

void BCodeEditPrivate::parceTaskFinished()
{
    ProcessTextResultFutureWatcher *watcher = dynamic_cast<ProcessTextResultFutureWatcher *>(sender());
    if (!watcher)
        return;
    ProcessTextResult res = watcher->result();
    delete watcher;
    ptedt->setEnabled(true);
    blockHighlighter(true);
    ptedt->setPlainText(res.newText);
    QTextCursor tc = ptedt->textCursor();
    tc.movePosition(QTextCursor::Start);
    ptedt->setTextCursor(tc);
    blockHighlighter(false);
    ptedt->setFocus();
    setBuisy(false);
    emitLinesSplitted(res.splittedLinesRanges);
    onceEdited = true;
}

void BCodeEditPrivate::setTextToEmptyLine()
{
    ptedt->setPlainText(QString().fill(' ', lineLength));
    QTextCursor tc = ptedt->textCursor();
    tc.movePosition(QTextCursor::Start);
    ptedt->setTextCursor(tc);
}

void BCodeEditPrivate::updateCopyAvailable(bool available)
{
    bool b1 = !ptedt->isReadOnly() && copyAvailable;
    bool b2 = copyAvailable;
    copyAvailable = available;
    if (b1 != (!ptedt->isReadOnly() && copyAvailable))
        QMetaObject::invokeMethod(q_func(), "cutAvailableChanged", Q_ARG(bool, !ptedt->isReadOnly() && copyAvailable));
    if (b2 != copyAvailable)
        QMetaObject::invokeMethod(q_func(), "copyAvailableChanged", Q_ARG(bool, copyAvailable));
}

void BCodeEditPrivate::updateCursorPosition()
{
    QTextCursor tc = ptedt->textCursor();
    cursorPosition = tc.position();
    cursorPositionRowColumn = QPoint(tc.positionInBlock(), tc.blockNumber());
    QMetaObject::invokeMethod(q_func(), "cursorPositionChanged", Q_ARG(int, cursorPosition));
    QMetaObject::invokeMethod(q_func(), "cursorPositionChanged", Q_ARG(QPoint, cursorPositionRowColumn));
}

void BCodeEditPrivate::updateHasSelection()
{
    bool b = hasSelection;
    hasSelection = ptedt->textCursor().hasSelection();
    if (b != hasSelection)
        QMetaObject::invokeMethod(q_func(), "hasSelectionChanged", Q_ARG(bool, hasSelection));
}

void BCodeEditPrivate::updatePasteAvailable(bool available)
{
    bool b = !ptedt->isReadOnly() && pasteAvailable;
    pasteAvailable = available;
    if (b != pasteAvailable) {
        QMetaObject::invokeMethod(q_func(), "pasteAvailableChanged",
                                  Q_ARG(bool, !ptedt->isReadOnly() && pasteAvailable));
    }
}

void BCodeEditPrivate::updateRedoAvailable(bool available)
{
    bool b = !ptedt->isReadOnly() && redoAvailable;
    redoAvailable = available;
    if (b != redoAvailable) {
        QMetaObject::invokeMethod(q_func(), "redoAvailableChanged",
                                  Q_ARG(bool, !ptedt->isReadOnly() && redoAvailable));
    }
}

void BCodeEditPrivate::updateUndoAvailable(bool available)
{
    bool b = !ptedt->isReadOnly() && undoAvailable;
    undoAvailable = available;
    if (b != undoAvailable) {
        QMetaObject::invokeMethod(q_func(), "undoAvailableChanged",
                                  Q_ARG(bool, !ptedt->isReadOnly() && undoAvailable));
    }
}

/*============================================================================
================================ BCodeEdit ===================================
============================================================================*/

/*============================== Public constructors =======================*/

BCodeEdit::BCodeEdit(QWidget *parent) :
    QWidget(parent), BBaseObject(*new BCodeEditPrivate(this))
{
    d_func()->init();
}

BCodeEdit::~BCodeEdit()
{
    //
}

/*============================== Protected constructors ====================*/

BCodeEdit::BCodeEdit(BCodeEditPrivate &d, QWidget *parent) :
    QWidget(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BCodeEdit::clearUndoRedoStacks(QTextDocument::Stacks historyToClear)
{
    d_func()->ptedt->document()->clearUndoRedoStacks(historyToClear);
}

QMenu *BCodeEdit::createContextMenu()
{
    QMenu *menu = new QMenu;
    QAction *act = new QAction(menu);
      act->setText(tr("Undo", "act text"));
      act->setShortcut(QKeySequence::Undo);
      act->setEnabled(isUndoAvailable());
      connect(act, SIGNAL(triggered()), this, SLOT(undo()));
    menu->addAction(act);
    act = new QAction(menu);
      act->setText(tr("Redo", "act text"));
      act->setShortcut(QKeySequence::Redo);
      act->setEnabled(isRedoAvailable());
      connect(act, SIGNAL(triggered()), this, SLOT(redo()));
    menu->addAction(act);
    menu->addSeparator();
    act = new QAction(menu);
      act->setText(tr("Cut", "act text"));
      act->setShortcut(QKeySequence::Cut);
      act->setEnabled(isCutAvailable());
      connect(act, SIGNAL(triggered()), this, SLOT(cut()));
    menu->addAction(act);
    act = new QAction(menu);
      act->setText(tr("Copy", "act text"));
      act->setShortcut(QKeySequence::Copy);
      act->setEnabled(isCopyAvailable());
      connect(act, SIGNAL(triggered()), this, SLOT(copy()));
    menu->addAction(act);
    act = new QAction(menu);
      act->setText(tr("Paste", "act text"));
      act->setShortcut(QKeySequence::Paste);
      act->setEnabled(isPasteAvailable());
      connect(act, SIGNAL(triggered()), this, SLOT(paste()));
    menu->addAction(act);
    act = new QAction(menu);
      act->setText(tr("Delete", "act text"));
      act->setShortcut(QKeySequence::Delete);
      act->setEnabled(hasSelection() && !isReadOnly());
      connect(act, SIGNAL(triggered()), this, SLOT(deleteSelection()));
    menu->addAction(act);
    menu->addSeparator();
    act = new QAction(menu);
      act->setText(tr("Select all", "act text"));
      act->setShortcut(QKeySequence::SelectAll);
      connect(act, SIGNAL(triggered()), this, SLOT(selectAll()));
    menu->addAction(act);
    return menu;
}

int BCodeEdit::cursorPosition() const
{
    return d_func()->cursorPosition;
}

int BCodeEdit::cursorPositionForRowColumn(const QPoint &pos) const
{
    if (pos.x() < 0 || pos.y() < 0)
        return -1;
    const B_D(BCodeEdit);
    QTextBlock tb = d->ptedt->document()->findBlockByLineNumber(pos.y());
    if (!tb.isValid())
        return -1;
    if (pos.x() >= tb.length())
        return -1;
    return tb.position() + pos.x();
}

QPoint BCodeEdit::cursorPositionRowColumn() const
{
    return d_func()->cursorPositionRowColumn;
}

QFont BCodeEdit::editFont() const
{
    return d_func()->ptedt->font();
}

int BCodeEdit::editLineLength() const
{
    return d_func()->lineLength;
}

BCodeEdit::EditMode BCodeEdit::editMode() const
{
    return d_func()->blockMode ? BlockMode : NormalMode;
}

BeQt::TabWidth BCodeEdit::editTabWidth() const
{
    return d_func()->tabWidth;
}

BCodeEdit::ExtraSelectionList BCodeEdit::extraSelections() const
{
    return d_func()->ptedt->extraSelections();
}

bool BCodeEdit::findNext(const QString &txt, QTextDocument::FindFlags flags, bool cyclic)
{
    if (txt.isEmpty())
        return false;
    B_D(BCodeEdit);
    bool b = d->ptedt->find(txt, flags);
    if (!b && cyclic) {
        if (flags & QTextDocument::FindBackward)
            d->ptedt->moveCursor(QTextCursor::End);
        else
            d->ptedt->moveCursor(QTextCursor::Start);
        b = d->ptedt->find(txt, flags);
    }
    return b;
}

bool BCodeEdit::findNextRegexp(const QRegExp &rx, QTextDocument::FindFlags flags, bool cyclic)
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
    if (b) {
        d_func()->ptedt->blockSignals(true);
        d_func()->ptedt->setTextCursor(tc);
        d_func()->ptedt->blockSignals(false);
    }
    return b;
}

bool BCodeEdit::hasSelection() const
{
    return d_func()->hasSelection;
}

QTextDocument *BCodeEdit::innerDocument() const
{
    return d_func()->ptedt->document();
}

BPlainTextEdit *BCodeEdit::innerEdit() const
{
    return d_func()->ptedt;
}

bool BCodeEdit::isBuisy() const
{
    return d_func()->buisy;
}

bool BCodeEdit::isCopyAvailable() const
{
    return d_func()->copyAvailable;
}

bool BCodeEdit::isCutAvailable() const
{
    const B_D(BCodeEdit);
    return !d->ptedt->isReadOnly() && d->copyAvailable;
}

bool BCodeEdit::isEditAutoIndentationEnabled() const
{
    return d_func()->autoIndentation;
}

bool BCodeEdit::isLineNumberWidgetVisible() const
{
    return d_func()->lnwgt->isVisible();
}

bool BCodeEdit::isModified() const
{
    return d_func()->ptedt->document()->isModified();
}

bool BCodeEdit::isPasteAvailable() const
{
    const B_D(BCodeEdit);
    return !d->ptedt->isReadOnly() && d->pasteAvailable;
}

bool BCodeEdit::isReadOnly() const
{
    return d_func()->ptedt->isReadOnly();
}

bool BCodeEdit::isRedoAvailable() const
{
    const B_D(BCodeEdit);
    return !d->ptedt->isReadOnly() && d->redoAvailable;
}

bool BCodeEdit::isUndoAvailable() const
{
    const B_D(BCodeEdit);
    return !d->ptedt->isReadOnly() && d->undoAvailable;
}

int BCodeEdit::replaceInDocument(const QString &txt, const QString &newText, QTextDocument::FindFlags flags)
{
    B_D(BCodeEdit);
    if (isReadOnly() || txt.isEmpty() || txt == newText || txt.length() > d->lineLength)
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
    EditMode m = editMode();
    setEditMode(NormalMode);
    selectText(find, lind);
    QString ntext = ptext.mid(find, lind - find);
    BTextTools::replace(ntext, txt, newText, cs, w);
    insertText(ntext);
    setEditMode(m);
    return count;
}

int BCodeEdit::replaceInDocumentRegexp(const QRegExp &rx, const QString &newText)
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
    EditMode m = editMode();
    setEditMode(NormalMode);
    selectText(find, lind);
    QString ntext = ptext.mid(find, lind - find);
    ntext.replace(rx, newText);
    insertText(ntext);
    setEditMode(m);
    return count;
}

int BCodeEdit::replaceInSelection(const QString &txt, const QString &newText, QTextDocument::FindFlags flags)
{
    B_D(BCodeEdit);
    if (isReadOnly() || !hasSelection() || txt.isEmpty() || txt == newText || txt.length() > d->lineLength)
        return -1;
    Qt::CaseSensitivity cs = (QTextDocument::FindCaseSensitively & flags) ? Qt::CaseSensitive : Qt::CaseInsensitive;
    bool w = (QTextDocument::FindWholeWords & flags);
    QString ntext = selectedText(true);
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
    if (d->blockMode) {
        BPlainTextEditExtended::SelectionRange r = d->ptedt->selectionRanges().first();
        int plen = r.end - r.start;
        int min = r.start;
        int bpos = d->ptedt->document()->findBlock(min).position();
        int offset = min - bpos;
        BTextTools::replace(ntext, txt, newText, cs, w);
        int dlen = 0;
        BCodeEditPrivate::makeBlock(&ntext, &dlen);
        dlen -= plen;
        insertText(ntext);
        if (start < end) {
            if (start > bpos + offset)
                start += dlen;
            else
                end += dlen;
        } else {
            if (end > bpos + offset)
                end += dlen;
            else
                start += dlen;
        }
    }
    else {
        int len = ntext.length();
        BTextTools::replace(ntext, txt, newText, cs, w);
        insertText(ntext);
        if (start < end)
            end += (ntext.length() - len);
        else
            start += (ntext.length() - len);
    }
    selectText(start, end);
    tc.endEditBlock();
    QTextBlock tb = d->ptedt->document()->findBlock(qMin<int>(start, end));
    QTextBlock tbe = d->ptedt->document()->findBlock(qMax<int>(start, end));
    while (tb.isValid() && tb.blockNumber() < tbe.blockNumber()) {
        d->requestRehighlightBlock(tb);
        tb = tb.next();
    }
    return count;
}

int BCodeEdit::replaceInSelectionRegexp(const QRegExp &rx, const QString &newText)
{
    B_D(BCodeEdit);
    if (isReadOnly() || !hasSelection() || rx.isEmpty())
        return -1;
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
    if (d->blockMode) {
        BPlainTextEditExtended::SelectionRange r = d->ptedt->selectionRanges().first();
        int plen = r.end - r.start;
        int min = r.start;
        int bpos = d->ptedt->document()->findBlock(min).position();
        int offset = min - bpos;
        ntext.replace(rx, newText);
        int dlen = 0;
        BCodeEditPrivate::makeBlock(&ntext, &dlen);
        dlen -= plen;
        insertText(ntext);
        if (start < end) {
            if (start > bpos + offset)
                start += dlen;
            else
                end += dlen;
        } else {
            if (end > bpos + offset)
                end += dlen;
            else
                start += dlen;
        }
    } else {
        int len = ntext.length();
        ntext.replace(rx, newText);
        insertText(ntext);
        if (start < end)
            end += (ntext.length() - len);
        else
            start += (ntext.length() - len);
    }
    selectText(start, end);
    tc.endEditBlock();
    QTextBlock tb = d->ptedt->document()->findBlock(qMin<int>(start, end));
    QTextBlock tbe = d->ptedt->document()->findBlock(qMax<int>(start, end));
    while (tb.isValid() && tb.blockNumber() < tbe.blockNumber()) {
        d->requestRehighlightBlock(tb);
        tb = tb.next();
    }
    return count;
}

bool BCodeEdit::replaceNext(const QString &newText)
{
    if (isReadOnly() || !hasSelection())
        return false;
    insertText(newText);
    return true;
}

QString BCodeEdit::selectedText(bool full) const
{
    const B_D(BCodeEdit);
    QTextCursor tc = d->ptedt->textCursor();
    if (!tc.hasSelection())
        return "";
    if (d->blockMode) {
        QStringList lines;
        foreach (const BPlainTextEditExtended::SelectionRange &range, d->ptedt->selectionRanges()) {
            QTextBlock tb = d->ptedt->document()->findBlock(range.start);
            int offset = tb.position();
            lines << tb.text().mid(range.start - offset, range.end - range.start);
        }
        return lines.join("\n");
    } else {
        QString txt = tc.selectedText().replace(QChar::ParagraphSeparator, '\n');
        if (full)
            return txt;
        QString txtx = BTextTools::removeTrailingSpaces(txt);
        return !txtx.isEmpty() ? txtx : txt;
    }
}

int BCodeEdit::selectionEnd() const
{
    const B_D(BCodeEdit);
    if (!hasSelection())
        return cursorPosition();
    QTextCursor tc = d->ptedt->textCursor();
    return tc.selectionEnd();
}

QPoint BCodeEdit::selectionEndRowColumn() const
{
    const B_D(BCodeEdit);
    if (!hasSelection())
        return cursorPositionRowColumn();
    QTextCursor tc = d->ptedt->textCursor();
    QTextBlock tb = d->ptedt->document()->findBlock(tc.selectionEnd());
    return QPoint(tc.selectionEnd() - tb.position(), tb.blockNumber());
}

int BCodeEdit::selectionStart() const
{
    const B_D(BCodeEdit);
    if (!hasSelection())
        return cursorPosition();
    QTextCursor tc = d->ptedt->textCursor();
    return tc.selectionStart();
}

QPoint BCodeEdit::selectionStartRowColumn() const
{
    const B_D(BCodeEdit);
    if (!hasSelection())
        return cursorPositionRowColumn();
    QTextCursor tc = d->ptedt->textCursor();
    QTextBlock tb = d->ptedt->document()->findBlock(tc.selectionStart());
    return QPoint(tc.selectionStart() - tb.position(), tb.blockNumber());
}

void BCodeEdit::setEditAutoIndentationEnabled(bool enabled)
{
    d_func()->autoIndentation = enabled;
}

void BCodeEdit::setEditFont(const QFont &fnt)
{
    if (!QFontInfo(fnt).fixedPitch())
        return;
    if (fnt.pointSize() < 1 && fnt.pixelSize() < 1)
        return;
    B_D(BCodeEdit);
    QFont fntx = d->ptedt->font();
    //Workaround to set font correctly (assigning fnt directly doesn't work)
    fntx.setFamily(fnt.family());
    fntx.setPointSize(fnt.pointSize());
    d->ptedt->setFont(fntx);
}

void BCodeEdit::setEditLineLength(int ll)
{
    B_D(BCodeEdit);
    if (ll < 1 || ll == d->lineLength)
        return;
    if (d->buisy) {
        d->tmpLineLength = ll;
        return;
    }
    d->lineLength = ll;
    if (d->onceEdited) {
        QString text = d->ptedt->toPlainText();
        bool pm = d->ptedt->document()->isModified();
        d->setText(text, -1);
        d->ptedt->document()->setModified(pm || d->ptedt->document()->isModified());
        setFocus();
    } else {
        d->setTextToEmptyLine();
    }
}

void BCodeEdit::setEditMode(EditMode mode)
{
    B_D(BCodeEdit);
    bool b = (BlockMode == mode);
    if (b == d->blockMode)
        return;
    d->blockMode = b;
    d->ptedt->setBlockMode(b);
    Q_EMIT editModeChanged(mode);
}

void BCodeEdit::setEditTabWidth(BeQt::TabWidth tw)
{
    B_D(BCodeEdit);
    if (tw == d->tabWidth)
        return;
    d->tabWidth = tw;
}

void BCodeEdit::setExtraSelections(const ExtraSelectionList &list)
{
    ExtraSelectionList nlist = list;
    QTextCursor tc = d_func()->ptedt->textCursor();
    for (int i = 0; i < nlist.size(); ++i) {
        QTextCursor &ntc = nlist[i].cursor;
        int start = ntc.selectionStart();
        int end = ntc.selectionEnd();
        ntc = tc;
        ntc.setPosition(start);
        ntc.setPosition(end, QTextCursor::KeepAnchor);
    }
    d_func()->ptedt->setExtraSelections(nlist);
}

void BCodeEdit::setLineNumberWidgetVisible(bool b)
{
    d_func()->lnwgt->setVisible(b);
}

void BCodeEdit::setModification(bool modified)
{
    d_func()->ptedt->document()->setModified(modified);
}

void BCodeEdit::setReadOnly(bool ro)
{
    B_D(BCodeEdit);
    if (d->ptedt->isReadOnly() == ro)
        return;
    d->ptedt->setReadOnly(ro);
    Q_EMIT readOnlyChanged(ro);
    d->updateCopyAvailable(d->copyAvailable);
    d->updatePasteAvailable(d->pasteAvailable);
    d->updateUndoAvailable(d->undoAvailable);
    d->updateRedoAvailable(d->redoAvailable);
}

QString BCodeEdit::text(bool full) const
{
    const B_D(BCodeEdit);
    QString text = d->ptedt->toPlainText().replace(QChar::ParagraphSeparator, '\n');
    if (!full)
        BTextTools::removeTrailingSpaces(&text);
    return text;
}

/*============================== Public slots ==============================*/

void BCodeEdit::activateWindow()
{
    d_func()->ptedt->activateWindow();
}

void BCodeEdit::clear()
{
    setText("");
}

void BCodeEdit::copy()
{
    QString text = selectedText();
    if (text.isEmpty())
        return;
    QApplication::clipboard()->setText(text);
    setFocus();
}

void BCodeEdit::cut()
{
    if (isReadOnly() || !hasSelection())
        return;
    copy();
    d_func()->deleteSelection();
    setFocus();
}

void BCodeEdit::deleteSelection()
{
    if (isReadOnly() || !hasSelection())
        return;
    d_func()->deleteSelection();
    setFocus();
}

void BCodeEdit::deselectText()
{
    B_D(BCodeEdit);
    QTextCursor tc = d->ptedt->textCursor();
    if (!tc.hasSelection())
        return;
    tc.setPosition(tc.selectionEnd());
    d->ptedt->setTextCursor(tc);
    d->ptedt->centerCursor();
    setFocus();
}

void BCodeEdit::insertText(const QString &txt)
{
    d_func()->insertText(txt);
}

void BCodeEdit::moveCursor(int pos)
{
    B_D(BCodeEdit);
    QTextBlock tb = d->ptedt->document()->findBlock(pos);
    if (!tb.isValid())
        return;
    QTextCursor tc = d->ptedt->textCursor();
    tc.setPosition(pos);
    d->ptedt->setTextCursor(tc);
    d->ptedt->centerCursor();
    setFocus();
}

void BCodeEdit::moveCursor(const QPoint &pos)
{
    B_D(BCodeEdit);
    QTextBlock tb = d->ptedt->document()->findBlockByLineNumber(pos.y());
    if (!tb.isValid())
        return;
    QTextCursor tc = d->ptedt->textCursor();
    tc.setPosition(tb.position() + (pos.x() > 0 ? pos.x() : 0));
    d->ptedt->setTextCursor(tc);
    d->ptedt->centerCursor();
    setFocus();
}

void BCodeEdit::paste()
{
    if (isReadOnly())
        return;
    QString text = QApplication::clipboard()->text();
    if (text.isEmpty())
        return;
    insertText(text);
}

void BCodeEdit::redo()
{
    if (isReadOnly() || !isRedoAvailable())
        return;
    B_D(BCodeEdit);
    d->ptedt->redo();
    QTextCursor tc = d->ptedt->textCursor();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    int i = text.length() - 1;
    while (i >= 0 && text.at(i) == ' ')
        --i;
    if (tc.positionInBlock() <= i)
        return;
    tc.setPosition(tb.position() + i + 1);
    d->ptedt->setTextCursor(tc);
    setFocus();
}

void BCodeEdit::selectAll()
{
    d_func()->ptedt->selectAll();
    setFocus();
}

void BCodeEdit::selectLines(int firstLine, int lastLine)
{
    B_D(BCodeEdit);
    int bc = d_func()->ptedt->blockCount();
    if (firstLine < 0 || firstLine >= bc)
        return;
    QPoint start(0, firstLine);
    QPoint end(d->lineLength, firstLine);
    if (lastLine > 0 && lastLine < bc)
        end.setY(lastLine);
    selectText(start, end);
}

void BCodeEdit::selectText(const QPoint &start, const QPoint &end)
{
    B_D(BCodeEdit);
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
    d->ptedt->centerCursor();
    setFocus();
}

void BCodeEdit::selectText(int start, int end)
{
    if (start < 0 || end < 0)
        return;
    QTextCursor tc = d_func()->ptedt->textCursor();
    tc.setPosition(start);
    tc.setPosition(end, QTextCursor::KeepAnchor);
    d_func()->ptedt->setTextCursor(tc);
    d_func()->ptedt->centerCursor();
    setFocus();
}

void BCodeEdit::setFocus()
{
    d_func()->ptedt->setFocus();
}

void BCodeEdit::setText(const QString &txt, int asyncIfLongerThan)
{
    if (isBuisy())
        return;
    d_func()->setText(txt, asyncIfLongerThan);
    setFocus();
}

void BCodeEdit::switchMode()
{
    setEditMode(editMode() == NormalMode ? BlockMode : NormalMode);
}

void BCodeEdit::undo()
{
    if (isReadOnly() || !isUndoAvailable())
        return;
    B_D(BCodeEdit);
    d->ptedt->undo();
    QTextCursor tc = d->ptedt->textCursor();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    int i = text.length() - 1;
    while (i >= 0 && text.at(i) == ' ')
        --i;
    if (tc.positionInBlock() <= i)
        return;
    tc.setPosition(tb.position() + i + 1);
    d->ptedt->setTextCursor(tc);
    setFocus();
}
