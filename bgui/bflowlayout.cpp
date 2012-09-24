#include "bflowlayout.h"

#include <QLayout>
#include <QRect>
#include <QList>
#include <QStyle>
#include <QSize>
#include <QLayoutItem>
#include <QWidget>
#include <QObject>

BFlowLayout::BFlowLayout(QWidget *parent, int hSpacing, int vSpacing) :
    QLayout(parent), _m_hSpace(hSpacing), _m_vSpace(vSpacing)
{
}

BFlowLayout::BFlowLayout(int hSpacing, int vSpacing) :
    _m_hSpace(hSpacing), _m_vSpace(vSpacing)
{
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
    _m_items << item;
}

void BFlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    _m_doLayout(rect, false);
}

int BFlowLayout::horizontalSpacing() const
{
    return (_m_hSpace >= 0) ? _m_hSpace : _m_smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int BFlowLayout::verticalSpacing() const
{
    return (_m_vSpace >= 0) ? _m_vSpace : _m_smartSpacing(QStyle::PM_LayoutVerticalSpacing);
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
    return _m_doLayout(QRect(0, 0, width, 0), true);
}

int BFlowLayout::count() const
{
    return _m_items.size();
}

QLayoutItem *BFlowLayout::itemAt(int index) const
{
    return _m_items.value(index);
}

QLayoutItem *BFlowLayout::takeAt(int index)
{
    return ( index >= 0 && index < _m_items.size() ) ? _m_items.takeAt(index) : 0;
}

QSize BFlowLayout::minimumSize() const
{
    QSize size;
    QLayoutItem *item;
    foreach (item, _m_items)
        size = size.expandedTo( item->minimumSize() );
    size += QSize( 2 * margin(), 2 * margin() );
    return size;
}

QSize BFlowLayout::sizeHint() const
{
    return minimumSize();
}

//

int BFlowLayout::_m_doLayout(const QRect &rect, bool testOnly) const
{
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;
    QLayoutItem *item;
    foreach (item, _m_items)
    {
        QWidget *wid = item->widget();
        int spaceX = horizontalSpacing();
        if (spaceX == -1)
            spaceX = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        int spaceY = verticalSpacing();
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

int BFlowLayout::_m_smartSpacing(QStyle::PixelMetric pm) const
{
    QObject *parent = this->parent();
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
