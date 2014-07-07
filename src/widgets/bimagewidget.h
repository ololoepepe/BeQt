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

#ifndef BIMAGEWIDGET_H
#define BIMAGEWIDGET_H

class BImageWidgetPrivate;

class QIcon;
class QImage;
class QPicture;
class QPixmap;
class QString;

#include <BeQtCore/BBase>

#include <QWidget>

/*============================================================================
================================ BImageWidget ================================
============================================================================*/

class B_WIDGETS_EXPORT BImageWidget : public QWidget, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BImageWidget)
public:
    explicit BImageWidget(QWidget *parent = 0);
    explicit BImageWidget(const QPixmap &pixmap, QWidget *parent = 0);
    explicit BImageWidget(const QPicture &picture, QWidget *parent = 0);
    explicit BImageWidget(const QImage &image, QWidget *parent = 0);
    explicit BImageWidget(const QIcon &icon, QWidget *parent = 0);
    explicit BImageWidget(const QString &fileName, QWidget *parent = 0);
    ~BImageWidget();
protected:
    explicit BImageWidget(BImageWidgetPrivate &d, QWidget *parent = 0);
public:
    bool hasScaledContents() const;
    QIcon icon() const;
    QImage image() const;
    QString imageFileName() const;
    QPicture picture() const;
    QPixmap pixmap() const;
public Q_SLOTS:
    void setIcon(const QIcon &icn);
    void setImage(const QImage &img);
    void setImage(const QString &fileName);
    void setPicture(const QPicture &p);
    void setPixmap(const QPixmap &p);
    void setScaledContents(bool b);
private:
    Q_DISABLE_COPY(BImageWidget)
};

#endif // BIMAGEWIDGET_H
