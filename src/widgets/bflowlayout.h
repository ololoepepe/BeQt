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

#ifndef BFLOWLAYOUT_H
#define BFLOWLAYOUT_H

class BFlowLayoutPrivate;

class QLayoutItem;
class QRect;
class QSize;
class QWidget;

#include <BeQtCore/BBase>

#include <QLayout>

/*============================================================================
================================ BFlowLayout =================================
============================================================================*/

class B_WIDGETS_EXPORT BFlowLayout : public QLayout, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BFlowLayout)
public:
    explicit BFlowLayout(QWidget *parent, int hSpacing = -1, int vSpacing = -1);
    explicit BFlowLayout(int hSpacing = -1, int vSpacing = -1);
    ~BFlowLayout();
protected:
    explicit BFlowLayout(BFlowLayoutPrivate &d, QWidget *parent = 0);
public:
    void addItem(QLayoutItem *item);
    int count() const;
    Qt::Orientations expandingDirections() const;
    bool hasHeightForWidth() const;
    int heightForWidth(int width) const;
    int horizontalSpacing() const;
    QLayoutItem *itemAt(int index) const;
    QSize minimumSize() const;
    void setGeometry(const QRect &rect);
    QSize sizeHint() const;
    QLayoutItem *takeAt(int index);
    int verticalSpacing() const;
private:
    Q_DISABLE_COPY(BFlowLayout)
};

#endif // BFLOWLAYOUT_H
