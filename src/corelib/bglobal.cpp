/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
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

#include "bglobal.h"

#include "bpassword.h"
#include "bpersoninfo.h"
#include "bpersoninfolist.h"
#include "bproperties.h"
#include "btranslation.h"
#include "buuid.h"
#include "bversion.h"

#include <QDebug>
#include <QList>
#include <QMetaType>
#include <QString>

QList<int> bRange(int lb, int ub, int step)
{
    if (lb == ub)
        return QList<int>() << lb;
    if (!step)
        step = (lb < ub) ? 1 : -1;
    if ((lb < ub && step < 0) || (lb > ub && step > 0))
        return QList<int>();
    QList<int> r;
    if (lb < ub) {
        for (int i = lb; i <= ub; i += step)
            r << i;
    } else {
        for (int i = lb; i >= ub; i += step)
            r << i;
    }
    return r;
}

QList<int> bRangeD(int lb, int ub, unsigned step)
{
    return bRange(lb, ub, (step ? step : 1));
}

QList<int> bRangeM(int lb, int ub, unsigned multiplier)
{
    if (!lb || !ub || !multiplier || (lb < 0 && ub > 0) || (lb > 0 && ub < 0))
        return QList<int>();
    if (lb == ub)
        return QList<int>() << lb;
    if (1 == multiplier)
        return QList<int>();
    QList<int> r;
    if (lb < ub) {
        for (int i = lb; i <= ub; i *= multiplier)
            r << i;
    } else {
        for (int i = lb; i >= ub; i /= multiplier)
            r << i;
    }
    return r;
}

QList<int> bRangeR(int lb, int ub, unsigned step)
{
    return bRange(lb, ub, -1 * (step ? step : 1));
}

void bRegister()
{
    static bool registered = false;
    if (registered)
        return;
    qRegisterMetaType<BPassword>();
    qRegisterMetaTypeStreamOperators<BPassword>();
    qRegisterMetaType<BPersonInfo>();
    qRegisterMetaTypeStreamOperators<BPersonInfo>();
    qRegisterMetaType<BPersonInfoList>();
    qRegisterMetaTypeStreamOperators<BPersonInfoList>();
    qRegisterMetaType<BProperties>();
    qRegisterMetaTypeStreamOperators<BProperties>();
    qRegisterMetaType<BTranslation>();
    qRegisterMetaTypeStreamOperators<BTranslation>();
    qRegisterMetaType<BTr>("BTr");
    qRegisterMetaTypeStreamOperators<BTr>("BTr");
    qRegisterMetaType<BUuid>();
    qRegisterMetaTypeStreamOperators<BUuid>();
    qRegisterMetaType<BVersion>();
    qRegisterMetaTypeStreamOperators<BVersion>();
    registered = true;
}

bool bTest(bool condition, const char *where, const char *what)
{
    if (!condition)
        qCritical() << qPrintable(QString(where) + ": " + QString(what));
    return condition;
}

const char *bVersion()
{
    return "4.0.0-beta3";
}
