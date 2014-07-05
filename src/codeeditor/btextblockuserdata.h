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

class BTextBlockUserDataPrivate;

class QString;
class QTextBlock;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QTextBlockUserData>
#include <QList>

/*============================================================================
================================ BTextBlockUserData ==========================
============================================================================*/

class B_CODEEDITOR_EXPORT BTextBlockUserData : public QTextBlockUserData, public BBase
{
    B_DECLARE_PRIVATE(BTextBlockUserData)
public:
    struct SkipInterval
    {
        int start;
        int end;
    };
public:
    explicit BTextBlockUserData(const QList<SkipInterval> &list);
    explicit BTextBlockUserData();
    ~BTextBlockUserData();
public:
    static QString textWithoutSkipIntervals(const BTextBlockUserData *ud, const QString &text, char replacer = '\0');
    static QString textWithoutSkipIntervals(const QTextBlock &block, char replacer = '\0');
    static QList<SkipInterval> skipIntervals(const QTextBlock &block);
    static bool shouldSkip(const BTextBlockUserData *ud, int pos);
    static bool shouldSkip(const QTextBlock &block, int pos);
public:
    void setSkipIntervals(const QList<SkipInterval> &list);
    QList<SkipInterval> skipIntervals() const;
};

#endif // BTEXTBLOCKUSERDATA_H
