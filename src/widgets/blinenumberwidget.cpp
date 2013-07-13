#include "blinenumberwidget.h"
#include "blinenumberwidget_p.h"

#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QWidget>
#include <QPlainTextEdit>
#include <QSize>
#include <QLatin1Char>
#include <QPaintEvent>
#include <QPainter>
#include <QTextBlock>
#include <QRect>
#include <QRectF>
#include <QPointF>
#include <QString>
#include <QEvent>
#include <QTimer>

#include <QDebug>

/*============================================================================
================================ BSettingsDialogPrivate ======================
============================================================================*/

/*============================== Public constructors =======================*/

BLineNumberWidgetPrivate::BLineNumberWidgetPrivate(BLineNumberWidget *q, QPlainTextEdit *edit) :
    BBasePrivate(q), Edit(edit)
{
    //
}

BLineNumberWidgetPrivate::~BLineNumberWidgetPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BLineNumberWidgetPrivate::init()
{
    if (!Edit)
        return;
    Edit->installEventFilter(this);
    connect(Edit, SIGNAL(blockCountChanged(int)), this, SLOT(updateWidth()));
    connect(Edit, SIGNAL(updateRequest(QRect, int)), this, SLOT(update(QRect, int)));
    updateWidth();
}

bool BLineNumberWidgetPrivate::eventFilter(QObject *o, QEvent *e)
{
    if (o != Edit || e->type() != QEvent::Resize)
        return false;
    QTimer::singleShot(10, this, SLOT(resize()));
    return false;
}

int BLineNumberWidgetPrivate::getWidth() const
{
    if (!Edit)
        return 0;
    int digits = 1;
    int max = qMax(1, Edit->blockCount());
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }
    int space = 3 + Edit->fontMetrics().width(QLatin1Char('9')) * digits;
    return space;
}

bool BLineNumberWidgetPrivate::paintEvent(QPaintEvent *e)
{
    struct EditHack : QPlainTextEdit
    {
        using QPlainTextEdit::blockBoundingGeometry;
        using QPlainTextEdit::contentOffset;
        using QPlainTextEdit::firstVisibleBlock;
    public:
        static QRectF blockBoundingGeometryHack(const QPlainTextEdit *edit, const QTextBlock &block)
        {
            return (edit->*&EditHack::blockBoundingGeometry)(block);
        }
        static QPointF contentOffsetHack(const QPlainTextEdit *edit)
        {
            return (edit->*&EditHack::contentOffset)();
        }
        static QTextBlock firstVisibleBlockHack(const QPlainTextEdit *edit)
        {
            return (edit->*&EditHack::firstVisibleBlock)();
        }
    };
    if (!Edit)
        return false;
    QPainter painter(q_func());
    painter.fillRect(e->rect(), Qt::lightGray);
    QTextBlock block = Edit->textCursor().block();
    QPointF offset = EditHack::contentOffsetHack(Edit);
    if (block.isVisible())
    {
        QRect r = e->rect();
        QRectF g = EditHack::blockBoundingGeometryHack(Edit, block);
        r.setTop((int) g.translated(offset).top());
        r.setBottom(r.top() + (int) g.height());
        painter.fillRect(r, Qt::yellow);
    }
    block = EditHack::firstVisibleBlockHack(Edit);
    int blockNumber = block.blockNumber();
    QRectF g = EditHack::blockBoundingGeometryHack(Edit, block);
    int top = (int) g.translated(offset).top();
    int bottom = top + (int) g.height();
    while (block.isValid() && top <= e->rect().bottom())
    {
        if (block.isVisible() && bottom >= e->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, q_func()->width(), Edit->fontMetrics().height(), Qt::AlignRight, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + (int) EditHack::blockBoundingGeometryHack(Edit, block).height();
        ++blockNumber;
    }
    return true;
}

/*============================== Public slots ==============================*/

void BLineNumberWidgetPrivate::updateWidth()
{
    struct EditHack : QPlainTextEdit
    {
    public:
        static void setViewportMarginsHack(QPlainTextEdit *edit, int left, int top, int right, int bottom)
        {
            reinterpret_cast<EditHack *>(edit)->setViewportMargins(left, top, right, bottom);
        }
    };
    if (!Edit)
        return;
    EditHack::setViewportMarginsHack(Edit, getWidth(), 0, 0, 0);
    resize();
}

void BLineNumberWidgetPrivate::update(const QRect &rect, int dy)
{
    B_Q(BLineNumberWidget);
    if (dy)
        q->scroll(0, dy);
    else
        q->update(0, rect.y(), q->width(), rect.height());
    if (Edit && rect.contains(Edit->viewport()->rect()))
        updateWidth();
}

void BLineNumberWidgetPrivate::resize()
{
    QRect cr = Edit->contentsRect();
    q_func()->setGeometry(QRect(cr.left(), cr.top(), getWidth(), cr.height()));
}

/*============================================================================
================================ BSettingsDialog =============================
============================================================================*/

/*============================== Public constructors =======================*/

BLineNumberWidget::BLineNumberWidget(QPlainTextEdit *edit) :
    QWidget(edit), BBase(*new BLineNumberWidgetPrivate(this, edit))
{
    d_func()->init();
}

BLineNumberWidget::~BLineNumberWidget()
{
    //
}

/*============================== Public methods ============================*/

QSize BLineNumberWidget::sizeHint() const
{
    return QSize(d_func()->getWidth(), 0);
}

/*============================== Protected methods =========================*/

void BLineNumberWidget::paintEvent(QPaintEvent *e)
{
    if (!d_func()->paintEvent(e))
        QWidget::paintEvent(e);
}
