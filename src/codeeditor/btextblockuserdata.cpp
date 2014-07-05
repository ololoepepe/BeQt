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

#include "btextblockuserdata.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QTextBlockUserData>
#include <QString>
#include <QTextBlock>
#include <QList>
#include <QtAlgorithms>

#include <QDebug>

/*============================================================================
================================ BTextBlockUserDataPrivate ===================
============================================================================*/

class BTextBlockUserDataPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTextBlockUserData)
public:
    explicit BTextBlockUserDataPrivate(BTextBlockUserData *q);
    ~BTextBlockUserDataPrivate();
public:
    static QList<BTextBlockUserData::SkipInterval> processList(const QList<BTextBlockUserData::SkipInterval> &list);
    static bool lessThan(const BTextBlockUserData::SkipInterval &si1, const BTextBlockUserData::SkipInterval &si2);
public:
    void init();
public:
    QList<BTextBlockUserData::SkipInterval> skipIntervals;
};

/*============================================================================
================================ BTextBlockUserDataPrivate ===================
============================================================================*/

/*============================== Public constructors =======================*/

BTextBlockUserDataPrivate::BTextBlockUserDataPrivate(BTextBlockUserData *q) :
    BBasePrivate(q)
{
    //
}

BTextBlockUserDataPrivate::~BTextBlockUserDataPrivate()
{
    //
}

/*============================== Static public methods =====================*/

QList<BTextBlockUserData::SkipInterval> BTextBlockUserDataPrivate::processList(
        const QList<BTextBlockUserData::SkipInterval> &list)
{
    if (list.isEmpty())
        return list;
    QList<BTextBlockUserData::SkipInterval> nlist = list;
    foreach (int i, bRangeR(nlist.size() - 1, 0)) {
        const BTextBlockUserData::SkipInterval &si = nlist.at(i);
        if (si.start < 0 || si.end < 0 || si.start > si.end)
            nlist.removeAt(i);
    }
    if (nlist.isEmpty())
        return nlist;
    qSort(nlist.begin(), nlist.end(), &lessThan);
    foreach (int i, bRangeR(nlist.size() - 1, 1)) {
        BTextBlockUserData::SkipInterval &si1 = nlist[i - 1];
        const BTextBlockUserData::SkipInterval &si2 = nlist.at(i);
        if (si1.end >= si2.start)
            si1.end = si2.end;
        nlist.removeAt(i);
    }
    return nlist;
}

bool BTextBlockUserDataPrivate::lessThan(const BTextBlockUserData::SkipInterval &si1,
                                         const BTextBlockUserData::SkipInterval &si2)
{
    return si1.start < si2.start;
}

/*============================== Public methods ============================*/

void BTextBlockUserDataPrivate::init()
{
    //
}

/*============================================================================
================================ BTextBlockUserData ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BTextBlockUserData::BTextBlockUserData(const QList<SkipInterval> &list) :
    BBase(*new BTextBlockUserDataPrivate(this))
{
    d_func()->init();
    setSkipIntervals(list);
}

BTextBlockUserData::~BTextBlockUserData()
{
    //
}

/*============================== Static public methods =====================*/

QString BTextBlockUserData::textWithoutSkipIntervals(const BTextBlockUserData *ud, const QString &text, char replacer)
{
    if (!ud)
        return text;
    QString ntext = text;
    foreach (int i, bRangeR(ud->d_func()->skipIntervals.size() - 1, 0)) {
        const SkipInterval &si = ud->d_func()->skipIntervals.at(i);
        if (si.start >= ntext.length() || si.end >= ntext.length())
            continue;
        int len = si.end - si.start + 1;
        if ('\0' != replacer)
            ntext.replace(si.start, len, replacer);
        else
            ntext.remove(si.start, len);
    }
    return ntext;
}

QString BTextBlockUserData::textWithoutSkipIntervals(const QTextBlock &block, char replacer)
{
    return textWithoutSkipIntervals(static_cast<BTextBlockUserData *>(block.userData()), block.text(), replacer);
}

QList<BTextBlockUserData::SkipInterval> BTextBlockUserData::skipIntervals(const QTextBlock &block)
{
    BTextBlockUserData *ud = static_cast<BTextBlockUserData *>(block.userData());
    return ud ? ud->d_func()->skipIntervals : QList<SkipInterval>();
}

bool BTextBlockUserData::shouldSkip(const BTextBlockUserData *ud, int pos)
{
    if (!ud || pos < 0)
        return false;
    foreach (const SkipInterval &si, ud->d_func()->skipIntervals) {
        if (si.start <= pos && si.end >= pos)
            return true;
    }
    return false;
}

bool BTextBlockUserData::shouldSkip(const QTextBlock &block, int pos)
{
    if (pos < 0 || pos >= block.length())
        return false;
    BTextBlockUserData *ud = static_cast<BTextBlockUserData *>(block.userData());
    if (!ud)
        return false;
    return shouldSkip(ud, pos);
}

/*============================== Public methods ============================*/

void BTextBlockUserData::setSkipIntervals(const QList<SkipInterval> &list)
{
    d_func()->skipIntervals = BTextBlockUserDataPrivate::processList(list);
}

QList<BTextBlockUserData::SkipInterval> BTextBlockUserData::skipIntervals() const
{
    return d_func()->skipIntervals;
}
