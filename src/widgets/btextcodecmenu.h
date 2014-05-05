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

class QTextCodec;
class QString;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QMenu>

/*============================================================================
================================ BTextCodecMenu ==============================
============================================================================*/

class B_WIDGETS_EXPORT BTextCodecMenu : public QMenu, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTextCodecMenu)
public:
    enum Style
    {
        StructuredStyle = 0,
        PlainStyle
    };
public:
    explicit BTextCodecMenu(QWidget *parent = 0);
    explicit BTextCodecMenu(Style s, QWidget *parent = 0);
    ~BTextCodecMenu();
protected:
    explicit BTextCodecMenu(BTextCodecMenuPrivate &d, QWidget *parent = 0);
public:
    void setMapping(const QObject *receiver, const char *method);
    Style style() const;
    QTextCodec *selectedCodec() const;
    QString selectedCodecName() const;
private:
    Q_DISABLE_COPY(BTextCodecMenu)
};

#endif // BTEXTCODECMENU_H
