#include "bflowlayout.h"

#include <BeQtCore/private/bbase_p.h>

#include <QLayout>
#include <QRect>
#include <QList>
#include <QStyle>
#include <QSize>
#include <QLayoutItem>
#include <QWidget>
#include <QObject>

class BFlowLayoutPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BFlowLayout)
public:
    explicit BFlowLayoutPrivate(BFlowLayout *q);
    ~BFlowLayoutPrivate();
    //
    int doLayout(const QRect &rect, bool testOnly) const;
    int smartSpacing(QStyle::PixelMetric pm) const;
    //
    QList<QLayoutItem *> items;
    int hSpace;
    int vSpace;
private:
    Q_DISABLE_COPY(BFlowLayoutPrivate)
};

//

BFlowLayoutPrivate::BFlowLayoutPrivate(BFlowLayout *q) :
    BBasePrivate(q)
{
    //
}

BFlowLayoutPrivate::~BFlowLayoutPrivate()
{
    //
}

//

int BFlowLayoutPrivate::doLayout(const QRect &rect, bool testOnly) const
{
    const B_Q(BFlowLayout);
    int left, top, right, bottom;
    q->getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;
    QLayoutItem *item;
    foreach (item, items)
    {
        QWidget *wid = item->widget();
        int spaceX = q->horizontalSpacing();
        if (spaceX == -1)
            spaceX = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        int spaceY = q->verticalSpacing();
        if (spaceY == -1)
            spaceY = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0)
        {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }
        if (!testOnly)
            item->setGeometry( QRect( QPoint(x, y), item->sizeHint() ) );
        x = nextX;
        lineHeight = qMax<int>( lineHeight, item->sizeHint().height() );
    }
    return y + lineHeight - rect.y() + bottom;
}

int BFlowLayoutPrivate::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject *parent = q_func()->parent();
    if (!parent)
    {
        return -1;
    }
    else if ( parent->isWidgetType() )
    {
        QWidget *pw = static_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, 0, pw);
    }
    else
    {
        return static_cast<QLayout *>(parent)->spacing();
    }
}

//

BFlowLayout::BFlowLayout(QWidget *parent, int hSpacing, int vSpacing) :
    QLayout(parent), BBase( *new BFlowLayoutPrivate(this) )
{
    B_D(BFlowLayout);
    d->hSpace = hSpacing;
    d->vSpace = vSpacing;
}

BFlowLayout::BFlowLayout(int hSpacing, int vSpacing) :
    QLayout(0), BBase( *new BFlowLayoutPrivate(this) )
{
    B_D(BFlowLayout);
    d->hSpace = hSpacing;
    d->vSpace = vSpacing;
}

BFlowLayout::~BFlowLayout()
{
    QLayoutItem *item;
    while ( ( item = takeAt(0) ) )
        delete item;
}

//

void BFlowLayout::addItem(QLayoutItem *item)
{
    d_func()->items << item;
}

void BFlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    d_func()->doLayout(rect, false);
}

int BFlowLayout::horizontalSpacing() const
{
    const B_D(BFlowLayout);
    return (d->hSpace >= 0) ? d->hSpace : d->smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int BFlowLayout::verticalSpacing() const
{
    const B_D(BFlowLayout);
    return (d->vSpace >= 0) ? d->vSpace : d->smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}

Qt::Orientations BFlowLayout::expandingDirections() const
{
    return 0;
}

bool BFlowLayout::hasHeightForWidth() const
{
    return true;
}

int BFlowLayout::heightForWidth(int width) const
{
    return d_func()->doLayout(QRect(0, 0, width, 0), true);
}

int BFlowLayout::count() const
{
    return d_func()->items.size();
}

QLayoutItem *BFlowLayout::itemAt(int index) const
{
    return d_func()->items.value(index);
}

QLayoutItem *BFlowLayout::takeAt(int index)
{
    B_D(BFlowLayout);
    return ( index >= 0 && index < d->items.size() ) ? d->items.takeAt(index) : 0;
}

QSize BFlowLayout::minimumSize() const
{
    QSize size;
    QLayoutItem *item;
    foreach (item, d_func()->items)
        size = size.expandedTo( item->minimumSize() );
    size += QSize( 2 * margin(), 2 * margin() );
    return size;
}

QSize BFlowLayout::sizeHint() const
{
    return minimumSize();
}
