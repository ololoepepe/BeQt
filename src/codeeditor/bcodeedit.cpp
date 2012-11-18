#include "bcodeedit.h"
#include "bcodeedit_p.h"
#include "babstractfiletype.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BApplication>
#include <BeQtWidgets/BPlainTextEdit>

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QEvent>
#include <QString>
#include <QTextCursor>
#include <QTextBlock>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QRegExp>
#include <QStringList>
#include <QVector>
#include <QChar>
#include <QFutureWatcher>
#include <QFuture>
#include <QMetaObject>
#include <QSyntaxHighlighter>
#include <QPoint>
#include <QMenu>
#include <QAction>
#include <QFont>
#include <QFontInfo>
#include <QtConcurrentRun>
#include <QApplication>
#include <QClipboard>
#include <QTextEdit>
#include <QMap>
#include <QTextCharFormat>
#include <QBrush>
#include <QColor>

/*============================================================================
================================ Code Edit Clipboard Notifier
============================================================================*/

BCodeEditClipboardNotifier *BCodeEditClipboardNotifier::instance()
{
    return _m_self;
}

//

BCodeEditClipboardNotifier::BCodeEditClipboardNotifier() :
    QObject(0)
{
    dataChanged();
    connect( QApplication::clipboard(), SIGNAL( dataChanged() ), this, SLOT( dataChanged() ) );
    bTest(!_m_self, "BCodeEditClipboardNotifier", "There should be only one instance of BCodeEditClipboardNotifier");
    _m_self = this;
}

BCodeEditClipboardNotifier::~BCodeEditClipboardNotifier()
{
    _m_self = 0;
}

//

bool BCodeEditClipboardNotifier::clipboardDataAvailable() const
{
    return dataAvailable;
}

//

void BCodeEditClipboardNotifier::dataChanged()
{
    bool b = !QApplication::clipboard()->text().isEmpty();
    bool bb = (b != dataAvailable);
    dataAvailable = b;
    if (bb)
        emit clipboardDataAvailableChanged(b);
}

//

BCodeEditClipboardNotifier *BCodeEditClipboardNotifier::_m_self = 0;

//

/*============================================================================
================================ Code Edit Private Object
============================================================================*/

BCodeEditPrivateObject::BCodeEditPrivateObject(BCodeEditPrivate *p) :
    BBasePrivateObject(p)
{
    //
}

BCodeEditPrivateObject::~BCodeEditPrivateObject()
{
    //
}

//

bool BCodeEditPrivateObject::eventFilter(QObject *obj, QEvent *e)
{
    switch ( e->type() )
    {
    case QEvent::KeyPress:
        return p_func()->keyPressEvent( static_cast<QKeyEvent *>(e) );
    case QEvent::MouseButtonDblClick:
        return p_func()->mouseDoubleClickEvent( static_cast<QMouseEvent *>(e) );
    case QEvent::MouseButtonPress:
        return p_func()->mousePressEvent( static_cast<QMouseEvent *>(e) );
    default:
        return QObject::eventFilter(obj, e);
    }
}

//

void BCodeEditPrivateObject::futureWatcherFinished()
{
    p_func()->futureWatcherFinished( static_cast<BCodeEditPrivate::ProcessTextFutureWatcher *>( sender() ) );
}

void BCodeEditPrivateObject::customContextMenuRequested(const QPoint &pos)
{
    p_func()->popupMenu(pos);
}

void BCodeEditPrivateObject::cursorPositionChanged()
{
    p_func()->updateCursorPosition();
}

void BCodeEditPrivateObject::clipboardDataAvailableChanged(bool available)
{
    p_func()->updatePasteAvailable(available);
}

void BCodeEditPrivateObject::selectionChanged()
{
    p_func()->updateHasSelection();
}

void BCodeEditPrivateObject::copyAvailableChanged(bool available)
{
    p_func()->updateCopyAvailable(available);
}

void BCodeEditPrivateObject::undoAvailableChanged(bool available)
{
    p_func()->updateUndoAvailable(available);
}

void BCodeEditPrivateObject::redoAvailableChanged(bool available)
{
    p_func()->updateRedoAvailable(available);
}

/*============================================================================
================================ Code Edit Private
============================================================================*/

QStringList BCodeEditPrivate::processLine(const QString &line, int ll, BCodeEdit::TabWidth tw)
{
    QStringList sl;
    QString xline = replaceTabs(line, tw);
    int len = xline.length();
    if (len > ll)
    {
        while (xline.length() > ll)
        {
            int k = ll;
            while (k > 1 && xline.at(k) != ' ')
                --k;
            int l = k > 1 ? k : ll;
            QString nline = xline.left(l);;
            appendTrailingSpaces(nline, ll);
            sl << nline;
            xline.remove( 0, l + (k > 1 ? 1 : 0) );
        }
        if ( !xline.isEmpty() )
        {
            appendTrailingSpaces(xline, ll);
            sl << xline;
        }
    }
    else
    {
        appendTrailingSpaces(xline, ll);
        sl << xline;
    }
    return sl;
}

BCodeEditPrivate::ProcessTextResult BCodeEditPrivate::processText(const QString &text, int ll, BCodeEdit::TabWidth tw)
{
    ProcessTextResult res;
    //TODO
    QStringList sl = text.split('\n');
    QStringList sln;
    for (int i = 0; i < sl.size(); ++i)
    {
        QStringList lines = processLine(sl.at(i), ll, tw);
        sln << lines;
        if (lines.size() > 1)
        {
            BCodeEdit::SplittedLinesRange r;
            r.firstLineNumber = sln.size();
            r.lastLineNumber = r.firstLineNumber + lines.size();
            res.splittedLinesRanges << r;
        }
    }
    res.newText = sln.join("\n");
    return res;
}

QString BCodeEditPrivate::removeUnsupportedSymbols(const QString &s)
{
    QString ns = s;
    removeUnsupportedSymbols(ns);
    return ns;
}

void BCodeEditPrivate::removeUnsupportedSymbols(QString &text)
{
    text.remove('\r');
    for (int i = 0; i < unsupportedSymbols.size(); ++i)
        text.remove( unsupportedSymbols.at(i) );
}

QString BCodeEditPrivate::removeTrailingSpaces(const QString &s)
{
    QString ns = s;
    removeTrailingSpaces(ns);
    return ns;
}

void BCodeEditPrivate::removeTrailingSpaces(QString &s)
{
    //TODO: Use the commented variant with Qt5' improved RegExp
    //static QRegExp rx("\\s+(?=\\n|$)");
    static QRegExp rx("\\s+$");
    QStringList sl = s.split('\n');
    for (int i = 0; i < sl.size(); ++i)
        sl[i].remove(rx);
    s = sl.join("\n");
    //text.remove(rx);
}

QString BCodeEditPrivate::appendTrailingSpaces(const QString &s, int ll)
{
    QString ns = s;
    appendTrailingSpaces(ns, ll);
    return ns;
}

void BCodeEditPrivate::appendTrailingSpaces(QString &s, int ll)
{
    int len = s.length();
    if (len < ll)
        s.append( QString().fill(' ', ll - len) );
}

QString BCodeEditPrivate::replaceTabs(const QString &s, BCodeEdit::TabWidth tw)
{
    QString ns = s;
    replaceTabs(ns, tw);
    return ns;
}

void BCodeEditPrivate::replaceTabs(QString &s, BCodeEdit::TabWidth tw)
{
    int i = 0;
    while ( i < s.length() )
    {
        if (s.at(i) == '\t')
        {
            int spcount = tw - (i < tw ? i : i % tw);
            s.replace( i, 1, QString().fill(' ', spcount ? spcount : tw) );
            i += spcount;
        }
        else
        {
            ++i;
        }
    }
}

void BCodeEditPrivate::removeExtraSelections(QList<QTextEdit::ExtraSelection> &from,
                                             const QList<QTextEdit::ExtraSelection> &what)
{
    for (int i = from.size() - 1; i >= 0; --i)
        foreach (const QTextEdit::ExtraSelection &es, what)
            if (from.at(i).cursor == es.cursor && from.at(i).format == es.format)
                from.removeAt(i);
}

BCodeEditPrivate::TestBracketResult BCodeEditPrivate::testBracket(const QString &txt, int pos,
                                                                  const QList<BAbstractFileType::BracketPair> &brlist)
{
    if ( txt.isEmpty() || pos < 0 || pos >= txt.length() || brlist.isEmpty() )
        return TestBracketResult();
    foreach (const BAbstractFileType::BracketPair &br, brlist)
    {
        if ( br.opening.isEmpty() || br.closing.isEmpty() )
            continue;
        if (pos >= br.escape.length() && txt.mid( pos - br.escape.length(), br.escape.length() ) == br.escape)
            continue;
        if (txt.mid( pos, br.opening.length() ) == br.opening)
        {
            TestBracketResult r;
            r.valid = true;
            r.bracket = br;
            r.opening = true;
            return r;
        }
        if (txt.mid( pos, br.closing.length() ) == br.closing)
        {
            TestBracketResult r;
            r.valid = true;
            r.bracket = br;
            r.opening = false;
            return r;
        }
    }
    return TestBracketResult();
}

BCodeEditPrivate::FindBracketResult BCodeEditPrivate::findBracketPair(
        const BAbstractFileType::BracketPair &br, bool opening, const QTextBlock &tb, int pos,
        const QList<BAbstractFileType::BracketPair> &brlist)
{
    if ( br.opening.isEmpty() || br.closing.isEmpty() || !tb.isValid() || brlist.isEmpty() )
        return FindBracketResult();
    QTextBlock tbx = tb;
    bool b = true;
    int depth = 0;
    FindBracketResult ret;
    while ( tbx.isValid() )
    {
        QString txt = removeTrailingSpaces( tbx.text() );
        int lb = opening ? (b ? pos : 0) : txt.length() - 1;
        int ub = opening ? (b ? pos : txt.length() ) : -1;
        int m = opening ? 1 : -1;
        for (int i = lb; i != ub; i += m)
        {
            TestBracketResult r = testBracket(txt, i, brlist);
            if (!r.valid)
                continue;
            if (r.opening != opening && !depth--)
            {
                ret.valid = true;
                ret.pos = tbx.position() + i;
                ret.bracket = r.bracket;
                return ret;
            }
            else
            {
                ++depth;
            }
        }
        b = false;
        tbx = opening ? tbx.next() : tbx.previous();
    }
    return ret;
}

//

BCodeEditPrivate::BCodeEditPrivate(BCodeEdit *q) :
    BBasePrivate( *q, *new BCodeEditPrivateObject(this) )
{
    if ( !BCodeEditClipboardNotifier::instance() )
        new BCodeEditClipboardNotifier;
    QObject::connect( BCodeEditClipboardNotifier::instance(), SIGNAL( clipboardDataAvailableChanged(bool) ),
                      o_func(), SLOT( clipboardDataAvailableChanged(bool) ) );
    blockMode = false;
    lineLength = 120;
    tabWidth = BCodeEdit::TabWidth4;
    hasSelection = false;
    hasBookmarks = false;
    copyAvailable = false;
    pasteAvailable = BCodeEditClipboardNotifier::instance()->clipboardDataAvailable();
    undoAvailable = false;
    redoAvailable = false;
    maxBookmarks = 4;
    fileType = BAbstractFileType::defaultFileType();
    brackets = fileType->brackets();
    bracketsHighlighting = true;
    highlighter = fileType->createHighlighter();
    //
    vlt = new QVBoxLayout(q);
      vlt->setContentsMargins(0, 0, 0, 0);
      ptedt = new BPlainTextEdit(q);
        ptedt->installEventFilter( o_func() );
        ptedt->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect( ptedt, SIGNAL( customContextMenuRequested(QPoint) ),
                          o_func(), SLOT( customContextMenuRequested(QPoint) ) );
        QObject::connect( ptedt, SIGNAL( cursorPositionChanged() ), o_func(), SLOT( cursorPositionChanged() ) );
        QObject::connect( ptedt->document(), SIGNAL( modificationChanged(bool) ),
                          q, SIGNAL( modificationChanged(bool) ) );
        QObject::connect( ptedt, SIGNAL( selectionChanged() ), o_func(), SLOT( selectionChanged() ) );
        QObject::connect( ptedt, SIGNAL( copyAvailable(bool) ), o_func(), SLOT( copyAvailableChanged(bool) ) );
        QObject::connect( ptedt, SIGNAL( undoAvailable(bool) ), o_func(), SLOT( undoAvailableChanged(bool) ) );
        QObject::connect( ptedt, SIGNAL( redoAvailable(bool) ), o_func(), SLOT( redoAvailableChanged(bool) ) );
        //
      vlt->addWidget(ptedt);
    //
    setTextToEmptyLine();
}

BCodeEditPrivate::~BCodeEditPrivate()
{
    //
}

//

bool BCodeEditPrivate::keyPressEvent(QKeyEvent *e)
{
    /*if (_m_recorder) //Install external event filter
        _m_recorder->handleKeyPress(event);*/
    static const int ContorlShiftModifier = ( (int) Qt::ControlModifier | (int) Qt::ShiftModifier );
    static const int KeypadControlModifier = ( (int) Qt::KeypadModifier | (int) Qt::ControlModifier);
    int modifiers = e->modifiers();
    int key = e->key();
    switch (modifiers)
    {
    case Qt::NoModifier:
        switch (key)
        {
        case Qt::Key_Return:
            q_func()->insertText("\n");
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
        switch (key)
        {
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
        //case Qt::Key_Tab: //Install external event filter
            //emit requestSwitchDocument();
            //return true;
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
        default:
            break;
        }
        break;
    case Qt::ShiftModifier:
        switch (key)
        {
        case Qt::Key_Backspace:
            handleBackspace();
            return true;
        case Qt::Key_Left:
            handleLeft(true);
            return true;
        case Qt::Key_End:
            handleShiftEnd();
            return true;
        //case Qt::Key_PageUp: //Not needed (or Shift press emulation needed)
        //case Qt::Key_PageDown:
            //QTimer::singleShot( 0, this, SLOT( _m_editSelectionChanged() ) );
            //return false;
        default:
            break;
        }
        break;
    case Qt::AltModifier:
        switch (key)
        {
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            move(key);
            return true;
        default:
            break;
        }
        break;
    case ContorlShiftModifier:
        switch (key)
        {
        case Qt::Key_Z:
            q_func()->redo();
            return true;
        default:
            break;
        }
        break;
    case Qt::KeypadModifier:
        switch (key)
        {
        case Qt::Key_Enter:
            q_func()->insertText("\n");
            return true;
        case Qt::Key_End:
            handleEnd();
            return true;
        default:
            break;
        }
        break;
    case KeypadControlModifier:
        switch (key)
        {
        case Qt::Key_End:
            handleEnd(true);
            return true;
        default:
            break;
        }
        break;
    default:
        break;
    }
    QString text = e->text();
    if ( !text.isEmpty() )
    {
        QChar c = text.at(0);
        if (c.isPrint() || c == '\t')
            q_func()->insertText( QString(c) );
        return true;
    }
    return false;
}

bool BCodeEditPrivate::mouseDoubleClickEvent(QMouseEvent *e)
{
    QTextCursor tc = ptedt->textCursor();
    QString text = tc.block().text();
    int posb = tc.positionInBlock();
    int lposb = posb - 1;
    return ( (lposb < 0 || text.at(lposb) == ' ') && text.at(posb) == ' ' );
}

bool BCodeEditPrivate::mousePressEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton)
        return false;
    QTextCursor tc = ptedt->cursorForPosition( e->pos() );
    if (tc.positionInBlock() != lineLength)
        return false;
    QTextBlock tb = tc.block();
    if ( tb.next().isValid() )
        return false;
    QString text = tb.text();
    removeTrailingSpaces(text);
    tc.setPosition( tb.position() + text.length() );
    ptedt->setTextCursor(tc);
    return true;
}

void BCodeEditPrivate::setTextToEmptyLine()
{
    ptedt->setPlainText( QString().fill(' ', lineLength) );
}

void BCodeEditPrivate::deleteSelection()
{
    QTextCursor tc = ptedt->textCursor();
    if ( !tc.hasSelection() )
        return;
    tc.beginEditBlock();
    int pos = tc.selectionStart();
    if (blockMode)
    {
        QVector<BPlainTextEdit::SelectionRange> ranges = ptedt->selectionRanges();
        foreach(const BPlainTextEdit::SelectionRange &range, ranges)
        {
            tc.setPosition(range.start);
            tc.setPosition(range.end, QTextCursor::KeepAnchor);
            tc.removeSelectedText();
            QString text = tc.block().text();
            appendTrailingSpaces(text, lineLength);
            tc.movePosition(QTextCursor::StartOfBlock);
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            tc.insertText(text);
        }
    }
    else
    {
        tc.removeSelectedText();
        QString text = tc.block().text();
        if (text.length() > lineLength) //TODO: Don't just truncate, split into lines
            text.truncate(lineLength);
        appendTrailingSpaces(text, lineLength);
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.insertText(text);
    }
    tc.setPosition(pos);
    ptedt->setTextCursor(tc);
    tc.endEditBlock();
}

void BCodeEditPrivate::seletAll()
{
    //TODO: Don't select last line's trailing spaces in normal mode; don't select any trailing spaces in block mode
    ptedt->selectAll();
    //_m_editSelectionChanged();
}

int BCodeEditPrivate::replaceInSelectionLines(const QString &text, const QString &newText, Qt::CaseSensitivity cs)
{
    if (blockMode)
        return -1;
    B_Q(BCodeEdit);
    if ( !q->hasSelection() )
        return -1;
    QString ntext = q->selectedText();
    int count = ntext.count(text, cs);
    if (!count)
        return 0;
    QTextCursor tc = ptedt->textCursor();
    tc.beginEditBlock();
    int start = tc.selectionStart();
    int end = tc.selectionEnd();
    removeTrailingSpaces(ntext);
    ntext.replace(text, newText, cs);
    q->insertText(ntext);
    tc = ptedt->textCursor();
    int pos = tc.position();
    tc.setPosition(end < start ? pos : start);
    tc.setPosition(end < start ? end : pos, QTextCursor::KeepAnchor);
    ptedt->setTextCursor(tc);
    //_m_editSelectionChanged();
    tc.endEditBlock();
    //Rehighlighting
    if (highlighter)
    {
        QTextBlock tb = ptedt->document()->findBlock( qMin<int>(start, end) );
        QTextBlock tbe = ptedt->document()->findBlock( qMax<int>(start, end) );
        while ( tb.isValid() && tb.blockNumber() < tbe.blockNumber() )
        {
            highlighter->rehighlightBlock(tb);
            tb = tb.next();
        }
    }
    return count;
}

int BCodeEditPrivate::replaceInSelectionBlocks(const QString &text, const QString &newText, Qt::CaseSensitivity cs)
{
    //TODO: Improve replacing
    if (!blockMode)
        return -1;
    B_Q(BCodeEdit);
    if ( !q->hasSelection() )
        return -1;
    int count = 0;
    int maxlen = 0;
    QStringList sl;
    QTextCursor tcr = ptedt->textCursor();
    QVector<BPlainTextEdit::SelectionRange> ranges = ptedt->selectionRanges();
    foreach (const BPlainTextEdit::SelectionRange &range, ranges)
    {
        tcr.setPosition(range.start);
        tcr.setPosition(range.end, QTextCursor::KeepAnchor);
        QString st = tcr.selectedText();
        int slen = st.length();
        count += st.count(text, cs);
        removeTrailingSpaces(st);
        st.replace(text, newText, cs);
        if (st.length() > maxlen)
            maxlen = st.length();
        QTextBlock tbr = tcr.block();
        QString txt = tbr.text();
        removeTrailingSpaces(txt);
        sl << txt.replace(tcr.selectionStart() - tbr.position(), slen, st);
        if (txt.length() > lineLength) //This must be fixed
            return -1;
    }
    QTextCursor tc = ptedt->textCursor();
    tc.beginEditBlock();
    QTextBlock tbf = ptedt->document()->findBlock(ranges.first().start);
    int offset = ranges.first().start - tbf.position();
    int bpos = tc.block().position();
    int dlen = maxlen - (ranges.first().end - ranges.first().start);
    int start = tc.selectionStart();
    int end = tc.selectionEnd();
    foreach (const QString &txt, sl)
    {
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.insertText(txt);
    }
    if (start < end)
    {
        if (start > bpos + offset)
            start += dlen;
        else
            end += dlen;
    }
    else
    {
        if (end > bpos + offset)
            end += dlen;
        else
            start += dlen;
    }
    tc.setPosition(start);
    tc.setPosition(end, QTextCursor::KeepAnchor);
    ptedt->setTextCursor(tc);
    //_m_editSelectionChanged();
    tc.endEditBlock();
    //Rehighlighting
    if (highlighter)
        foreach (const BPlainTextEdit::SelectionRange &range, ranges)
            highlighter->rehighlightBlock( ptedt->document()->findBlock(range.start) );
    return count;
}

void BCodeEditPrivate::highlightBrackets()
{
    QList<QTextEdit::ExtraSelection> list = ptedt->extraSelections();
    removeExtraSelections(list, highlightedBrackets);
    highlightedBrackets.clear();
    QTextCursor tc = ptedt->textCursor();
    QTextBlock tb = tc.block();
    TestBracketResult op = testBracket(removeTrailingSpaces( tb.text() ), tc.positionInBlock(), brackets);
    TestBracketResult cl = testBracket(removeTrailingSpaces( tb.text() ), tc.positionInBlock() - 1, brackets);
    if (op.valid && op.opening)
    {
        FindBracketResult r = findBracketPair(op.bracket, true, tb, tc.positionInBlock(), brackets);
        if (r.valid)
        {
            QTextEdit::ExtraSelection es;
            es.format.setBackground( QBrush( QColor("yellow") ) );
            es.cursor.setPosition( tb.position() + tc.positionInBlock() );
            es.cursor.setPosition(tb.position() + tc.positionInBlock() + r.bracket.opening.length(),
                                  QTextCursor::KeepAnchor);
            list << es;
            es.cursor.setPosition(r.pos);
            es.cursor.setPosition(r.pos + r.bracket.closing.length(), QTextCursor::KeepAnchor);
            list << es;
        }
        else
        {
            QTextEdit::ExtraSelection es;
            es.format.setBackground( QBrush( QColor("hotpink") ) );
            es.cursor.setPosition( tb.position() + tc.positionInBlock() );
            es.cursor.setPosition(ptedt->document()->lastBlock().position() + ptedt->document()->lastBlock().length(),
                                  QTextCursor::KeepAnchor);
            list << es;
        }
    }
    if (cl.valid && !cl.opening)
    {
        FindBracketResult r = findBracketPair(op.bracket, false, tb, tc.positionInBlock() - 1, brackets);
        if (r.valid)
        {
            QTextEdit::ExtraSelection es;
            es.format.setBackground( QBrush( QColor("yellow") ) );
            es.cursor.setPosition(tb.position() + tc.positionInBlock() - 1);
            es.cursor.setPosition(tb.position() + tc.positionInBlock() - 1 + r.bracket.opening.length(),
                                  QTextCursor::KeepAnchor);
            list << es;
            es.cursor.setPosition(r.pos);
            es.cursor.setPosition(r.pos + r.bracket.closing.length(), QTextCursor::KeepAnchor);
            list << es;
        }
        else
        {
            QTextEdit::ExtraSelection es;
            es.format.setBackground( QBrush( QColor("hotpink") ) );
            es.cursor.setPosition(tb.position() + tc.positionInBlock() - 1);
            es.cursor.setPosition(0, QTextCursor::KeepAnchor);
            list << es;
        }
    }
    ptedt->setExtraSelections(list);
}

void BCodeEditPrivate::emitLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> &ranges)
{
    if (ranges.size() > 1)
        QMetaObject::invokeMethod( q_func(), "linesSplitted",
                                   Q_ARG(const QList<BCodeEdit::SplittedLinesRange> &, ranges) );
    else if (ranges.size() == 1)
        QMetaObject::invokeMethod( q_func(), "lineSplitted",
                                   Q_ARG( const BCodeEdit::SplittedLinesRange &, ranges.first() ) );
}

void BCodeEditPrivate::handleBackspace()
{
    if ( ptedt->isReadOnly() )
        return;
    QTextCursor tc = ptedt->textCursor();
    if ( tc.hasSelection() )
        return deleteSelection();
    if (tc.position() == 0)
        return;
    tc.beginEditBlock();
    int posb = tc.positionInBlock();
    if (posb)
    {

        QString text = tc.block().text();
        int pos = tc.position() - 1;
        text.remove(posb - 1, 1);
        text.append(' ');
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.insertText(text);
        tc.setPosition(pos);
    }
    else
    {
        QTextBlock tb = tc.block();
        QTextBlock tbp = tb.previous();
        int pbpos = tbp.position();
        QString ptext = removeTrailingSpaces( tbp.text() );
        QString text = ptext + removeTrailingSpaces( tb.text() );
        tc.movePosition(QTextCursor::EndOfBlock);
        tc.movePosition(QTextCursor::PreviousBlock, QTextCursor::KeepAnchor);
        tc.removeSelectedText();
        q_func()->insertText(text);
        tc.setPosition( pbpos + ptext.length() );
    }
    ptedt->setTextCursor(tc);
    tc.endEditBlock();
}

void BCodeEditPrivate::handleCtrlBackspace()
{
    if ( ptedt->isReadOnly() )
        return;
    QTextCursor tc = ptedt->textCursor();
    if ( tc.hasSelection() )
        return deleteSelection();
    tc.beginEditBlock();
    QString text = tc.block().text();
    int posb = tc.positionInBlock();
    int i = posb;
    while (--i >= 0 && text.at(i) == ' ')
        ;
    while (--i >= 0 && text.at(i) != ' ')
        ;
    i += 1;
    text.remove(i, posb - i);
    appendTrailingSpaces(text, lineLength);
    tc.movePosition(QTextCursor::StartOfBlock);
    tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    tc.insertText(text);
    tc.setPosition(tc.block().position() + i);
    ptedt->setTextCursor(tc);
    tc.endEditBlock();
}

void BCodeEditPrivate::handleDelete()
{
    if ( ptedt->isReadOnly() )
        return;
    QTextCursor tc = ptedt->textCursor();
    if ( tc.hasSelection() )
        return deleteSelection();
    if (tc.position() == 0)
        return;
    int posb = tc.positionInBlock();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    int pos = tc.position();
    QString t = text;
    removeTrailingSpaces(t);
    bool b = posb < t.length();
    if ( !b && !tb.next().isValid() )
        return;
    tc.beginEditBlock();
    if (b)
    {
        text.remove(posb, 1);
        text.append(' ');
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.insertText(text);
        tc.setPosition(pos);
    }
    else
    {
        QTextBlock tb = tc.block();
        QTextBlock tbn = tb.next();
        int nbpos = tbn.position();
        QString ntext = removeTrailingSpaces( tbn.text() );
        QString text = removeTrailingSpaces( tb.text() ) + ntext;
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.removeSelectedText();
        q_func()->insertText(text);
        tc.setPosition( nbpos + ntext.length() );
    }
    ptedt->setTextCursor(tc);
    tc.endEditBlock();
}

void BCodeEditPrivate::handleCtrlDelete()
{
    if ( ptedt->isReadOnly() )
            return;
    QTextCursor tc = ptedt->textCursor();
    if ( tc.hasSelection() )
        return deleteSelection();
    tc.beginEditBlock();
    QString text = tc.block().text();
    int posb = tc.positionInBlock();
    int i = posb;
    while (++i < text.length() && text.at(i) == ' ')
        ;
    while (++i < text.length() && text.at(i) != ' ')
        ;
    text.remove(posb, i - posb);
    appendTrailingSpaces(text, lineLength);
    tc.movePosition(QTextCursor::StartOfBlock);
    tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    tc.insertText(text);
    tc.setPosition(tc.block().position() + posb);
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
    //_m_editCursorPositionChanged();
}

void BCodeEditPrivate::handleShiftEnd()
{
    QTextCursor tc = ptedt->textCursor();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    removeTrailingSpaces(text);
    int d = text.length() - tc.positionInBlock();
    if (d < 1)
        return;
    tc.setPosition(tc.position() + d, QTextCursor::KeepAnchor);
    ptedt->setTextCursor(tc);
    //_m_editCursorPositionChanged();
}

void BCodeEditPrivate::handleLeft(bool shift)
{
    QTextCursor tc = ptedt->textCursor();
    int pos = tc.position();
    if (!pos)
    {
        if (!shift)
        {
            tc.setPosition(0);
            ptedt->setTextCursor(tc);
            //_m_editSelectionChanged();
        }
        return;
    }
    QTextCursor::MoveMode mode = shift ?
                QTextCursor::KeepAnchor : QTextCursor::MoveAnchor;
    int npos;
    if ( tc.positionInBlock() )
    {
        npos = pos - 1;
    }
    else
    {
        QTextBlock tb = tc.block().previous();
        //check block validity
        QString text = tb.text();
        int i = text.length() - 1;
        while (i >= 0 && text.at(i) == ' ')
            --i;
        npos = tb.position() + i + 1;
    }
    tc.setPosition(npos, mode);
    ptedt->setTextCursor(tc);
    //_m_editSelectionChanged();
}

void BCodeEditPrivate::handleCtrlLeft()
{
    QTextCursor tc = ptedt->textCursor();
    int posb = tc.positionInBlock();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    int bpos = tb.position();
    removeTrailingSpaces(text);
    if (!posb)
    {
        QTextBlock tbp = tb.previous();
        if ( tbp.isValid() )
        {
            QString textp = tbp.text();
            removeTrailingSpaces(textp);
            tc.setPosition( tbp.position() + textp.length() );
            ptedt->setTextCursor(tc);
        }
        return;
    }
    if (text.length() < posb)
    {
        tc.setPosition( bpos + text.length() );
        ptedt->setTextCursor(tc);
        return;
    }
    if ( text.at(posb - 1).isLetterOrNumber() )
        while ( posb > 0 && text.at(posb - 1).isLetterOrNumber() )
            --posb;
    else if ( text.at(posb - 1).isSpace() )
        while ( posb > 0 && text.at(posb - 1).isSpace() )
            --posb;
    else if ( text.at(posb - 1).isSymbol() || text.at(posb - 1).isPunct() )
        while ( posb > 0 && ( text.at(posb - 1).isSymbol() || text.at(posb - 1).isPunct() ) )
            --posb;
    while ( posb > 0 && text.at(posb - 1).isSpace() )
        --posb;
    tc.setPosition(bpos + posb);
    ptedt->setTextCursor(tc);
}

void BCodeEditPrivate::handleCtrlRight()
{
    QTextCursor tc = ptedt->textCursor();
    int posb = tc.positionInBlock();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    int bpos = tb.position();
    removeTrailingSpaces(text);
    if (text.length() <= posb)
    {
        QTextBlock tbn = tb.next();
        if ( tbn.isValid() )
            tc.setPosition( tbn.position() );
        else
            tc.setPosition( bpos + text.length() );
        ptedt->setTextCursor(tc);
        return;
    }
    if ( text.at(posb).isLetterOrNumber() )
        while ( posb < text.length() && text.at(posb).isLetterOrNumber() )
            ++posb;
    else if ( text.at(posb).isSpace() )
        while ( posb < text.length() && text.at(posb).isSpace() )
            ++posb;
    else if ( text.at(posb).isSymbol() || text.at(posb).isPunct() )
        while ( posb < text.length() && ( text.at(posb).isSymbol() || text.at(posb).isPunct() ) )
            ++posb;
    while ( posb < text.length() && text.at(posb).isSpace() )
        ++posb;
    tc.setPosition(bpos + posb);
    ptedt->setTextCursor(tc);
}

void BCodeEditPrivate::move(int key)
{
    if ( ptedt->isReadOnly() )
        return;
    QTextCursor tc = ptedt->textCursor();
    if ( !tc.hasSelection() )
        return;
    tc.beginEditBlock();
    int start = tc.selectionStart();
    int end = tc.selectionEnd();
    if (blockMode)
    {
        QVector<BPlainTextEdit::SelectionRange> ranges = ptedt->selectionRanges();
        if (Qt::Key_Left == key || Qt::Key_Right == key)
        {
            QTextCursor tcr = tc;
            foreach (const BPlainTextEdit::SelectionRange &range, ranges)
            {
                tcr.setPosition(range.start);
                tcr.setPosition(range.end, QTextCursor::KeepAnchor);
                QString text = tcr.selectedText();
                if (Qt::Key_Left == key)
                    text.remove(0, 1).append(' ');
                else
                    text.remove(text.length() - 1, 1).prepend(' ');
                tcr.insertText(text);
            }
        }
        else if (Qt::Key_Up == key || Qt::Key_Down == key)
        {
            QTextCursor tcr = tc;
            const int lb = (Qt::Key_Up == key) ? 0 : ranges.size() - 1;
            const int ub = (Qt::Key_Up == key) ? ranges.size() - 1 : 0;
            const int m = (Qt::Key_Up == key) ? 1 : -1;
            for (int i = lb; i != ub; i += m)
            {
                tcr.setPosition(ranges.at(i + m).start);
                tcr.setPosition(ranges.at(i + m).end, QTextCursor::KeepAnchor);
                QString text = tcr.selectedText();
                tcr.setPosition(ranges.at(i).start);
                tcr.setPosition(ranges.at(i).end, QTextCursor::KeepAnchor);
                tcr.insertText(text);
            }
            tcr.setPosition(ranges.at(ub).start);
            tcr.setPosition(ranges.at(ub).end, QTextCursor::KeepAnchor);
            tcr.insertText( QString().fill( ' ', tcr.selectionEnd() - tcr.selectionStart() ) );
        }
    }
    else
    {
        QString text = tc.selectedText();
        if (Qt::Key_Left == key || Qt::Key_Right == key)
        {
            QStringList sl = text.split(QChar::ParagraphSeparator);
            if (Qt::Key_Left == key)
                for (int i = 0; i < sl.size(); ++i)
                    if ( !sl[i].isEmpty() )
                        sl[i].remove(0, 1).append(' ');
            else
               for (int i = 0; i < sl.size(); ++i)
                   if ( !sl[i].isEmpty() )
                       sl[i].remove(sl.at(i).length() - 1, 1).prepend(' ');
            tc.insertText( sl.join("\n") );
        }
        else if (Qt::Key_Up == key)
        {
            tc.insertText( text.toUpper() );
        }
        else if (Qt::Key_Down == key)
        {
            tc.insertText( text.toLower() );
        }
    }
    tc.setPosition(start);
    tc.setPosition(end, QTextCursor::KeepAnchor);
    ptedt->setTextCursor(tc);
    //_m_editSelectionChanged();
    tc.endEditBlock();
}

void BCodeEditPrivate::futureWatcherFinished(ProcessTextFutureWatcher *watcher)
{
    if (watcher)
        return;
    ProcessTextResult res = watcher->result();
    watcher->deleteLater();
    ptedt->setEnabled(true);
    if (highlighter)
        highlighter->setDocument(0);
    ptedt->setPlainText(res.newText);
    if (highlighter)
        highlighter->setDocument( ptedt->document() );
    ptedt->document()->setModified(false);
    ptedt->setFocus();
    emitLinesSplitted(res.splittedLinesRanges);
}

void BCodeEditPrivate::popupMenu(const QPoint &pos)
{
    QMenu *menu = ptedt->createStandardContextMenu();
    //TODO
    QList<QAction *> actions = menu->actions();
    //
    menu->popup( ptedt->mapToGlobal(pos) );
}

void BCodeEditPrivate::updateCursorPosition()
{
    if (bracketsHighlighting)
        highlightBrackets();
    QTextCursor tc = ptedt->textCursor();
    cursorPosition = QPoint( tc.positionInBlock(), tc.blockNumber() );
    QMetaObject::invokeMethod( q_func(), "cursorPositionChanged", Q_ARG(QPoint, cursorPosition) );
}

void BCodeEditPrivate::updateHasSelection()
{
    bool b = hasSelection;
    hasSelection = ptedt->textCursor().hasSelection();
    if (b != hasSelection)
        QMetaObject::invokeMethod( q_func(), "selectionChanged", Q_ARG(bool, hasSelection) );
}

void BCodeEditPrivate::updateHasBookmarks()
{
    bool b = hasBookmarks;
    hasBookmarks = bookmarks.isEmpty();
    if (b != hasBookmarks)
        QMetaObject::invokeMethod( q_func(), "selectionBookmarks", Q_ARG(bool, hasBookmarks) );
}

void BCodeEditPrivate::updateCopyAvailable(bool available)
{
    bool b1 = ptedt->isReadOnly() && copyAvailable;
    bool b2 = copyAvailable;
    copyAvailable = available;
    if ( b1 != (ptedt->isReadOnly() && copyAvailable) )
        QMetaObject::invokeMethod( q_func(), "copyAvailableChanged",
                                   Q_ARG(bool, ptedt->isReadOnly() && copyAvailable) );
    if (b2 != copyAvailable)
        QMetaObject::invokeMethod( q_func(), "copyAvailableChanged", Q_ARG(bool, copyAvailable) );
}

void BCodeEditPrivate::updatePasteAvailable(bool available)
{
    bool b = ptedt->isReadOnly() && pasteAvailable;
    pasteAvailable = available;
    if (b != pasteAvailable)
        QMetaObject::invokeMethod( q_func(), "pasteAvailableChanged",
                                   Q_ARG(bool, ptedt->isReadOnly() && pasteAvailable) );
}

void BCodeEditPrivate::updateUndoAvailable(bool available)
{
    bool b = ptedt->isReadOnly() && undoAvailable;
    undoAvailable = available;
    if (b != undoAvailable)
        QMetaObject::invokeMethod( q_func(), "undoAvailableChanged",
                                   Q_ARG(bool, ptedt->isReadOnly() && undoAvailable) );
}

void BCodeEditPrivate::updateRedoAvailable(bool available)
{
    bool b = ptedt->isReadOnly() && redoAvailable;
    redoAvailable = available;
    if (b != redoAvailable)
        QMetaObject::invokeMethod( q_func(), "redoAvailableChanged",
                                   Q_ARG(bool, ptedt->isReadOnly() && redoAvailable) );
}

//

const QList<QChar> BCodeEditPrivate::unsupportedSymbols = QList<QChar>() << QChar(1) << QChar(2) << QChar(3)
    << QChar(4) << QChar(5) << QChar(6) << QChar(7) << QChar(8) << QChar(9) << QChar(11) << QChar(12) << QChar(13)
    << QChar(14) << QChar(15) << QChar(16) << QChar(17) << QChar(18) << QChar(19) << QChar(20) << QChar(21)
    << QChar(22) << QChar(23)  << QChar(24) << QChar(25) << QChar(26) << QChar(27) << QChar(28) << QChar(29)
    << QChar(30) << QChar(31);

//

BCodeEditPrivate::BCodeEditPrivate(BCodeEdit &q, BCodeEditPrivateObject &o) :
    BBasePrivate(q, o)
{
    //
}

/*============================================================================
================================ Code Edit
============================================================================*/

BCodeEdit::BCodeEdit(QWidget *parent) :
    QWidget(parent), BBase( *new BCodeEditPrivate(this) )
{
    //
}

BCodeEdit::~BCodeEdit()
{
    //
}

//Setters

void BCodeEdit::setReadOnly(bool ro)
{
    B_D(BCodeEdit);
    d->ptedt->setReadOnly(ro);
    d->updateCopyAvailable(d->copyAvailable);
    d->updatePasteAvailable(d->pasteAvailable);
    d->updateUndoAvailable(d->undoAvailable);
    d->updateRedoAvailable(d->redoAvailable);
}

void BCodeEdit::setEditFont(const QFont &fnt)
{
    if ( !QFontInfo(fnt).fixedPitch() )
        return;
    if (fnt.pointSize() < 1 && fnt.pixelSize() < 1)
        return;
    d_func()->ptedt->setFont(fnt);
}

void BCodeEdit::setEditMode(EditMode mode)
{
    B_D(BCodeEdit);
    bool b = (BlockMode == mode);
    if (b == d->blockMode)
        return;
    d->blockMode = b;
    d->ptedt->update();
    emit editModeChanged(mode);
}

void BCodeEdit::setEditLineLength(int ll)
{
    B_D(BCodeEdit);
    if (ll < 1 || ll == d->lineLength)
        return;
    d->lineLength = ll;
    QString text = d->ptedt->toPlainText();
    bool pm = d->ptedt->document()->isModified();
    setText(text);
    d->ptedt->document()->setModified(pm);
}

void BCodeEdit::setEditTabWidth(TabWidth tw)
{
    B_D(BCodeEdit);
    if (tw == d->tabWidth)
        return;
    d->tabWidth = tw;
}

void BCodeEdit::setMaximumBookmarkCount(int count)
{
    B_D(BCodeEdit);
    if (count == d->maxBookmarks)
        return;
    if (count < 0)
    {
        d->bookmarks.clear();
        count = -1;
    }
    else
    {
        while (d->bookmarks.size() > count)
            d->bookmarks.removeFirst();
    }
    d->maxBookmarks = count;
}

void BCodeEdit::setBracketHighlightingEnabled(bool enabled)
{
    B_D(BCodeEdit);
    d->bracketsHighlighting = enabled;
    d->highlightBrackets();
}

void BCodeEdit::setFileType(BAbstractFileType *type)
{
    if (!type)
        return;
    B_D(BCodeEdit);
    d->highlighter->deleteLater();
    d->fileType = type;
    d->brackets = type->brackets();
    d->highlighter = type->createHighlighter();
    if (d->highlighter)
        d->highlighter->setDocument( d->ptedt->document() );
}

//Getters

bool BCodeEdit::isReadOnly() const
{
    return d_func()->ptedt->isReadOnly();
}

bool BCodeEdit::isModified() const
{
    return d_func()->ptedt->document()->isModified();
}

bool BCodeEdit::hasSelection() const
{
    return d_func()->hasSelection;
}

bool BCodeEdit::hasBookmarks() const
{
    return d_func()->hasBookmarks;
}

bool BCodeEdit::isCutAvailable() const
{
    const B_D(BCodeEdit);
    return !d->ptedt->isReadOnly() && d->copyAvailable;
}

bool BCodeEdit::isCopyAvailable() const
{
    return d_func()->copyAvailable;
}

bool BCodeEdit::isPasteAvailable() const
{
    const B_D(BCodeEdit);
    return !d->ptedt->isReadOnly() && d->pasteAvailable;
}

bool BCodeEdit::isUndoAvailable() const
{
    const B_D(BCodeEdit);
    return !d->ptedt->isReadOnly() && d->undoAvailable;
}

bool BCodeEdit::isRedoAvailable() const
{
    const B_D(BCodeEdit);
    return !d->ptedt->isReadOnly() && d->redoAvailable;
}

QFont BCodeEdit::editFont() const
{
    return d_func()->ptedt->font();
}

BCodeEdit::EditMode BCodeEdit::editMode() const
{
    return d_func()->blockMode ? BlockMode : NormalMode;
}

int BCodeEdit::editLineLength() const
{
    return d_func()->lineLength;
}

BCodeEdit::TabWidth BCodeEdit::editTabWidth() const
{
    return d_func()->tabWidth;
}

int BCodeEdit::maximumBookmarkCount() const
{
    return d_func()->maxBookmarks;
}

bool BCodeEdit::isBracketHighlightingEnabled() const
{
    return d_func()->bracketsHighlighting;
}

BAbstractFileType *BCodeEdit::fileType() const
{
    return d_func()->fileType;
}

QPoint BCodeEdit::cursorPosition() const
{
    return d_func()->cursorPosition;
}

QString BCodeEdit::text() const
{
    const QString text = d_func()->ptedt->toPlainText().replace(QChar::ParagraphSeparator, '\n');
    return BCodeEditPrivate::removeTrailingSpaces(text);
}

QString BCodeEdit::selectedText() const
{
    const B_D(BCodeEdit);
    QTextCursor tc = d->ptedt->textCursor();
    if ( !tc.hasSelection() )
        return "";
    if (!d->blockMode)
    {
        const QString text = tc.selectedText().replace(QChar::ParagraphSeparator, '\n');
        return BCodeEditPrivate::removeTrailingSpaces(text);
    }
    QStringList lines;
    foreach ( const BPlainTextEdit::SelectionRange &range, d->ptedt->selectionRanges() )
    {
        QTextBlock tb = d->ptedt->document()->findBlock(range.start);
        int offset = tb.position();
        lines << tb.text().mid(range.start - offset, range.end - offset);
    }
    return lines.join("\n");
}

//Operations

bool BCodeEdit::findNext(const QString &txt, QTextDocument::FindFlags flags, bool cyclic)
{
    if ( txt.isEmpty() )
        return false;
    B_D(BCodeEdit);
    bool b = d->ptedt->find(txt, flags);
    if (!b && cyclic)
    {
        if (flags & QTextDocument::FindBackward)
            d->ptedt->moveCursor(QTextCursor::End);
        else
            d->ptedt->moveCursor(QTextCursor::Start);
        b = d->ptedt->find(txt, flags);
    }
    //if (b)
        //_m_editSelectionChanged();
    return b;
}

bool BCodeEdit::replaceNext(const QString &newText)
{
    if ( isReadOnly() || !hasSelection() )
        return false;
    insertText(newText);
    return true;
}

int BCodeEdit::replaceInSelection(const QString &txt, const QString &newText, Qt::CaseSensitivity cs)
{
    B_D(BCodeEdit);
    if ( isReadOnly() || txt.isEmpty() || txt == newText || txt.length() > d->lineLength)
        return -1;
    return d->blockMode ? d->replaceInSelectionBlocks(txt, newText, cs) : d->replaceInSelectionLines(txt, newText, cs);
}

int BCodeEdit::replaceInDocument(const QString &txt, const QString &newText, Qt::CaseSensitivity cs)
{
    B_D(BCodeEdit);
    if ( isReadOnly() || txt.isEmpty() || txt == newText || txt.length() > d->lineLength)
        return -1;
    QTextCursor tc = d->ptedt->textCursor();
    int pos = tc.position();
    QString ptext = text();
    int count = ptext.count(txt, cs);
    if (!count)
        return 0;
    ptext.replace(txt, newText, cs);
    int lind = ptext.lastIndexOf(txt, -1, cs);
    int llnl = d->lineLength + 1;
    int llinepos = (lind / llnl) * llnl;
    QString lline = ptext.mid(llinepos, d->lineLength);
    BCodeEditPrivate::removeTrailingSpaces(lline);
    int llind = lline.lastIndexOf(txt, -1, cs);
    QString llinex = lline.left(llind).replace(txt, newText, cs);
    lline.replace(0, llind, llinex);
    BCodeEditPrivate::appendTrailingSpaces(lline, d->lineLength);
    ptext.replace(llinepos, d->lineLength, lline);
    lind = ptext.lastIndexOf(txt, -1, cs);
    int dpos = ptext.length() - lind - newText.length();
    BCodeEditPrivate::removeTrailingSpaces(lline);
    if (lline.length() + ( newText.length() - txt.length() ) > d->lineLength)
    {
        dpos += d->lineLength + 1;
        int x = lline.lastIndexOf(txt, -1, cs) + newText.length();
        if (x > d->lineLength)
        {
            dpos -= x - d->lineLength;
            if ( (lline.lastIndexOf(txt, -1, cs) + txt.length() ) == d->lineLength )
                dpos += 1;
        }
    }
    pos = ptext.length() - dpos;
    setText(ptext);
    tc.setPosition(pos);
    d->ptedt->setTextCursor(tc);
    return count;
}

//

QList<BCodeEdit::SplittedLinesRange> BCodeEdit::setText(const QString &txt, int asyncIfLongerThan)
{
    B_D(BCodeEdit);
    if ( isReadOnly() )
        return QList<SplittedLinesRange>();
    if ( txt.isEmpty() )
    {
        d->setTextToEmptyLine();
        return QList<SplittedLinesRange>();
    }
    if (asyncIfLongerThan > 0 && txt.length() > asyncIfLongerThan)
    {
        d->ptedt->setEnabled(false);
        d->ptedt->setPlainText( tr("Processing content, please wait...", "ptedt text") );
        BCodeEditPrivate::ProcessTextFuture fut = QtConcurrent::run(&BCodeEditPrivate::processText,
                                                                    txt, d->lineLength, d->tabWidth);
        BCodeEditPrivate::ProcessTextFutureWatcher *watcher = new BCodeEditPrivate::ProcessTextFutureWatcher(this);
        watcher->setFuture(fut);
        connect( watcher, SIGNAL( finished() ), d->o_func(), SLOT( futureWatcherFinished() ) );
        return QList<SplittedLinesRange>();
    }
    else
    {
        BCodeEditPrivate::ProcessTextResult res = BCodeEditPrivate::processText(txt, d->lineLength, d->tabWidth);
        if (d->highlighter)
            d->highlighter->setDocument(0);
        d->ptedt->setPlainText(res.newText);
        if (d->highlighter)
            d->highlighter->setDocument( d->ptedt->document() );
        d->ptedt->document()->setModified(false);
        d->ptedt->setFocus();
        d->emitLinesSplitted(res.splittedLinesRanges);
        return res.splittedLinesRanges;
    }

}

void BCodeEdit::switchMode()
{
    setEditMode(editMode() == NormalMode ? BlockMode : NormalMode);
}

QList<BCodeEdit::SplittedLinesRange> BCodeEdit::insertText(const QString &txt)
{
    B_D(BCodeEdit);
    if ( d->ptedt->isReadOnly() )
        return QList<SplittedLinesRange>();
    QTextCursor tc = d->ptedt->textCursor();
    tc.beginEditBlock();
    d->deleteSelection();
    if ( txt.isEmpty() )
    {
        tc.endEditBlock();
        return QList<SplittedLinesRange>(); //TODO: deleteSelection() must return a splitted lines list
    }
    tc = d->ptedt->textCursor();
    int posb = tc.positionInBlock();
    QString btext = tc.block().text();
    QString ltext = btext.left(posb);
    QString rtext = btext.right(btext.length() - posb);
    BCodeEditPrivate::removeTrailingSpaces(rtext);
    QStringList sl = BCodeEditPrivate::replaceTabs(BCodeEditPrivate::removeUnsupportedSymbols(txt),
                                                   d->tabWidth).split('\n');
    bool b = false;
    if ( d->blockMode && sl.size() > 1 && sl.size() < d->ptedt->blockCount() )
    {
        b = true;
        int len = sl.first().length();
        for (int i = 0; i < sl.size(); ++i)
        {
            if (sl.at(i).length() != len)
            {
                b = false;
                break;
            }
        }
    }
    QList<SplittedLinesRange> ranges;
    if (b)
    {
        //TODO: Improve text insertion in block mode
        int offset = ltext.length();
        QTextBlock tb = tc.block();
        int  i = 0;
        while ( tb.isValid() && i < sl.size() )
        {
            int bpos = tb.position();
            QString t = tb.text();
            t.insert( offset, sl.at(i) );
            if (t.length() > d->lineLength)
                t.truncate(d->lineLength);
            BCodeEditPrivate::appendTrailingSpaces(t, d->lineLength);
            tc.setPosition(bpos);
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            tc.insertText(t);
            tb = tb.next();
            ++i;
        }
        QString spaces;
        spaces.fill(' ', offset);
        while ( i < sl.size() )
        {
            tc.insertBlock();
            QString &t = sl[i];
            t.prepend(spaces);
            if (t.length() > d->lineLength)
                t.truncate(d->lineLength);
            BCodeEditPrivate::appendTrailingSpaces(t, d->lineLength);
            tc.insertText(t);
            ++i;
        }
        tb = tc.block().next();
        if ( !tb.isValid() )
        {
            tc.movePosition(QTextCursor::EndOfBlock);
            tc.insertBlock();
            tc.insertText( QString().fill(' ', d->lineLength) );
            tc.setPosition(tc.block().position() + offset);
        }
        else
        {
            tc.setPosition(tb.position() + offset);
        }
    }
    else
    {
        sl.first().prepend(ltext);
        sl.last().append(rtext);
        BCodeEditPrivate::ProcessTextResult res = BCodeEditPrivate::processText(sl.join("\n"), d->lineLength,
                                                                                d->tabWidth);
        int boffset = tc.block().blockNumber();
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.insertText(res.newText);
        int pos = d->lineLength;
        QString btext = tc.block().text();
        while (pos > 0 && btext.at(pos - 1) == ' ')
            --pos;
        if (pos > 0)
            pos -= rtext.length();
        tc.setPosition(tc.block().position() + pos);
        ranges = res.splittedLinesRanges;
        for (int i = 0; i < ranges.size(); ++i)
        {
            ranges[i].firstLineNumber += boffset;
            ranges[i].lastLineNumber += boffset;
        }

        d->emitLinesSplitted(ranges);
    }
    d->ptedt->setTextCursor(tc);
    if (d->highlighter)
        d->highlighter->rehighlight();
    tc.endEditBlock();
    return ranges;
}

void BCodeEdit::moveCursor(const QPoint &pos)
{
    B_D(BCodeEdit);
    QTextBlock tb = d->ptedt->document()->findBlockByLineNumber( pos.y() );
    if ( !tb.isValid() )
        return;
    QTextCursor tc = d->ptedt->textCursor();
    tc.setPosition( tb.position() + (pos.x() > 0 ? pos.x() : 0) );
    d->ptedt->setTextCursor(tc);
    //_m_editSelectionChanged();
}

void BCodeEdit::selectText(const QPoint &start, const QPoint &end)
{
    B_D(BCodeEdit);
    QTextBlock tbs = d->ptedt->document()->findBlockByLineNumber( start.y() );
    if ( !tbs.isValid() )
        return;
    QTextBlock tbe = d->ptedt->document()->findBlockByLineNumber( end.y() );
    if ( !tbe.isValid() )
        return;
    QTextCursor tc = d->ptedt->textCursor();
    tc.setPosition(tbs.position() + (start.x() > 0 ? start.x() : 0) );
    tc.setPosition(tbe.position() + (end.x() > 0 ? end.x() : 0), QTextCursor::KeepAnchor);
    d->ptedt->setTextCursor(tc);
    //_m_editSelectionChanged();
}

void BCodeEdit::deselectText()
{
    B_D(BCodeEdit);
    QTextCursor tc = d->ptedt->textCursor();
    if ( !tc.hasSelection() )
        return;
    tc.setPosition( tc.selectionEnd() );
    d->ptedt->setTextCursor(tc);
}

void BCodeEdit::makeBookmark()
{
    B_D(BCodeEdit);
    QTextCursor tc = d->ptedt->textCursor();
    BCodeEditPrivate::Bookmark bm;
    bm.block = tc.block();
    bm.posInBlock = tc.positionInBlock();
    if ( d->bookmarks.contains(bm) )
        d->bookmarks.removeAll(bm);
    d->bookmarks << bm;
    while (d->bookmarks.size() > d->maxBookmarks)
        d->bookmarks.removeFirst();
    d->updateHasBookmarks();
}

void BCodeEdit::removeBookmark(int index)
{
    B_D(BCodeEdit);
    if ( index < 0 || index >= d->bookmarks.size() )
        return;
    int pind = d->bookmarks.indexOf(d->currentBookmark) - 1;
    if (pind < 0)
        pind = 0;
    d->bookmarks.removeAt(index);
    d->currentBookmark = d->bookmarks.at(pind);
    d->updateHasBookmarks();
}

void BCodeEdit::removeLastBookmark()
{
    B_D(BCodeEdit);
    if ( d->bookmarks.isEmpty() )
        return;
    removeBookmark( d->bookmarks.indexOf(d->currentBookmark) );
}

bool BCodeEdit::gotoBookmark(int index)
{
    B_D(BCodeEdit);
    if ( index < 0 || index >= d->bookmarks.size() )
        return false;
    d->currentBookmark = d->bookmarks.at(index);
    if ( !d->currentBookmark.block.isValid() )
    {
        removeBookmark(index);
        d->currentBookmark = BCodeEditPrivate::Bookmark();
        return false;
    }
    QTextCursor tc = d->ptedt->textCursor();
    tc.setPosition(d->currentBookmark.block.position() + d->currentBookmark.posInBlock);
    d->ptedt->setTextCursor(tc);
    return true;
}

bool BCodeEdit::gotoNextBookmark()
{
    B_D(BCodeEdit);
    if ( d->bookmarks.isEmpty() )
        return false;
    int ind = d->bookmarks.indexOf(d->currentBookmark) + 1;
    return gotoBookmark(d->bookmarks.size() != ind ? ind : 0);
}

void BCodeEdit::cut()
{
    if ( isReadOnly() || !hasSelection() )
    copy();
    d_func()->deleteSelection();
}

void BCodeEdit::copy()
{
    QString text = selectedText();
    if ( text.isEmpty() )
        return;
    QApplication::clipboard()->setText(text);
}

void BCodeEdit::paste()
{
    if ( isReadOnly() )
        return;
    QString text = QApplication::clipboard()->text();
    if ( text.isEmpty() )
        return;
    insertText(text);
}

void BCodeEdit::undo()
{
    if ( isReadOnly() || !isUndoAvailable() )
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
    //_m_editCursorPositionChanged();
}

void BCodeEdit::redo()
{
    if ( isReadOnly() || !isRedoAvailable() )
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
    //_m_editCursorPositionChanged();
}

//

BCodeEdit::BCodeEdit(BCodeEditPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    //
}

//

BPlainTextEdit *BCodeEdit::innerEdit() const
{
    return d_func()->ptedt;
}
