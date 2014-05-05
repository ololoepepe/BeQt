/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCodeEditor module of the BeQt library.
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

#ifndef BTEXTBLOCKUSERDATA_H
#define BTEXTBLOCKUSERDATA_H

class BTextBlock;

class QString;

#include <BeQtCore/BeQtGlobal>

#include <QTextBlockUserData>

/*============================================================================
================================ BTextBlockUserData ==========================
============================================================================*/

class B_CODEEDITOR_EXPORT BTextBlockUserData : public QTextBlockUserData
{
public:
    explicit BTextBlockUserData(int sf = -1, int st = -1);
    ~BTextBlockUserData();
public:
    static QString textWithoutComments(const BTextBlockUserData *ud, const QString &text);
    static QString textWithoutComments(const QTextBlock &block);
    static int blockSkipFrom(const QTextBlock &block);
public:
    int skipFrom;
    int skipTo;
};

#endif // BTEXTBLOCKUSERDATA_H
