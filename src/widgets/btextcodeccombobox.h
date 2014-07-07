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

#ifndef BTEXTCODECCOMBOBOX_H
#define BTEXTCODECCOMBOBOX_H

class BTextCodecComboBoxPrivate;

class QString;
class QTextCodec;

#include "btextcodecmenu.h"

#include <BeQtCore/BBaseObject>

#include <QComboBox>

/*============================================================================
================================ BTextCodecComboBox ==========================
============================================================================*/

class B_WIDGETS_EXPORT BTextCodecComboBox : public QComboBox, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTextCodecComboBox)
public:
    explicit BTextCodecComboBox(QWidget *parent = 0);
    explicit BTextCodecComboBox(BTextCodecMenu::Style s, QWidget *parent = 0);
    ~BTextCodecComboBox();
protected:
    explicit BTextCodecComboBox(BTextCodecComboBoxPrivate &d, QWidget *parent = 0);
public:
    QTextCodec *selectedCodec() const;
    QString selectedCodecName() const;
    BTextCodecMenu::Style style() const;
public Q_SLOTS:
    void selectCodec(QTextCodec *codec);
    void selectCodec(const QString &codecName);
protected:
    void hidePopup();
    void showPopup();
Q_SIGNALS:
    void codecChanged(QTextCodec *codec);
    void codecNameChanged(const QString &codecName);
private:
    Q_DISABLE_COPY(BTextCodecComboBox)
};

#endif // BTEXTCODECCOMBOBOX_H
