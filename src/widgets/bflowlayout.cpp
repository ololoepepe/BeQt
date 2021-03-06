/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtWidgets module of the BeQt library.
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

#include "bflowlayout.h"

#include <BeQtCore/private/bbase_p.h>

#include <QLayout>
#include <QLayoutItem>
#include <QList>
#include <QObject>
#include <QRect>
#include <QSize>
#include <QStyle>
#include <QWidget>

/*============================================================================
================================ BFlowLayoutPrivate ==========================
============================================================================*/

class BFlowLayoutPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BFlowLayout)
public:
    int hSpace;
    QList<QLayoutItem *> items;
    int vSpace;
public:
    explicit BFlowLayoutPrivate(BFlowLayout *q);
    ~BFlowLayoutPrivate();
public:
    int doLayout(const QRect &rect, bool testOnly) const;
    void init();
    int smartSpacing(QStyle::PixelMetric pm) const;
private:
    Q_DISABLE_COPY(BFlowLayoutPrivate)
};

/*============================================================================
================================ BFlowLayoutPrivate ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BFlowLayoutPrivate::BFlowLayoutPrivate(BFlowLayout *q) :
    BBasePrivate(q)
{
    //
}

BFlowLayoutPrivate::~BFlowLayoutPrivate()
{
    //
}

/*============================== Public methods ============================*/

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
    foreach (item, items) {
        QWidget *wid = item->widget();
        int spaceX = q->horizontalSpacing();
        if (spaceX == -1)
            spaceX = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        int spaceY = q->verticalSpacing();
        if (spaceY == -1)
            spaceY = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }
        if (!testOnly)
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
        x = nextX;
        lineHeight = qMax<int>(lineHeight, item->sizeHint().height());
    }
    return y + lineHeight - rect.y() + bottom;
}

void BFlowLayoutPrivate::init()
{
    //
}

int BFlowLayoutPrivate::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject *parent = q_func()->parent();
    if (!parent)
        return -1;
    if (!parent->isWidgetType())
        return static_cast<QLayout *>(parent)->spacing();
    QWidget *pw = static_cast<QWidget *>(parent);
    return pw->style()->pixelMetric(pm, 0, pw);
}

/*============================================================================
================================ BFlowLayout =================================
============================================================================*/

/*============================== Public constructors =======================*/

BFlowLayout::BFlowLayout(QWidget *parent, int hSpacing, int vSpacing) :
    QLayout(parent), BBase(*new BFlowLayoutPrivate(this))
{
    d_func()->init();
    B_D(BFlowLayout);
    d->hSpace = hSpacing;
    d->vSpace = vSpacing;
}

BFlowLayout::BFlowLayout(int hSpacing, int vSpacing) :
    QLayout(0), BBase(*new BFlowLayoutPrivate(this))
{
    d_func()->init();
    B_D(BFlowLayout);
    d->hSpace = hSpacing;
    d->vSpace = vSpacing;
}

BFlowLayout::~BFlowLayout()
{
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}

/*============================== Protected constructors ====================*/

BFlowLayout::BFlowLayout(BFlowLayoutPrivate &d, QWidget *parent) :
    QLayout(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BFlowLayout::addItem(QLayoutItem *item)
{
    d_func()->items << item;
}

int BFlowLayout::count() const
{
    return d_func()->items.size();
}

Qt::Orientations BFlowLayout::expandingDirections() const
{
    return Qt::Orientations(0);
}

bool BFlowLayout::hasHeightForWidth() const
{
    return true;
}

int BFlowLayout::heightForWidth(int width) const
{
    return d_func()->doLayout(QRect(0, 0, width, 0), true);
}

int BFlowLayout::horizontalSpacing() const
{
    const B_D(BFlowLayout);
    return (d->hSpace >= 0) ? d->hSpace : d->smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

QLayoutItem *BFlowLayout::itemAt(int index) const
{
    return d_func()->items.value(index);
}

QSize BFlowLayout::minimumSize() const
{
    QSize size;
    QLayoutItem *item;
    foreach (item, d_func()->items)
        size = size.expandedTo(item->minimumSize());
    size += QSize(2 * margin(), 2 * margin());
    return size;
}

void BFlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    d_func()->doLayout(rect, false);
}

QSize BFlowLayout::sizeHint() const
{
    return minimumSize();
}

QLayoutItem *BFlowLayout::takeAt(int index)
{
    B_D(BFlowLayout);
    return (index >= 0 && (index < d->items.size())) ? d->items.takeAt(index) : 0;
}

int BFlowLayout::verticalSpacing() const
{
    const B_D(BFlowLayout);
    return (d->vSpace >= 0) ? d->vSpace : d->smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}
