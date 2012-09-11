#include "bcontainerwidget.h"

#include <QWidget>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidgetList>

BContainerWidget::BContainerWidget(Qt::Orientation orientation, QWidget *parent) :
    QWidget(parent)
{
    if (Qt::Horizontal == orientation)
        _m_blt = new QHBoxLayout(this);
    else
        _m_blt = new QVBoxLayout(this);
    _m_blt->setContentsMargins(0, 0, 0, 0);
}

//

void BContainerWidget::addWidget(QWidget *widget)
{
    if (!widget)
        return;
    widget->setParent(this);
    _m_blt->addWidget(widget);
}

void BContainerWidget::addWidgets(const QWidgetList &list)
{
    for (int i = 0; i < list.size(); ++i)
        addWidget( list.at(i) );
}

void BContainerWidget::addStretch(int stretch)
{
    _m_blt->addStretch(stretch);
}

void BContainerWidget::removeWidget(QWidget *widget)
{
    _m_blt->removeWidget(widget);
    widget->setParent(0);
}
