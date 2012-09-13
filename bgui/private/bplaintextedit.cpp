class BTextEditorDocument;

class QWidget;

#include "bplaintextedit.h"

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
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QTextLayout>
#include <QTextBlockFormat>
#include <QVector>
#include <QTextLine>
#include <QScrollBar>
#include <QMimeData>

static void fillBackground( QPainter *painter, const QRectF &rect, QBrush brush, QRectF gradientRect = QRectF() )
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

BPlainTextEdit::BPlainTextEdit(BTextEditorDocument *editorDocument, QWidget *parent) :
    QPlainTextEdit(parent), _m_CDocument(editorDocument)
{
}

//

BTextEditorDocument *BPlainTextEdit::editorDocument() const
{
    return _m_CDocument;
}

//

void BPlainTextEdit::paintEvent(QPaintEvent *event)
{
    QPainter painter( viewport() );
    Q_ASSERT( qobject_cast<QPlainTextDocumentLayout*>( document()->documentLayout() ) );
    QPointF offset( contentOffset() );
    QRect er = event->rect();
    QRect viewportRect = viewport()->rect();
    bool editable = !isReadOnly();
    QTextBlock block = firstVisibleBlock();
    qreal maximumWidth = document()->documentLayout()->documentSize().width();
    painter.setBrushOrigin(offset);
    int maxX = offset.x() + qMax( (qreal) viewportRect.width(), maximumWidth ) - document()->documentMargin();
    er.setRight( qMin(er.right(), maxX) );
    painter.setClipRect(er);
    QAbstractTextDocumentLayout::PaintContext context = getPaintContext();
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
                fillBackground(&painter, contentsRect, bg);
            }
            QVector<QTextLayout::FormatRange> selections;
            int blpos = block.position();
            int bllen = block.length();
            for (int i = 0; i < context.selections.size(); ++i)
            {
                const QAbstractTextDocumentLayout::Selection &range = context.selections.at(i);
                const int selStart = range.cursor.selectionStart() - blpos;
                const int selEnd = range.cursor.selectionEnd() - blpos;
                if ( range.format.background().color().isValid() ) //my check
                {
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
                else if ( palette().text().color().isValid() ) //my check
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

QMimeData *BPlainTextEdit::createMimeDataFromSelection() const
{
    return 0;
}
