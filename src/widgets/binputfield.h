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

#ifndef BINPUTFIELD_H
#define BINPUTFIELD_H

class BInputFieldPrivate;

class QSize;

#include <BeQtCore/BBase>
#include <BeQtCore/BTranslation>

#include <QWidget>

/*============================================================================
================================ BInputField =================================
============================================================================*/

class B_WIDGETS_EXPORT BInputField : public QWidget, public BBase
{
    Q_OBJECT
    Q_PROPERTY(BTranslation invalidToolTip READ invalidToolTip WRITE setInvalidToolTip)
    Q_PROPERTY(ShowStyle showStyle READ showStyle WRITE setShowStyle)
    Q_PROPERTY(bool valid READ isValid STORED false)
    Q_PROPERTY(BTranslation validToolTip READ validToolTip WRITE setValidToolTip)
    Q_ENUMS(ShowStyle)
    B_DECLARE_PRIVATE(BInputField)
public:
    enum ShowStyle
    {
        ShowAlways = 1,
        ShowIfInvalid,
        ShowIfValid,
        ShowNever
    };
public:
    explicit BInputField(QWidget *parent = 0);
    explicit BInputField(ShowStyle s, QWidget *parent = 0);
    ~BInputField();
public:
    void addWidget(QWidget *w);
    void insertWidget(int index, QWidget *w);
    BTranslation invalidToolTip() const;
    bool isValid() const;
    void removeWidget(QWidget *w);
    void setInvalidToolTip(const BTranslation &t);
    void setShowStyle(ShowStyle s);
    void setValidToolTip(const BTranslation &t);
    ShowStyle showStyle() const;
    QSize sizeHint() const;
    BTranslation validToolTip() const;
public Q_SLOTS:
    void setValid(bool b);
private:
    Q_DISABLE_COPY(BInputField)
};

#endif // BINPUTFIELD_H
