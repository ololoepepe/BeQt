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

#ifndef BTEXTCODECMENU_H
#define BTEXTCODECMENU_H

class BTextCodecMenuPrivate;

class QString;
class QTextCodec;

#include <BeQtCore/BBaseObject>

#include <QMenu>

/*============================================================================
================================ BTextCodecMenu ==============================
============================================================================*/

class B_WIDGETS_EXPORT BTextCodecMenu : public QMenu, public BBaseObject
{
    Q_OBJECT
    Q_PROPERTY(QTextCodec * selectedCodec READ selectedCodec)
    Q_PROPERTY(QString selectedCodecName READ selectedCodecName)
    Q_PROPERTY(Style style READ style CONSTANT)
    Q_ENUMS(Style)
    B_DECLARE_PRIVATE(BTextCodecMenu)
public:
    enum Style
    {
        PlainStyle = 1,
        StructuredStyle
    };
public:
    explicit BTextCodecMenu(QWidget *parent = 0);
    explicit BTextCodecMenu(Style s, QWidget *parent = 0);
    ~BTextCodecMenu();
protected:
    explicit BTextCodecMenu(BTextCodecMenuPrivate &d, QWidget *parent = 0);
public:
    QTextCodec *selectedCodec() const;
    QString selectedCodecName() const;
    void setMapping(const QObject *receiver, const char *method);
    Style style() const;
private:
    Q_DISABLE_COPY(BTextCodecMenu)
};

#endif // BTEXTCODECMENU_H
