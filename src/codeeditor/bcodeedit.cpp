#include "bcodeedit.h"
#include "bcodeedit_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
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

/*========== Code Edit Private Object ==========*/

BCodeEditPrivateObject::BCodeEditPrivateObject(BCodeEditPrivate *p) :
    QObject(0), _m_p(p)
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
        return _m_p->keyPressEvent( static_cast<QKeyEvent *>(e) );
    case QEvent::MouseButtonDblClick:
        return _m_p->mouseDoubleClickEvent( static_cast<QMouseEvent *>(e) );
    case QEvent::MouseButtonPress:
        return _m_p->mousePressEvent( static_cast<QMouseEvent *>(e) );
    default:
        return QObject::eventFilter(obj, e);
    }
}

/*========== Code Edit Private ==========*/

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
    //for (int i = 0; i < _m_UnprintedList.size(); ++i)
        //text.remove( _m_UnprintedList.at(i) );
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

//

BCodeEditPrivate::BCodeEditPrivate(BCodeEdit *q) :
    BBasePrivate(q), _m_o( new BCodeEditPrivateObject(this) )
{
    blockMode = false;
    lineLength = 120;
    tabWidth = BCodeEdit::TabWidth4;
    //
    vlt = new QVBoxLayout(q);
      vlt->setContentsMargins(0, 0, 0, 0);
      ptedt = new BPlainTextEdit(q);
        ptedt->installEventFilter(_m_o);
      vlt->addWidget(ptedt);
}

BCodeEditPrivate::~BCodeEditPrivate()
{
    _m_o->deleteLater();
}

//

bool BCodeEditPrivate::keyPressEvent(QKeyEvent *e)
{
    /*if (_m_recorder)
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
            //undo();
            return true;
        case Qt::Key_X:
            //cut();
            return true;
        case Qt::Key_C:
            //copy();
            return true;
        case Qt::Key_V:
            //paste();
            return true;
        case Qt::Key_A:
            //_m_selectAll();
            return true;
        case Qt::Key_End:
            handleEnd(true);
            return true;
        case Qt::Key_Tab:
            //emit requestSwitchDocument();
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
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            //QTimer::singleShot( 0, this, SLOT( _m_editSelectionChanged() ) );
            return false;
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
            //redo();
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

/*========== Code Edit ==========*/

BCodeEdit::BCodeEdit(QWidget *parent) :
    QWidget(parent), BBase( *new BCodeEditPrivate(this) )
{
    //
}

BCodeEdit::~BCodeEdit()
{
    //
}

//

bool BCodeEdit::isReadOnly() const
{
    return d_func()->ptedt->isReadOnly();
}

//

QList<BCodeEdit::SplittedLinesRange> BCodeEdit::insertText(const QString &text)
{
    B_D(BCodeEdit);
    if ( d->ptedt->isReadOnly() || text.isEmpty() )
        return QList<SplittedLinesRange>();
    QTextCursor tc = d->ptedt->textCursor();
    tc.beginEditBlock();
    d->deleteSelection();
    tc = d->ptedt->textCursor();
    int posb = tc.positionInBlock();
    QString btext = tc.block().text();
    QString ltext = btext.left(posb);
    QString rtext = btext.right(btext.length() - posb);
    BCodeEditPrivate::removeTrailingSpaces(rtext);
    QStringList sl = BCodeEditPrivate::replaceTabs(BCodeEditPrivate::removeUnsupportedSymbols(text),
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
        for (int i = 0; i < res.splittedLinesRanges.size(); ++i)
        {
            res.splittedLinesRanges[i].firstLineNumber += boffset;
            res.splittedLinesRanges[i].lastLineNumber += boffset;
        }
        if (res.splittedLinesRanges.size() > 1)
            emit linesSplitted(res.splittedLinesRanges);
        else if (res.splittedLinesRanges.size() == 1)
            emit lineSplitted( res.splittedLinesRanges.first() );
    }
    d->ptedt->setTextCursor(tc);
    //_m_highlighter->rehighlight();
    tc.endEditBlock();
    return QList<SplittedLinesRange>();
}

//

BCodeEdit::BCodeEdit(BCodeEditPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    //
}
