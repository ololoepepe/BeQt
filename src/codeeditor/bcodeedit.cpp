#include "bcodeedit.h"
#include "bcodeedit_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BApplication>
#include <BeQtWidgets/BPlainTextEdit>

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
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
#include <QMetaObject>
#include <QSyntaxHighlighter>
#include <QPoint>
#include <QMenu>
#include <QAction>
#include <QFont>
#include <QFontInfo>
#include <QApplication>
#include <QClipboard>
#include <QTextEdit>
#include <QMap>
#include <QTextCharFormat>
#include <QBrush>
#include <QColor>
#include <QTimer>
#include <QRegExp>
#include <QPlainTextEdit>
#include <QPainter>
#include <QRectF>
#include <QBrush>
#include <QTransform>
#include <QGradient>
#include <QPaintEvent>
#include <QPlainTextDocumentLayout>
#include <QPointF>
#include <QRect>
#include <QAbstractTextDocumentLayout>
#include <QTextLayout>
#include <QTextBlockFormat>
#include <QVector>
#include <QTextLine>
#include <QScrollBar>
#include <QVector>
#include <QThreadPool>
#include <QRunnable>
#include <QResizeEvent>
#include <QSize>
#include <QTextBlockUserData>
#include <QRegularExpression>

#include <QDebug>

/*============================================================================
================================ BTextBlockUserData ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BTextBlockUserData::BTextBlockUserData(int sf, int st)
{
    skipFrom = sf;
    skipTo = st;
}

BTextBlockUserData::~BTextBlockUserData()
{
    //
}

/*============================== Static public methods =====================*/

QString BTextBlockUserData::textWithoutComments(const BTextBlockUserData *ud, const QString &text)
{
    if (!ud || ud->skipFrom < 0)
        return text;
    QString ntext = text;
    int len = ( ud->skipTo >= 0 ? ud->skipTo : text.length() ) - ud->skipFrom;
    ntext.replace( ud->skipFrom, len, QString().fill(' ', len) );
    return ntext;
}

QString BTextBlockUserData::textWithoutComments(const QTextBlock &block)
{
    return textWithoutComments( static_cast<BTextBlockUserData *>( block.userData() ), block.text() );
}

int BTextBlockUserData::blockSkipFrom(const QTextBlock &block)
{
    BTextBlockUserData *ud = static_cast<BTextBlockUserData *>( block.userData() );
    return ud ? ud->skipFrom : -1;
}

/*============================================================================
================================ BCodeEditClipboardNotifier ==================
============================================================================*/

/*============================== Public constructors =======================*/

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

/*============================== Static public methods =====================*/

BCodeEditClipboardNotifier *BCodeEditClipboardNotifier::instance()
{
    return _m_self;
}

/*============================== Public methods ============================*/

bool BCodeEditClipboardNotifier::clipboardDataAvailable() const
{
    return dataAvailable;
}

/*============================== Public slots ==============================*/

void BCodeEditClipboardNotifier::dataChanged()
{
    bool b = !QApplication::clipboard()->text().isEmpty();
    bool bb = (b != dataAvailable);
    dataAvailable = b;
    if (bb)
        emit clipboardDataAvailableChanged(b);
}

/*============================== Static protected variables ================*/

BCodeEditClipboardNotifier *BCodeEditClipboardNotifier::_m_self = 0;

/*============================================================================
================================ BLineNumberWidget ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BLineNumberWidget::BLineNumberWidget(BPlainTextEditExtended *ptedt) :
    QWidget(ptedt), Ptedt(ptedt)
{
    //
}

BLineNumberWidget::~BLineNumberWidget()
{
    //
}

/*============================== Public methods ============================*/

QSize BLineNumberWidget::sizeHint() const
{
    return QSize(Ptedt->d_func()->lineNumberWidgetWidth(), 0);
}

/*============================== Protected methods =========================*/

void BLineNumberWidget::paintEvent(QPaintEvent *e)
{
    Ptedt->d_func()->lineNumberWidgetPaintEvent(e);
}

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
    if (bs >= Qt::LinearGradientPattern && bs <= Qt::ConicalGradientPattern)
    {
        if ( !gradientRect.isNull() )
        {
            QTransform t = QTransform::fromTranslate( gradientRect.left(), gradientRect.top() );
            t.scale( gradientRect.width(), gradientRect.height() );
            brush.setTransform(t);
            const_cast<QGradient *>( brush.gradient() )->setCoordinateMode(QGradient::LogicalMode);
        }
    }
    else
    {
        painter->setBrushOrigin( rect.topLeft() );
    }
    painter->fillRect(rect, brush);
    painter->restore();
}

/*============================== Public methods ============================*/

void BPlainTextEditExtendedPrivate::init()
{
    B_Q(BPlainTextEditExtended);
    blockMode = false;
    hasSelection = false;
    lnwgt = new BLineNumberWidget(q);
    connect( q, SIGNAL( selectionChanged() ), this, SLOT( selectionChanged() ) );
    connect( q, SIGNAL( blockCountChanged(int) ), this, SLOT( updateLineNumberWidgetWidth(int) ) );
    connect( q, SIGNAL( updateRequest(QRect, int) ), this, SLOT( updateLineNumberWidget(QRect, int) ) );
    updateLineNumberWidgetWidth(0);
}

void BPlainTextEditExtendedPrivate::emulateShiftPress()
{
    QKeyEvent e(QKeyEvent::KeyPress, Qt::Key_Shift, Qt::NoModifier);
    QApplication::sendEvent(q_func(), &e);
}

void BPlainTextEditExtendedPrivate::lineNumberWidgetPaintEvent(QPaintEvent *e)
{
    B_Q(BPlainTextEditExtended);
    QPainter painter(lnwgt);
    painter.fillRect(e->rect(), Qt::lightGray);
    QTextBlock block = q->textCursor().block();
    if ( block.isVisible() )
    {
        QRect r = e->rect();
        r.setTop( (int) q->blockBoundingGeometry(block).translated( q->contentOffset() ).top() );
        r.setBottom( r.top() + (int) q->blockBoundingRect(block).height() );
        painter.fillRect(r, Qt::yellow);
    }
    block = q->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) q->blockBoundingGeometry(block).translated( q->contentOffset() ).top();
    int bottom = top + (int) q->blockBoundingRect(block).height();
    while ( block.isValid() && top <= e->rect().bottom() )
    {
        if ( block.isVisible() && bottom >= e->rect().top() )
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lnwgt->width(), q->fontMetrics().height(), Qt::AlignRight, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + (int) q->blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int BPlainTextEditExtendedPrivate::lineNumberWidgetWidth() const
{
    int digits = 1;
    int max = qMax( 1, q_func()->blockCount() );
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }
    int space = 3 + q_func()->fontMetrics().width( QLatin1Char('9') ) * digits;
    return space;
}

QAbstractTextDocumentLayout::PaintContext BPlainTextEditExtendedPrivate::getPaintContext() const
{
    QAbstractTextDocumentLayout::PaintContext context = q_func()->getPaintContext();
    if (!blockMode || !hasSelection)
        return context;
    QAbstractTextDocumentLayout::Selection sel = context.selections.last();
    context.selections.remove(context.selections.size() - 1);
    foreach (const BPlainTextEditExtended::SelectionRange &sr, selectionRanges)
    {
        sel.cursor.setPosition(sr.start);
        sel.cursor.setPosition(sr.end, QTextCursor::KeepAnchor);
        context.selections.append(sel);
    }
    return context;
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
    while ( tb.isValid() )
    {
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

void BPlainTextEditExtendedPrivate::updateLineNumberWidgetWidth(int)
{
    q_func()->setViewportMargins(lineNumberWidgetWidth(), 0, 0, 0);
}

void BPlainTextEditExtendedPrivate::updateLineNumberWidget(const QRect &rect, int dy)
{
    if (dy)
        lnwgt->scroll(0, dy);
    else
        lnwgt->update( 0, rect.y(), lnwgt->width(), rect.height() );
    if ( rect.contains( q_func()->viewport()->rect() ) )
        updateLineNumberWidgetWidth(0);
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

bool BPlainTextEditExtended::blockMode() const
{
    return d_func()->blockMode;
}

QVector<BPlainTextEditExtended::SelectionRange> BPlainTextEditExtended::selectionRanges() const
{
    return d_func()->selectionRanges;
}

/*============================== Protected methods =========================*/

void BPlainTextEditExtended::paintEvent(QPaintEvent *e)
{
    QPainter painter( viewport() );
    Q_ASSERT( qobject_cast<QPlainTextDocumentLayout*>( document()->documentLayout() ) );
    QPointF offset( contentOffset() );
    QRect er = e->rect();
    QRect viewportRect = viewport()->rect();
    bool editable = !isReadOnly();
    QTextBlock block = firstVisibleBlock();
    qreal maximumWidth = document()->documentLayout()->documentSize().width();
    painter.setBrushOrigin(offset);
    int maxX = offset.x() + qMax( (qreal) viewportRect.width(), maximumWidth ) - document()->documentMargin();
    er.setRight( qMin(er.right(), maxX) );
    painter.setClipRect(er);
    QAbstractTextDocumentLayout::PaintContext context = d_func()->getPaintContext(); //Custom paint context
    while ( block.isValid() )
    {
        QRectF r = blockBoundingRect(block).translated(offset);
        QTextLayout *layout = block.layout();
        if ( !block.isVisible() )
        {
            offset.ry() += r.height();
            block = block.next();
            continue;
        }
        if ( r.bottom() >= er.top() && r.top() <= er.bottom() )
        {
            QTextBlockFormat blockFormat = block.blockFormat();
            QBrush bg = blockFormat.background();
            if (bg != Qt::NoBrush)
            {
                QRectF contentsRect = r;
                contentsRect.setWidth( qMax(r.width(), maximumWidth) );
                BPlainTextEditExtendedPrivate::fillBackground(&painter, contentsRect, bg);
            }
            QVector<QTextLayout::FormatRange> selections;
            int blpos = block.position();
            int bllen = block.length();
            for (int i = 0; i < context.selections.size(); ++i)
            {
                const QAbstractTextDocumentLayout::Selection &range = context.selections.at(i);
                const int selStart = range.cursor.selectionStart() - blpos;
                const int selEnd = range.cursor.selectionEnd() - blpos;
                if (selStart < bllen && selEnd > 0 && selEnd > selStart)
                {
                    QTextLayout::FormatRange o;
                    o.start = selStart;
                    o.length = selEnd - selStart;
                    o.format = range.format;
                    selections.append(o);
                }
                else if ( !range.cursor.hasSelection() &&
                          range.format.hasProperty(QTextFormat::FullWidthSelection) &&
                          block.contains( range.cursor.position() ) )
                {
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
            bool drawCursor = ( ( editable || (textInteractionFlags() & Qt::TextSelectableByKeyboard) ) &&
                                context.cursorPosition >= blpos && context.cursorPosition < blpos + bllen );
            bool drawCursorAsBlock = drawCursor && overwriteMode();
            if (drawCursorAsBlock)
            {
                if (context.cursorPosition == blpos + bllen - 1)
                {
                    drawCursorAsBlock = false;
                }
                else
                {
                    QTextLayout::FormatRange o;
                    o.start = context.cursorPosition - blpos;
                    o.length = 1;
                    o.format.setForeground( palette().base() );
                    o.format.setBackground( palette().text() );
                    selections.append(o);
                }
            }
            layout->draw(&painter, offset, selections, er);
            if ( (drawCursor && !drawCursorAsBlock) ||
                 ( editable && context.cursorPosition < -1 && !layout->preeditAreaText().isEmpty() ) )
            {
                int cpos = context.cursorPosition;
                if (cpos < -1)
                    cpos = layout->preeditAreaPosition() - (cpos + 2);
                else
                    cpos -= blpos;
                layout->drawCursor( &painter, offset, cpos, cursorWidth() );
            }
        }
        offset.ry() += r.height();
        if ( offset.y() > viewportRect.height() )
            break;
        block = block.next();
    }
    if ( backgroundVisible() && !block.isValid() && offset.y() <= er.bottom() &&
         ( centerOnScroll() || verticalScrollBar()->maximum() == verticalScrollBar()->minimum() ) )
        painter.fillRect( QRect( QPoint( (int) er.left(), (int) offset.y() ),
                                 er.bottomRight() ), palette().background() );
}

void BPlainTextEditExtended::resizeEvent(QResizeEvent *e)
{
    BPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    d_func()->lnwgt->setGeometry( QRect( cr.left(), cr.top(), d_func()->lineNumberWidgetWidth(), cr.height() ) );
}

/*============================================================================
================================ BCodeEditPrivate ============================
============================================================================*/

/*============================== Public constructors =======================*/

BCodeEditPrivate::BCodeEditPrivate(BCodeEdit *q) :
    BBasePrivate(q)
{
    //
}

BCodeEditPrivate::~BCodeEditPrivate()
{
    if (highlighter)
        highlighter->deleteLater();
}

/*============================== Static public methods =====================*/

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
            sl << appendTrailingSpaces(xline.left(l), ll);
            xline.remove( 0, l + (k > 1 ? 1 : 0) );
        }
        if ( !xline.isEmpty() )
            sl << appendTrailingSpaces(xline, ll);;
    }
    else
    {
        sl << appendTrailingSpaces(xline, ll);
    }
    return sl;
}

BCodeEditPrivate::ProcessTextResult BCodeEditPrivate::processText(const QString &text, int ll, BCodeEdit::TabWidth tw)
{
    ProcessTextResult res;
    QStringList sl = removeUnsupportedSymbols(text).split('\n');
    QStringList sln;
    for (int i = 0; i < sl.size(); ++i)
    {
        QStringList lines = processLine(sl.at(i), ll, tw);
        if (lines.size() > 1)
        {
            BCodeEdit::SplittedLinesRange r = createSplittedLinesRange();
            r.firstLineNumber = sln.size();
            r.lastLineNumber = r.firstLineNumber + lines.size() - 1;
            if (!res.splittedLinesRanges.isEmpty() &&
                    res.splittedLinesRanges.last().lastLineNumber == r.firstLineNumber - 1)
                res.splittedLinesRanges.last().lastLineNumber = r.lastLineNumber;
            else
                res.splittedLinesRanges << r;
        }
        sln << lines;
    }
    res.newText = sln.join("\n");
    return res;
}

QString BCodeEditPrivate::removeUnsupportedSymbols(const QString &s)
{
    QString ns = s;
    removeUnsupportedSymbols(&ns);
    return ns;
}

void BCodeEditPrivate::removeUnsupportedSymbols(QString *text)
{
    if (!text)
        return;
    foreach ( const QChar &c, createUnsupportedSymbols() )
        text->remove(c);
}

QString BCodeEditPrivate::removeTrailingSpaces(const QString &s)
{
    QString ns = s;
    removeTrailingSpaces(&ns);
    return ns;
}

void BCodeEditPrivate::removeTrailingSpaces(QString *s)
{
    if (!s || s->isEmpty())
        return;
    QStringList sl = s->split('\n');
    static QRegularExpression rx("\\s+$");
    foreach ( int i, bRange(0, sl.size() - 1) )
        sl[i].remove(rx);
    *s = sl.join('\n');
}

QString BCodeEditPrivate::appendTrailingSpaces(const QString &s, int ll)
{
    QString ns = s;
    appendTrailingSpaces(&ns, ll);
    return ns;
}

void BCodeEditPrivate::appendTrailingSpaces(QString *s, int ll)
{
    if (!s)
        return;
    int len = s->length();
    if (len < ll)
        s->append( QString().fill(' ', ll - len) );
}

QString BCodeEditPrivate::replaceTabs(const QString &s, BCodeEdit::TabWidth tw)
{
    QString ns = s;
    replaceTabs(&ns, tw);
    return ns;
}

void BCodeEditPrivate::replaceTabs(QString *s, BCodeEdit::TabWidth tw)
{
    if (!s)
        return;
    int i = 0;
    while ( i < s->length() )
    {
        if (s->at(i) == '\t')
        {
            int spcount = tw - (i < tw ? i : i % tw);
            s->replace( i, 1, QString().fill(' ', spcount ? spcount : tw) );
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

QList<QChar> BCodeEditPrivate::createUnsupportedSymbols()
{
    QList<QChar> list;
    list << QChar(1) << QChar(2) << QChar(3) << QChar(4) << QChar(5) << QChar(6) << QChar(7) << QChar(8) << QChar(9);
    list << QChar(11) << QChar(12) << QChar(13) << QChar(14) << QChar(15) << QChar(16) << QChar(17) << QChar(18);
    list << QChar(19) << QChar(20) << QChar(21) << QChar(22) << QChar(23)  << QChar(24) << QChar(25) << QChar(26);
    list << QChar(27) << QChar(28) << QChar(29) << QChar(30) << QChar(31);
    return list;
}

QTextCharFormat BCodeEditPrivate::createBracketsFormat()
{
    QTextCharFormat fmt;
    fmt.setForeground( QBrush( QColor("red") ) );
    return fmt;
}

QTextCharFormat BCodeEditPrivate::createBracketsErrorFormat()
{
    QTextCharFormat fmt;
    fmt.setBackground( QBrush( QColor("hotpink") ) );
    return fmt;
}

BCodeEdit::SplittedLinesRange BCodeEditPrivate::createSplittedLinesRange()
{
    BCodeEdit::SplittedLinesRange slr;
    slr.firstLineNumber = -1;
    slr.lastLineNumber = -1;
    return slr;
}

BCodeEditPrivate::FindBracketPairResult BCodeEditPrivate::createFindBracketPairResult()
{
    FindBracketPairResult r;
    r.start = -1;
    r.end = -1;
    r.startBr = 0;
    r.endBr = 0;
    return r;
}

QTextEdit::ExtraSelection BCodeEditPrivate::createExtraSelection(const QPlainTextEdit *edt,
                                                                 const QTextCharFormat &format)
{
    QTextEdit::ExtraSelection r;
    if (edt)
        r.cursor = edt->textCursor();
    r.format = format;
    return r;
}

bool BCodeEditPrivate::testBracketPairsEquality(const BCodeEdit::BracketPair &bp1, const BCodeEdit::BracketPair &bp2)
{
    return bp1.opening == bp2.opening && bp1.closing == bp2.closing && bp1.escape == bp2.escape;
}

bool BCodeEditPrivate::testBracketPairListsEquality(const QList<BCodeEdit::BracketPair> &l1,
                                                    const QList<BCodeEdit::BracketPair> &l2)
{
    if ( l1.size() != l2.size() )
        return false;
    for (int i = 0; i < l1.size(); ++i)
        if ( !testBracketPairsEquality( l1.at(i), l2.at(i) ) )
            return false;
    return true;
}

/*============================== Public methods ============================*/

void BCodeEditPrivate::init()
{
    if ( !BCodeEditClipboardNotifier::instance() )
        new BCodeEditClipboardNotifier;
    connect( BCodeEditClipboardNotifier::instance(), SIGNAL( clipboardDataAvailableChanged(bool) ),
             this, SLOT( updatePasteAvailable(bool) ) );
    blockMode = false;
    lineLength = 120;
    tabWidth = BCodeEdit::TabWidth4;
    hasSelection = false;
    hasBookmarks = false;
    copyAvailable = false;
    pasteAvailable = BCodeEditClipboardNotifier::instance()->clipboardDataAvailable();
    undoAvailable = false;
    redoAvailable = false;
    buisy = false;
    parceTask = 0;
    highlighter = 0;
    bracketsHighlighting = true;
    B_Q(BCodeEdit);
    hlt = new QHBoxLayout(q);
      hlt->setContentsMargins(0, 0, 0, 0);
      ptedt = new BPlainTextEditExtended(q);
        ptedt->setLineWrapMode(QPlainTextEdit::NoWrap);
        ptedt->setWordWrapMode(QTextOption::NoWrap);
        ptedt->setDragEnabled(false);
        ptedt->installEventFilter(this);
        ptedt->setContextMenuPolicy(Qt::CustomContextMenu);
        ptedt->setFont( BApplication::createMonospaceFont() );
        setTextToEmptyLine();
        ptedt->document()->setModified(false);
        connect( ptedt, SIGNAL( customContextMenuRequested(QPoint) ), this, SLOT( popupMenu(QPoint) ) );
        connect( ptedt, SIGNAL( cursorPositionChanged() ), this, SLOT( updateCursorPosition() ) );
        connect( ptedt->document(), SIGNAL( modificationChanged(bool) ), this, SLOT( emitModificationChanged(bool) ) );
        connect( ptedt, SIGNAL( selectionChanged() ), this, SLOT( emitSelectionChanged() ) );
        connect( ptedt, SIGNAL( selectionChanged() ), this, SLOT( updateHasSelection() ) );
        connect( ptedt, SIGNAL( copyAvailable(bool) ), this, SLOT( updateCopyAvailable(bool) ) );
        connect( ptedt, SIGNAL( undoAvailable(bool) ), this, SLOT( updateUndoAvailable(bool) ) );
        connect( ptedt, SIGNAL( redoAvailable(bool) ), this, SLOT( updateRedoAvailable(bool) ) );
        //
      hlt->addWidget(ptedt);
}

bool BCodeEditPrivate::eventFilter(QObject *obj, QEvent *e)
{
    switch ( e->type() )
    {
    case QEvent::KeyPress:
        return keyPressEvent( static_cast<QKeyEvent *>(e) );
    case QEvent::MouseButtonDblClick:
        return mouseDoubleClickEvent( static_cast<QMouseEvent *>(e) );
    case QEvent::MouseButtonPress:
        return mousePressEvent( static_cast<QMouseEvent *>(e) );
    default:
        return QObject::eventFilter(obj, e);
    }
}

bool BCodeEditPrivate::keyPressEvent(QKeyEvent *e)
{
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
        default:
            break;
        }
        break;
    case Qt::ShiftModifier:
        switch (key)
        {
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
            handleReturn();
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

bool BCodeEditPrivate::mouseDoubleClickEvent(QMouseEvent *)
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
    tc.setPosition( tb.position() + removeTrailingSpaces( tb.text() ).length() );
    ptedt->setTextCursor(tc);
    return true;
}

BCodeEdit::SplittedLinesRange BCodeEditPrivate::deleteSelection()
{
    QTextCursor tc = ptedt->textCursor();
    if ( !tc.hasSelection() )
        return createSplittedLinesRange();
    tc.beginEditBlock();
    int pos = qMin( tc.selectionStart(), tc.selectionEnd() );
    BCodeEdit::SplittedLinesRange slr = createSplittedLinesRange();
    if (blockMode)
    {
        QVector<BPlainTextEditExtended::SelectionRange> ranges = ptedt->selectionRanges();
        foreach(const BPlainTextEditExtended::SelectionRange &range, ranges)
        {
            tc.setPosition(range.start);
            tc.setPosition(range.end, QTextCursor::KeepAnchor);
            tc.removeSelectedText();
            QString text = appendTrailingSpaces(tc.block().text(), lineLength);
            tc.movePosition(QTextCursor::StartOfBlock);
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            tc.insertText(text);
        }
    }
    else
    {
        tc.removeSelectedText();
        QString text = tc.block().text();
        QStringList sl = processLine(text, lineLength, tabWidth);
        if (sl.size() > 1)
        {
            slr.firstLineNumber = tc.block().blockNumber();
            slr.lastLineNumber = slr.firstLineNumber + sl.size();
        }
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.insertText( sl.join("\n") );
    }
    tc.setPosition(pos);
    ptedt->setTextCursor(tc);
    tc.endEditBlock();
    return slr;
}

void BCodeEditPrivate::seletAll()
{
    if (blockMode)
        return ptedt->selectAll();
    QTextCursor tc = ptedt->textCursor();
    QTextBlock tbl = ptedt->document()->lastBlock();
    int len = removeTrailingSpaces( tbl.text() ).length();
    tc.movePosition(QTextCursor::Start);
    tc.setPosition(tbl.position() + len, QTextCursor::KeepAnchor);
    ptedt->setTextCursor(tc);
}

void BCodeEditPrivate::setText(const QString &txt, int asyncIfLongerThan)
{
    if ( txt.isEmpty() )
       return setTextToEmptyLine();
    setBuisy(true);
    if (asyncIfLongerThan > 0 && txt.length() > asyncIfLongerThan)
    {
        ptedt->setEnabled(false);
        ptedt->setPlainText( tr("Processing content, please wait...", "ptedt text") );
        parceTask = new BCodeEditParseTask(txt, lineLength, tabWidth);
        parceTask->setAutoDelete(false);
        connect(parceTask, SIGNAL( finished() ), this, SLOT( parceTaskFinished() ), Qt::QueuedConnection);
        BCodeEditParseTask::pool()->start(parceTask);
    }
    else
    {
        ProcessTextResult res = processText(txt, lineLength, tabWidth);
        if (highlighter)
            highlighter->setDocument(0);
        ptedt->setPlainText(res.newText);
        QTextCursor tc = ptedt->textCursor();
        tc.movePosition(QTextCursor::Start);
        ptedt->setTextCursor(tc);
        if (highlighter)
            highlighter->setDocument( ptedt->document() );
        ptedt->setFocus();
        emitLinesSplitted(res.splittedLinesRanges);
        setBuisy(false);
    }
}

void BCodeEditPrivate::setBuisy(bool b)
{
    if (b == buisy)
        return;
    buisy = b;
    QMetaObject::invokeMethod( q_func(), "buisyChanged", Q_ARG(bool, b) );
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
    removeTrailingSpaces(&ntext);
    ntext.replace(text, newText, cs);
    q->insertText(ntext);
    tc = ptedt->textCursor();
    int pos = tc.position();
    tc.setPosition(end < start ? pos : start);
    tc.setPosition(end < start ? end : pos, QTextCursor::KeepAnchor);
    ptedt->setTextCursor(tc);
    tc.endEditBlock();
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
    //TODO (release 3.0.0): Improve replacing
    if (!blockMode)
        return -1;
    B_Q(BCodeEdit);
    if ( !q->hasSelection() )
        return -1;
    int count = 0;
    int maxlen = 0;
    QStringList sl;
    QTextCursor tcr = ptedt->textCursor();
    QVector<BPlainTextEditExtended::SelectionRange> ranges = ptedt->selectionRanges();
    foreach (const BPlainTextEditExtended::SelectionRange &range, ranges)
    {
        tcr.setPosition(range.start);
        tcr.setPosition(range.end, QTextCursor::KeepAnchor);
        QString st = tcr.selectedText();
        int slen = st.length();
        count += st.count(text, cs);
        removeTrailingSpaces(&st);
        st.replace(text, newText, cs);
        if (st.length() > maxlen)
            maxlen = st.length();
        QTextBlock tbr = tcr.block();
        QString txt = removeTrailingSpaces( tbr.text() );
        sl << txt.replace(tcr.selectionStart() - tbr.position(), slen, st);
        if (txt.length() > lineLength) //TODO (release 3.0.0): This must be fixed
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
    tc.endEditBlock();
    if (highlighter)
        foreach (const BPlainTextEditExtended::SelectionRange &range, ranges)
            highlighter->rehighlightBlock( ptedt->document()->findBlock(range.start) );
    return count;
}

void BCodeEditPrivate::highlightBrackets()
{
    QList<QTextEdit::ExtraSelection> selections = ptedt->extraSelections();
    removeExtraSelections(selections, highlightedBrackets);
    highlightedBrackets.clear();
    QList<FindBracketPairResult> list;
    list << findLeftBracketPair();
    list << findRightBracketPair();
    foreach (const FindBracketPairResult &res, list)
    {
        if (res.start >= 0 && res.end >= 0)
        {
            if ( testBracketPairsEquality(*res.startBr, *res.endBr) )
            {
                QTextEdit::ExtraSelection ess = createExtraSelection( ptedt, createBracketsFormat() );
                ess.cursor.setPosition(res.start);
                ess.cursor.setPosition(res.start + res.startBr->opening.length(), QTextCursor::KeepAnchor);
                highlightedBrackets << ess;
                selections << ess;
                QTextEdit::ExtraSelection ese = createExtraSelection( ptedt, createBracketsFormat() );
                ese.cursor.setPosition( res.end - res.endBr->closing.length() );
                ese.cursor.setPosition(res.end, QTextCursor::KeepAnchor);
                highlightedBrackets << ese;
                selections << ese;
            }
            else
            {
                QTextEdit::ExtraSelection es = createExtraSelection( ptedt, createBracketsErrorFormat() );
                es.cursor.setPosition(res.start);
                es.cursor.setPosition(res.end, QTextCursor::KeepAnchor);
                highlightedBrackets << es;
                selections << es;
            }
        }
    }
    ptedt->setExtraSelections(selections);
}

BCodeEditPrivate::FindBracketPairResult BCodeEditPrivate::findLeftBracketPair() const
{
    FindBracketPairResult res = createFindBracketPairResult();
    QTextCursor tc = ptedt->textCursor();
    QTextBlock tb = tc.block();
    int posInBlock = tc.positionInBlock();
    const BCodeEdit::BracketPair *bracket = 0;
    if ( !testBracket(BTextBlockUserData::textWithoutComments(tb), posInBlock, false, bracket) )
        return res;
    res.end = tb.position() + posInBlock;
    posInBlock -= bracket->closing.length();
    int depth = 1;
    const BCodeEdit::BracketPair *br = 0;
    while ( tb.isValid() )
    {
        QString text = removeTrailingSpaces( BTextBlockUserData::textWithoutComments(tb) );
        while (posInBlock >= 0)
        {
            if ( testBracket(text, posInBlock, true, br) )
            {
                --depth;
                if (!depth)
                {
                    res.start = tb.position() + posInBlock;
                    res.startBr = br;
                    res.endBr = bracket;
                    return res;
                }
                posInBlock -= br->opening.length();
            }
            else if ( testBracket(text, posInBlock, false, br) )
            {
                ++depth;
                posInBlock -= br->closing.length();
            }
            else
            {
                --posInBlock;
            }
        }
        tb = tb.previous();
        int skipFrom = BTextBlockUserData::blockSkipFrom(tb);
        posInBlock = (skipFrom >= 0) ? (skipFrom - 1) : tb.length();
    }
    return res;
}

BCodeEditPrivate::FindBracketPairResult BCodeEditPrivate::findRightBracketPair() const
{
    FindBracketPairResult res = createFindBracketPairResult();
    QTextCursor tc = ptedt->textCursor();
    QTextBlock tb = tc.block();
    int posInBlock = tc.positionInBlock();
    const BCodeEdit::BracketPair *bracket = 0;
    if ( !testBracket(BTextBlockUserData::textWithoutComments(tb), posInBlock, true, bracket) )
        return res;
    res.start = tb.position() + posInBlock;
    posInBlock += bracket->opening.length();
    int depth = 1;
    const BCodeEdit::BracketPair *br = 0;
    while ( tb.isValid() )
    {
        QString text = removeTrailingSpaces( BTextBlockUserData::textWithoutComments(tb) );
        while ( posInBlock <= text.length() )
        {
            if ( testBracket(text, posInBlock, false, br) )
            {
                --depth;
                if (!depth)
                {
                    res.end = tb.position() + posInBlock;
                    res.startBr = bracket;
                    res.endBr = br;
                    return res;
                }
                posInBlock += br->closing.length();
            }
            else if ( testBracket(text, posInBlock, true, br) )
            {
                ++depth;
                posInBlock += br->opening.length();
            }
            else
            {
                ++posInBlock;
            }
        }
        tb = tb.next();
        posInBlock = 0;
    }
    return res;
}

bool BCodeEditPrivate::testBracket(const QString &text, int posInBlock, bool opening,
                                   const BCodeEdit::BracketPair *&bracket) const
{
    bracket = 0;
    if ( recognizedBrackets.isEmpty() )
        return false;
    foreach (const BCodeEdit::BracketPair &br, recognizedBrackets)
    {
        const QString &brText = opening ? br.opening : br.closing;
        if ( brText.isEmpty() )
            continue;
        int len = brText.length();
        int pos = opening ? posInBlock : posInBlock - len;
        if (text.mid(pos, len) != brText)
            continue;
        if ( br.escape.isEmpty() )
        {
            bracket = &br;
            return true;
        }
        len = br.escape.length();
        if ( text.mid(pos - len, len) != br.escape)
        {
            bracket = &br;
            return true;
        }
    }
    return false;
}

void BCodeEditPrivate::emitLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> &ranges)
{
    if (ranges.size() > 1)
        QMetaObject::invokeMethod( q_func(), "linesSplitted",
                                   Q_ARG(QList<BCodeEdit::SplittedLinesRange>, ranges) );
    else if (ranges.size() == 1 && ranges.first().firstLineNumber >= 0)
        QMetaObject::invokeMethod( q_func(), "lineSplitted",
                                   Q_ARG(const BCodeEdit::SplittedLinesRange, ranges.first() ) );
}

void BCodeEditPrivate::emitLineSplitted(const BCodeEdit::SplittedLinesRange &range)
{
    emitLinesSplitted(QList<BCodeEdit::SplittedLinesRange>() << range);
}

void BCodeEditPrivate::handleReturn()
{
    if ( ptedt->isReadOnly() )
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
    QString ltext = appendTrailingSpaces(text.left(posb), lineLength);
    QString rtext = appendTrailingSpaces(text.right(text.length() - posb).prepend( QString().fill(' ', i) ),
                                         lineLength);
    tc.movePosition(QTextCursor::StartOfBlock);
    tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    tc.insertText(ltext);
    tc.insertBlock();
    tc.insertText(rtext);
    tc.movePosition(QTextCursor::StartOfBlock);
    tc.setPosition(tc.position() + i);
    ptedt->setTextCursor(tc);
    tc.endEditBlock();
    //Not sure if this is necessary
    if (highlighter)
        highlighter->rehighlightBlock(tbp);
}

void BCodeEditPrivate::handleSpace()
{
    if ( ptedt->isReadOnly() )
        return;
    QTextCursor tc = ptedt->textCursor();
    tc.beginEditBlock();
    if (hasSelection)
        deleteSelection();
    tc = ptedt->textCursor();
    QString text = tc.block().text();
    static QRegExp rx("\\S");
    if ( tc.positionInBlock() == text.length() )
    {
        tc.insertBlock();
        tc.insertText( QString().fill(' ', lineLength) );
        tc.movePosition(QTextCursor::StartOfBlock);
        ptedt->setTextCursor(tc);
    }
    else if ( !text.mid( tc.positionInBlock() ).contains(rx) )
    {
        tc.setPosition(tc.position() + 1);
        ptedt->setTextCursor(tc);
    }
    else
    {
        q_func()->insertText(" ");
    }
    tc.endEditBlock();
}

void BCodeEditPrivate::handleTab()
{
    if ( ptedt->isReadOnly() )
        return;
    QTextCursor tc = ptedt->textCursor();
    tc.beginEditBlock();
    if (hasSelection)
        deleteSelection();
    tc = ptedt->textCursor();
    QString text = tc.block().text();
    static QRegExp rx("\\S");
    int posb = tc.positionInBlock();
    int spcount = tabWidth - (posb < tabWidth ? posb : posb % tabWidth);
    if (!spcount)
        spcount = tabWidth;
    if ( posb == text.length() )
    {
        tc.insertBlock();
        tc.insertText( QString().fill(' ', lineLength) );
        tc.setPosition(tc.block().position() + tabWidth);
        ptedt->setTextCursor(tc);
    }
    else if ( !text.mid( tc.positionInBlock() ).contains(rx) )
    {
        tc.setPosition(tc.position() + spcount);
        ptedt->setTextCursor(tc);
    }
    else
    {
        q_func()->insertText( QString().fill(' ', spcount) );
    }
    tc.endEditBlock();
}

void BCodeEditPrivate::handleBackspace()
{
    if ( ptedt->isReadOnly() )
        return;
    QTextCursor tc = ptedt->textCursor();
    if ( tc.hasSelection() )
        return emitLineSplitted( deleteSelection() );
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
        QString text = appendTrailingSpaces(ptext + removeTrailingSpaces( tb.text() ), lineLength);
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
        return emitLineSplitted( deleteSelection() );
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
    appendTrailingSpaces(&text, lineLength);
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
        return emitLineSplitted( deleteSelection() );
    int posb = tc.positionInBlock();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    if (posb < removeTrailingSpaces(text).length())
    {
        tc.beginEditBlock();
        int pos = tc.position();
        text.remove(posb, 1);
        text.append(' ');
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.insertText(text);
        tc.setPosition(pos);
    }
    else
    {
        QTextBlock tbn = tb.next();
        if (!tbn.isValid())
            return;
        QString ntext = removeTrailingSpaces(tbn.text());
        if (ntext.isEmpty())
        {
            tc.beginEditBlock();
            int pos = tc.position();
            tc.movePosition(QTextCursor::EndOfBlock);
            tc.movePosition(QTextCursor::NextBlock);
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            tc.removeSelectedText();
            tc.deletePreviousChar();
            tc.setPosition(pos);
        }
        else
        {
            text = text.left(posb);
            if (text.length() + ntext.length() > lineLength)
                return;
            tc.beginEditBlock();
            tc.movePosition(QTextCursor::StartOfBlock);
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            tc.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            tc.removeSelectedText();
            q_func()->insertText(appendTrailingSpaces(text + ntext, lineLength));
            tc.setPosition(tc.block().position() + posb);
        }
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
        return emitLineSplitted( deleteSelection() );
    tc.beginEditBlock();
    QString text = tc.block().text();
    int posb = tc.positionInBlock();
    int i = posb;
    while (++i < text.length() && text.at(i) == ' ')
        ;
    while (++i < text.length() && text.at(i) != ' ')
        ;
    text.remove(posb, i - posb);
    appendTrailingSpaces(&text, lineLength);
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
}

void BCodeEditPrivate::handleShiftEnd()
{
    QTextCursor tc = ptedt->textCursor();
    QTextBlock tb = tc.block();
    int d = removeTrailingSpaces( tb.text() ).length() - tc.positionInBlock();
    if (d < 1)
        return;
    tc.setPosition(tc.position() + d, QTextCursor::KeepAnchor);
    ptedt->setTextCursor(tc);
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
        //TODO: Maybe block validity should be checked
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

void BCodeEditPrivate::handleCtrlLeft()
{
    QTextCursor tc = ptedt->textCursor();
    int posb = tc.positionInBlock();
    QTextBlock tb = tc.block();
    QString text = removeTrailingSpaces( tb.text() );
    int bpos = tb.position();
    if (!posb)
    {
        QTextBlock tbp = tb.previous();
        if ( tbp.isValid() )
        {
            tc.setPosition( tbp.position() + removeTrailingSpaces( tbp.text() ).length() );
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
    QString text = removeTrailingSpaces( tb.text() );
    int bpos = tb.position();
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
        QVector<BPlainTextEditExtended::SelectionRange> ranges = ptedt->selectionRanges();
        if (Qt::Key_Left == key || Qt::Key_Right == key)
        {
            QTextCursor tcr = tc;
            foreach (const BPlainTextEditExtended::SelectionRange &range, ranges)
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
            {
                foreach ( int i, bRange(0, sl.size() - 1) )
                    if ( !sl.at(i).isEmpty() )
                        sl[i].remove(0, 1).append(' ');
            }
            else
            {
               foreach ( int i, bRange(0, sl.size() - 1) )
                   if ( !sl.at(i).isEmpty() )
                       sl[i].remove(sl.at(i).length() - 1, 1).prepend(' ');
            }
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
    tc.endEditBlock();
}

/*============================== Public slots ==============================*/

void BCodeEditPrivate::parceTaskFinished()
{
    if (!parceTask)
        return;
    ProcessTextResult res = parceTask->result();
    parceTask->deleteLater();
    parceTask = 0;
    ptedt->setEnabled(true);
    if (highlighter)
        highlighter->setDocument(0);
    ptedt->setPlainText(res.newText);
    QTextCursor tc = ptedt->textCursor();
    tc.movePosition(QTextCursor::Start);
    ptedt->setTextCursor(tc);
    if (highlighter)
        highlighter->setDocument( ptedt->document() );
    ptedt->setFocus();
    setBuisy(false);
    emitLinesSplitted(res.splittedLinesRanges);
}

void BCodeEditPrivate::popupMenu(const QPoint &pos)
{
    B_Q(BCodeEdit);
    QMenu *menu = new QMenu;
    QAction *act = new QAction(menu);
      act->setText( tr("Undo", "act text") );
      act->setShortcut(QKeySequence::Undo);
      act->setEnabled( q->isUndoAvailable() );
      connect( act, SIGNAL( triggered() ), q, SLOT( undo() ) );
    menu->addAction(act);
    act = new QAction(menu);
      act->setText( tr("Redo", "act text") );
      act->setShortcut(QKeySequence::Redo);
      act->setEnabled( q->isRedoAvailable() );
      connect( act, SIGNAL( triggered() ), q, SLOT( redo() ) );
    menu->addAction(act);
    menu->addSeparator();
    act = new QAction(menu);
      act->setText( tr("Cut", "act text") );
      act->setShortcut(QKeySequence::Cut);
      act->setEnabled( q->isCutAvailable() );
      connect( act, SIGNAL( triggered() ), q, SLOT( cut() ) );
    menu->addAction(act);
    act = new QAction(menu);
      act->setText( tr("Copy", "act text") );
      act->setShortcut(QKeySequence::Copy);
      act->setEnabled( q->isCopyAvailable() );
      connect( act, SIGNAL( triggered() ), q, SLOT( copy() ) );
    menu->addAction(act);
    act = new QAction(menu);
      act->setText( tr("Paste", "act text") );
      act->setShortcut(QKeySequence::Paste);
      act->setEnabled( q->isPasteAvailable() );
      connect( act, SIGNAL( triggered() ), q, SLOT( paste() ) );
    menu->addAction(act);
    act = new QAction(menu);
      act->setText( tr("Delete", "act text") );
      act->setShortcut(QKeySequence::Delete);
      act->setEnabled( q->hasSelection() && !q->isReadOnly() );
      connect( act, SIGNAL( triggered() ), q, SLOT( deleteSelection() ) );
    menu->addAction(act);
    menu->addSeparator();
    act = new QAction(menu);
      act->setText( tr("Select all", "act text") );
      act->setShortcut(QKeySequence::SelectAll);
      connect( act, SIGNAL( triggered() ), q, SLOT( selectAll() ) );
    menu->addAction(act);
    menu->exec( ptedt->mapToGlobal(pos) );
    menu->deleteLater();
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
        QMetaObject::invokeMethod( q_func(), "hasSelectionChanged", Q_ARG(bool, hasSelection) );
}

void BCodeEditPrivate::updateCopyAvailable(bool available)
{
    bool b1 = !ptedt->isReadOnly() && copyAvailable;
    bool b2 = copyAvailable;
    copyAvailable = available;
    if ( b1 != (!ptedt->isReadOnly() && copyAvailable) )
        QMetaObject::invokeMethod( q_func(), "cutAvailableChanged",
                                   Q_ARG(bool, !ptedt->isReadOnly() && copyAvailable) );
    if (b2 != copyAvailable)
        QMetaObject::invokeMethod( q_func(), "copyAvailableChanged", Q_ARG(bool, copyAvailable) );
}

void BCodeEditPrivate::updatePasteAvailable(bool available)
{
    bool b = !ptedt->isReadOnly() && pasteAvailable;
    pasteAvailable = available;
    if (b != pasteAvailable)
        QMetaObject::invokeMethod( q_func(), "pasteAvailableChanged",
                                   Q_ARG(bool, !ptedt->isReadOnly() && pasteAvailable) );
}

void BCodeEditPrivate::updateUndoAvailable(bool available)
{
    bool b = !ptedt->isReadOnly() && undoAvailable;
    undoAvailable = available;
    if (b != undoAvailable)
        QMetaObject::invokeMethod( q_func(), "undoAvailableChanged",
                                   Q_ARG(bool, !ptedt->isReadOnly() && undoAvailable) );
}

void BCodeEditPrivate::updateRedoAvailable(bool available)
{
    bool b = !ptedt->isReadOnly() && redoAvailable;
    redoAvailable = available;
    if (b != redoAvailable)
        QMetaObject::invokeMethod( q_func(), "redoAvailableChanged",
                                   Q_ARG(bool, !ptedt->isReadOnly() && redoAvailable) );
}

void BCodeEditPrivate::emitModificationChanged(bool modified)
{
    QMetaObject::invokeMethod( q_func(), "modificationChanged", Q_ARG(bool, modified) );
}

void BCodeEditPrivate::emitSelectionChanged()
{
    QMetaObject::invokeMethod(q_func(), "selectionChanged");
}

void BCodeEditPrivate::setTextToEmptyLine()
{
    ptedt->setPlainText( QString().fill(' ', lineLength) );
    QTextCursor tc = ptedt->textCursor();
    tc.movePosition(QTextCursor::Start);
    ptedt->setTextCursor(tc);
}

/*============================================================================
================================ BCodeEdit ===================================
============================================================================*/

/*============================== Public constructors =======================*/

BCodeEdit::BCodeEdit(QWidget *parent) :
    QWidget(parent), BBase( *new BCodeEditPrivate(this) )
{
    d_func()->init();
}

BCodeEdit::~BCodeEdit()
{
    //
}

/*============================== Protected constructors ====================*/

BCodeEdit::BCodeEdit(BCodeEditPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

void BCodeEdit::setBlockComment(QTextBlock block, int start, int end)
{
    BTextBlockUserData *ud = dynamic_cast<BTextBlockUserData *>( block.userData() );
    if (!ud)
    {
        if (start < 0)
            return;
        ud = new BTextBlockUserData(start, end);
    }
    else
    {
        ud->skipFrom = start;
        ud->skipTo = end;
    }
    block.setUserData(ud);
}

/*============================== Public methods ============================*/

void BCodeEdit::setReadOnly(bool ro)
{
    B_D(BCodeEdit);
    if (d->ptedt->isReadOnly() == ro)
        return;
    d->ptedt->setReadOnly(ro);
    emit readOnlyChanged(ro);
    d->updateCopyAvailable(d->copyAvailable);
    d->updatePasteAvailable(d->pasteAvailable);
    d->updateUndoAvailable(d->undoAvailable);
    d->updateRedoAvailable(d->redoAvailable);
}

void BCodeEdit::setModification(bool modified)
{
    d_func()->ptedt->document()->setModified(modified);
}

void BCodeEdit::setEditFont(const QFont &fnt)
{
    if ( !QFontInfo(fnt).fixedPitch() )
        return;
    if (fnt.pointSize() < 1 && fnt.pixelSize() < 1)
        return;
    B_D(BCodeEdit);
    QFont fntx = d->ptedt->font();
    //Workaround to set font correctly (assigning fnt directly doesn't work)
    fntx.setFamily( fnt.family() );
    fntx.setPointSize( fnt.pointSize() );
    d->ptedt->setFont(fntx);
}

void BCodeEdit::setEditMode(EditMode mode)
{
    B_D(BCodeEdit);
    bool b = (BlockMode == mode);
    if (b == d->blockMode)
        return;
    d->blockMode = b;
    d->ptedt->setBlockMode(b);
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
    d->setText(text, -1);
    d->ptedt->document()->setModified(pm);
    setFocus();
}

void BCodeEdit::setEditTabWidth(TabWidth tw)
{
    B_D(BCodeEdit);
    if (tw == d->tabWidth)
        return;
    d->tabWidth = tw;
}

void BCodeEdit::setHighlighter(QSyntaxHighlighter *hl)
{;
    B_D(BCodeEdit);
    if (d->highlighter)
        d->highlighter->deleteLater();
    d->highlighter = hl;
    if (d->highlighter)
        d->highlighter->setDocument( d->ptedt->document() );
}

void BCodeEdit::setRecognizedBrackets(const QList<BracketPair> &list)
{
    B_D(BCodeEdit);
    if ( BCodeEditPrivate::testBracketPairListsEquality(d->recognizedBrackets, list) )
        return;
    d->recognizedBrackets = list;
    d->highlightBrackets();
}

void BCodeEdit::setBracketHighlightingEnabled(bool enabled)
{
    B_D(BCodeEdit);
    if (enabled == d->bracketsHighlighting)
        return;
    d->bracketsHighlighting = enabled;
    d->highlightBrackets();
}

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

QSyntaxHighlighter *BCodeEdit::highlighter() const
{
    return d_func()->highlighter;
}

QList<BCodeEdit::BracketPair> BCodeEdit::recognizedBrackets() const
{
    return d_func()->recognizedBrackets;
}

bool BCodeEdit::isBracketHighlightingEnabled() const
{
    return d_func()->bracketsHighlighting;
}

QPoint BCodeEdit::cursorPosition() const
{
    return d_func()->cursorPosition;
}

QString BCodeEdit::text() const
{
    const B_D(BCodeEdit);
    return BCodeEditPrivate::removeTrailingSpaces( d->ptedt->toPlainText().replace(QChar::ParagraphSeparator, '\n') );
}

QPoint BCodeEdit::selectionStart() const
{
    const B_D(BCodeEdit);
    if ( !hasSelection() )
        return cursorPosition();
    QTextCursor tc = d->ptedt->textCursor();
    QTextBlock tb = d->ptedt->document()->findBlock( tc.selectionStart() );
    return QPoint( tc.selectionStart() - tb.position(), tb.blockNumber() );
}

QPoint BCodeEdit::selectionEnd() const
{
    const B_D(BCodeEdit);
    if ( !hasSelection() )
        return cursorPosition();
    QTextCursor tc = d->ptedt->textCursor();
    QTextBlock tb = d->ptedt->document()->findBlock( tc.selectionEnd() );
    return QPoint( tc.selectionEnd() - tb.position(), tb.blockNumber() );
}

QString BCodeEdit::selectedText() const
{
    const B_D(BCodeEdit);
    QTextCursor tc = d->ptedt->textCursor();
    if ( !tc.hasSelection() )
        return "";
    if (!d->blockMode)
    {
        QString txt = tc.selectedText().replace(QChar::ParagraphSeparator, '\n');
        QString txtx = BCodeEditPrivate::removeTrailingSpaces(txt);
        return !txtx.isEmpty() ? txtx : txt;
    }
    QStringList lines;
    foreach ( const BPlainTextEditExtended::SelectionRange &range, d->ptedt->selectionRanges() )
    {
        QTextBlock tb = d->ptedt->document()->findBlock(range.start);
        int offset = tb.position();
        lines << tb.text().mid(range.start - offset, range.end - range.start);
    }
    return lines.join("\n");
}

bool BCodeEdit::isBuisy() const
{
    return d_func()->buisy;
}

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
    QString ptext = text();
    int count = ptext.count(txt, cs);
    if (!count)
        return 0;
    int find = ptext.indexOf(txt, 0, cs);
    int lind = ptext.lastIndexOf(txt, -1, cs) + txt.length();
    selectText( QPoint(find % d->lineLength, find / d->lineLength),
                QPoint(lind % d->lineLength, lind / d->lineLength) );
    insertText( ptext.mid(find, lind - find).replace(txt, newText, cs) );
    return count;
}

/*============================== Public slots ==============================*/

void BCodeEdit::setFocus()
{
    d_func()->ptedt->setFocus();
}

void BCodeEdit::activateWindow()
{
    d_func()->ptedt->activateWindow();
}

void BCodeEdit::setText(const QString &txt, int asyncIfLongerThan)
{
    if ( isBuisy() )
        return;
    d_func()->setText(txt, asyncIfLongerThan);
    setFocus();
}

void BCodeEdit::switchMode()
{
    setEditMode(editMode() == NormalMode ? BlockMode : NormalMode);
}

void BCodeEdit::insertText(const QString &txt)
{
    if ( isBuisy() )
        return;
    B_D(BCodeEdit);
    d->setBuisy(true);
    QTextCursor tc = d->ptedt->textCursor();
    tc.beginEditBlock();
    QList<SplittedLinesRange> ranges;
    SplittedLinesRange range = d->deleteSelection();
    if (range.firstLineNumber >= 0)
        ranges << range;
    if ( txt.isEmpty() )
    {
        tc.endEditBlock();
        return d->setBuisy(false);
    }
    tc = d->ptedt->textCursor();
    int posb = tc.positionInBlock();
    QString btext = tc.block().text();
    int lind = BCodeEditPrivate::removeTrailingSpaces(btext).lastIndexOf(' ');
    int tcpos = ( lind >= posb && (d->lineLength - lind) >= txt.length() ) ? ( tc.position() + txt.length() ) : -1;
    QString ltext = btext.left(posb);
    QString rtext = BCodeEditPrivate::removeTrailingSpaces( btext.right(btext.length() - posb) );
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
    if (b)
    {
        //TODO (release 3.0.0): Improve text insertion in block mode
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
            BCodeEditPrivate::appendTrailingSpaces(&t, d->lineLength);
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
            BCodeEditPrivate::appendTrailingSpaces(&t, d->lineLength);
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
        //Workaround for lines containing spaces only
        foreach ( int i, bRange(0, sl.size() - 1) )
        {
            QString l = BCodeEditPrivate::removeTrailingSpaces( sl.at(i) );
            if ( !l.isEmpty() )
                sl[i] = l;
        }
        //End of the workaround
        sl.first().prepend(ltext);
        sl.last().append(rtext);
        BCodeEditPrivate::ProcessTextResult res = BCodeEditPrivate::processText(sl.join("\n"), d->lineLength,
                                                                                d->tabWidth);
        int boffset = tc.block().blockNumber();
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        int posmod = 0;
        //Workaround for correct text wrapping
        QString nbt;
        if (tc.block().next().isValid() && res.newText.length() <= d->lineLength * 2 + 1)
            nbt = BCodeEditPrivate::removeTrailingSpaces( tc.block().next().text() );
        QStringList ntl;
        if ( !nbt.isEmpty() )
            ntl = res.newText.split('\n');
        if (ntl.size() > 1 && !sl.last().isEmpty())
        {
            QStringList ntl = res.newText.split('\n');
            if (nbt.length() + BCodeEditPrivate::removeTrailingSpaces( ntl.last() ).length() < d->lineLength)
            {
                BCodeEditPrivate::removeTrailingSpaces( &ntl.last() );
                ntl.last() += nbt;
                posmod -= nbt.length();
                BCodeEditPrivate::appendTrailingSpaces(&ntl.last(), d->lineLength);
                res.newText = ntl.join("\n");
                tc.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
                tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            }
        }
        //End of the workaround
        tc.insertText(res.newText);
        int pos = d->lineLength;
        QString btext = tc.block().text();
        while (pos > 0 && btext.at(pos - 1) == ' ')
            --pos;
        if (pos > 0)
            pos -= rtext.length();
        if ( !res.splittedLinesRanges.isEmpty() )
        {
            if (ltext.length() + txt.length() <= d->lineLength)
                pos -= 1;
            ranges = res.splittedLinesRanges;
            for (int i = 0; i < ranges.size(); ++i)
            {
                ranges[i].firstLineNumber += boffset;
                ranges[i].lastLineNumber += boffset;
            }
        }
        if (tcpos > 0)
            tc.setPosition(tcpos);
        else
            tc.setPosition(tc.block().position() + pos + posmod);
    }
    d->ptedt->setTextCursor(tc);
    if (d->highlighter)
        d->highlighter->rehighlight();
    tc.endEditBlock();
    d->setBuisy(false);
    d->emitLinesSplitted(ranges);
    setFocus();
    d->ptedt->ensureCursorVisible();
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
    setFocus();
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
    setFocus();
}

void BCodeEdit::selectText(int firstLine, int lastLine)
{
    B_D(BCodeEdit);
    int bc = d_func()->ptedt->blockCount();
    if ( firstLine < 0 || firstLine >= bc)
        return;
    QPoint start(0, firstLine);
    QPoint end(d->lineLength, firstLine);
    if (lastLine > 0 && lastLine < bc)
        end.setY(lastLine);
    selectText(start, end);
}

void BCodeEdit::selectAll()
{
    d_func()->ptedt->selectAll();
    setFocus();
}

void BCodeEdit::deselectText()
{
    B_D(BCodeEdit);
    QTextCursor tc = d->ptedt->textCursor();
    if ( !tc.hasSelection() )
        return;
    tc.setPosition( tc.selectionEnd() );
    d->ptedt->setTextCursor(tc);
    setFocus();
}

void BCodeEdit::cut()
{
    if ( isReadOnly() || !hasSelection() )
        return;
    copy();
    d_func()->deleteSelection();
    setFocus();
}

void BCodeEdit::copy()
{
    QString text = selectedText();
    if ( text.isEmpty() )
        return;
    QApplication::clipboard()->setText(text);
    setFocus();
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

void BCodeEdit::deleteSelection()
{
    if ( isReadOnly() || !hasSelection() )
        return;
    d_func()->deleteSelection();
    setFocus();
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
    setFocus();
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
    setFocus();
}

/*============================== Protected methods =========================*/

BPlainTextEdit *BCodeEdit::innerEdit() const
{
    return d_func()->ptedt;
}

/*============================================================================
================================ BCodeEditParseTask ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BCodeEditParseTask::BCodeEditParseTask(const QString &text, int lineLength, BCodeEdit::TabWidth tabWidth) :
    QObject(0), Text(text), LineLength(lineLength), TabWidth(tabWidth)
{
    //
}

BCodeEditParseTask::~BCodeEditParseTask()
{
    //
}

/*============================== Static public methods =====================*/

QThreadPool *BCodeEditParseTask::pool()
{
    if (!tp)
        tp = new QThreadPool;
    return tp;
}

/*============================== Public methods ============================*/

void BCodeEditParseTask::run()
{
    res = BCodeEditPrivate::processText(Text, LineLength, TabWidth);
    emit finished();
}

BCodeEditPrivate::ProcessTextResult BCodeEditParseTask::result() const
{
    return res;
}

/*============================== Static private variables ==================*/

QThreadPool *BCodeEditParseTask::tp = 0;
