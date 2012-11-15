#include "bplaintextedit.h"
#include "bplaintextedit_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QPlainTextEdit>
#include <QWidget>
#include <QMimeData>
#include <QTextCursor>
#include <QTextDocument>
#include <QPainter>
#include <QRectF>
#include <QBrush>
#include <QTransform>
#include <QGradient>
#include <QPlainTextEdit>
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
#include <QTextBlock>
#include <QVector>
#include <QFont>
#include <QKeyEvent>
#include <QEvent>
#include <QApplication>

#include <QDebug>

BPlainTextEditPrivateObject::BPlainTextEditPrivateObject(BPlainTextEditPrivate *p) :
    QObject(0), _m_p(p)
{
    //
}

BPlainTextEditPrivateObject::~BPlainTextEditPrivateObject()
{
    //
}

//

void BPlainTextEditPrivateObject::cursorPositionChanged(const QTextCursor &cursor)
{
    _m_p->cursorPositionChanged(cursor);
}

void BPlainTextEditPrivateObject::contentsChange(int position, int charsRemoved, int charsAdded)
{
    _m_p->contentsChange(position, charsRemoved, charsAdded);
}

void BPlainTextEditPrivateObject::selectionChanged()
{
    _m_p->selectionChanged();
}

//

void BPlainTextEditPrivate::fillBackground(QPainter *painter, const QRectF &rect, QBrush brush, QRectF gradientRect)
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

//

BPlainTextEditPrivate::BPlainTextEditPrivate(BPlainTextEdit *q) :
    BBasePrivate(q), _m_o( new BPlainTextEditPrivateObject(this) )
{
    drag = true;
    blockMode = false;
    hasSelection = false;
    lineLength = 20; //Just for test
    q->setFont( QFont("DejaVu Sans Mono") ); //Just for test
    QObject::connect( q->document(), SIGNAL( cursorPositionChanged(QTextCursor) ),
                      _m_o, SLOT( cursorPositionChanged(QTextCursor) ) );
    QObject::connect( q->document(), SIGNAL( contentsChange(int, int, int) ),
                      _m_o, SLOT( contentsChange(int, int, int) ) );
    QObject::connect( q, SIGNAL( selectionChanged() ), _m_o, SLOT( selectionChanged() ) );
}

BPlainTextEditPrivate::~BPlainTextEditPrivate()
{
    _m_o->deleteLater();
}

//

void BPlainTextEditPrivate::cursorPositionChanged(const QTextCursor &cursor)
{
    //
}

void BPlainTextEditPrivate::contentsChange(int position, int charsRemoved, int charsAdded)
{
    //
}

void BPlainTextEditPrivate::selectionChanged()
{
    B_Q(BPlainTextEdit);
    QTextCursor tc = q->textCursor();
    hasSelection = tc.hasSelection();
    if (!blockMode)
        return;
    selectionRanges.clear();
    if (!hasSelection)
        return;
    //Assuming all lines have the same length
    int start = tc.selectionStart();
    int end = tc.selectionEnd();
    int soffset = start - q->document()->findBlock(start).position();
    int eoffset = end - q->document()->findBlock(end).position();
    if (soffset == eoffset) //Seems to be odd, such a situation is hardly possible
        return;
    int minoffset = qMin<int>(soffset, eoffset);
    int maxoffset = qMax<int>(soffset, eoffset);
    int astart = qMin<int>(start, end);
    int aend = qMax<int>(start, end);
    QTextBlock tb = q->document()->findBlock(astart);
    SelectionRange sr;
    while ( tb.isValid() )
    {
        int bpos = tb.position();
        if (bpos > aend)
            break;
        sr.start = bpos + minoffset;
        sr.end = bpos + maxoffset;
        selectionRanges.append(sr);
        tb = tb.next();
    }
    //Workaround
    QKeyEvent e(QKeyEvent::KeyPress, Qt::Key_Shift, Qt::NoModifier);
    QApplication::sendEvent(q, &e);
}

QAbstractTextDocumentLayout::PaintContext BPlainTextEditPrivate::getPaintContext() const
{
    if (!blockMode || !hasSelection)
        return q_func()->getPaintContext();
    QAbstractTextDocumentLayout::PaintContext context = q_func()->getPaintContext();
    QAbstractTextDocumentLayout::Selection sel = context.selections.last();
    context.selections.remove(context.selections.size() - 1);
    foreach (const SelectionRange &sr, selectionRanges)
    {
        sel.cursor.setPosition(sr.start);
        sel.cursor.setPosition(sr.end, QTextCursor::KeepAnchor);
        context.selections.append(sel);
    }
    return context;
}

//

BPlainTextEdit::BPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent), BBase( *new BPlainTextEditPrivate(this) )
{
    //
}

BPlainTextEdit::~BPlainTextEdit()
{
    //
}

//

void BPlainTextEdit::setDragEnabled(bool b)
{
    d_func()->drag = b;
}

void BPlainTextEdit::setMode(Mode mode)
{
    d_func()->blockMode = (BlockMode == mode);
}

void BPlainTextEdit::setLineLength(int length)
{
    if (length < 10) //Just for test
        return;
    d_func()->lineLength = length;
}

bool BPlainTextEdit::dragEnabled() const
{
    return d_func()->drag;
}

BPlainTextEdit::Mode BPlainTextEdit::mode() const
{
    return d_func()->blockMode ? BlockMode : NormalMode;
}

int BPlainTextEdit::lineLength() const
{
    return d_func()->lineLength;
}

//

BPlainTextEdit::BPlainTextEdit(BPlainTextEditPrivate &d, QWidget *parent) :
    QPlainTextEdit(parent), BBase(d)
{
    //
}

//

bool BPlainTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    return QPlainTextEdit::canInsertFromMimeData(source);
}

QMimeData *BPlainTextEdit::createMimeDataFromSelection() const
{
    return d_func()->drag ? QPlainTextEdit::createMimeDataFromSelection() : 0;
}

void BPlainTextEdit::insertFromMimeData(const QMimeData *source)
{
    QPlainTextEdit::insertFromMimeData(source);
}

void BPlainTextEdit::paintEvent(QPaintEvent *e)
{
    B_D(BPlainTextEdit);
    if (!d->blockMode || !d->hasSelection)
        return QPlainTextEdit::paintEvent(e);
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
    QAbstractTextDocumentLayout::PaintContext context = d->getPaintContext(); //my paint context
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
                BPlainTextEditPrivate::fillBackground(&painter, contentsRect, bg);
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
