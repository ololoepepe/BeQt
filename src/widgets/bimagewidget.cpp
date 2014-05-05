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

#include "bimagewidget.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QWidget>
#include <QPixmap>
#include <QPicture>
#include <QImage>
#include <QIcon>
#include <QString>
#include <QLabel>
#include <QMovie>
#include <QVBoxLayout>
#include <QImageReader>
#include <QList>
#include <QByteArray>

/*============================================================================
================================ BImageWidgetPrivate =========================
============================================================================*/

class BImageWidgetPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BImageWidget)
public:
    explicit BImageWidgetPrivate(BImageWidget *q);
    ~BImageWidgetPrivate();
public:
    void init();
    void reset(bool animated);
public:
    QLabel *lbl;
    QMovie *mov;
    QString fileName;
private:
    Q_DISABLE_COPY(BImageWidgetPrivate)
};


/*============================================================================
================================ BImageWidgetPrivate =========================
============================================================================*/

/*============================== Public constructors =======================*/

BImageWidgetPrivate::BImageWidgetPrivate(BImageWidget *q) :
    BBasePrivate(q)
{
    //
}

BImageWidgetPrivate::~BImageWidgetPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BImageWidgetPrivate::init()
{
    QVBoxLayout *vlt = new QVBoxLayout(q_func());
      vlt->setContentsMargins(0, 0, 0, 0);
      lbl = new QLabel;
        mov = new QMovie(this);
      vlt->addWidget(lbl);
}

void BImageWidgetPrivate::reset(bool animated)
{
    if (animated)
    {
        lbl->setPixmap(QPixmap());
        lbl->setMovie(mov);
        mov->start();
    }
    else
    {
        mov->stop();
        mov->setFileName("");
        lbl->setMovie(0);
    }
}

/*============================================================================
================================ BImageWidget ================================
============================================================================*/

/*============================== Public constructors =======================*/

BImageWidget::BImageWidget(QWidget *parent) :
    QWidget(parent), BBase(*new BImageWidgetPrivate(this))
{
    d_func()->init();
}

BImageWidget::BImageWidget(const QPixmap &pixmap, QWidget *parent) :
    QWidget(parent), BBase(*new BImageWidgetPrivate(this))
{
    d_func()->init();
    setPixmap(pixmap);
}

BImageWidget::BImageWidget(const QPicture &picture, QWidget *parent) :
    QWidget(parent), BBase(*new BImageWidgetPrivate(this))
{
    d_func()->init();
    setPicture(picture);
}

BImageWidget::BImageWidget(const QImage &image, QWidget *parent) :
    QWidget(parent), BBase(*new BImageWidgetPrivate(this))
{
    d_func()->init();
    setImage(image);
}

BImageWidget::BImageWidget(const QIcon &icon, QWidget *parent) :
    QWidget(parent), BBase(*new BImageWidgetPrivate(this))
{
    d_func()->init();
    setIcon(icon);
}

BImageWidget::BImageWidget(const QString &fileName, QWidget *parent) :
    QWidget(parent), BBase(*new BImageWidgetPrivate(this))
{
    d_func()->init();
    setImage(fileName);
}

BImageWidget::~BImageWidget()
{
    //
}

/*============================== Protected constructors ====================*/

BImageWidget::BImageWidget(BImageWidgetPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

bool BImageWidget::hasScaledContents() const
{
    return d_func()->lbl->hasScaledContents();
}

QPicture BImageWidget::picture() const
{
    if (d_func()->mov->isValid())
        return QPicture();
    else
        return *d_func()->lbl->picture();
}

QPixmap BImageWidget::pixmap() const
{
    if (d_func()->mov->isValid())
        return d_func()->mov->currentPixmap();
    else
        return *d_func()->lbl->pixmap();
}

QImage BImageWidget::image() const
{
    if (d_func()->mov->isValid())
        return d_func()->mov->currentImage();
    else
        return d_func()->lbl->pixmap()->toImage();
}

QIcon BImageWidget::icon() const
{
    if (d_func()->mov->isValid())
        return QIcon(d_func()->mov->currentPixmap());
    else
        return QIcon(*d_func()->lbl->pixmap());
}

QString BImageWidget::imageFileName() const
{
    return d_func()->fileName;
}


/*============================== Public slots ==============================*/

void BImageWidget::setPixmap(const QPixmap &p)
{
    d_func()->reset(false);
    d_func()->lbl->setPixmap(p);
    d_func()->fileName.clear();
}

void BImageWidget::setPicture(const QPicture &p)
{
    d_func()->reset(false);
    d_func()->lbl->setPicture(p);
    d_func()->fileName.clear();
}

void BImageWidget::setImage(const QImage &img)
{
    d_func()->reset(false);
    d_func()->lbl->setPixmap(QPixmap::fromImage(img));
    d_func()->fileName.clear();
}

void BImageWidget::setIcon(const QIcon &icn)
{
    d_func()->reset(false);
    d_func()->lbl->setPixmap(icn.pixmap(100000)); //Getting the largest image possible
    d_func()->fileName.clear();
}

void BImageWidget::setImage(const QString &fileName)
{
    d_func()->fileName = fileName;
    static QList<QByteArray> AnimatedFormats = QList<QByteArray>() << "gif" << "mng"; //apng is not supported by Qt
    if (AnimatedFormats.contains(QImageReader::imageFormat(fileName)))
    {
        d_func()->mov->setFileName(fileName);
        d_func()->reset(true);
    }
    else
    {
        d_func()->reset(false);
        d_func()->lbl->setPixmap(QPixmap(fileName));
    }
}

void BImageWidget::setScaledContents(bool b)
{
    d_func()->lbl->setScaledContents(b);
}
