#include "bglobal.h"

#include <QString>
#include <QList>

#include <QDebug>

bool bTest(bool condition, const char *where, const char *what)
{
    if (!condition)
        qCritical() << qPrintable( QString(where) + ": " + QString(what) );
    return condition;
}

const char *bVersion()
{
    return "2.1.3";
}

QList<int> bRange(int lb, int ub, int step)
{
    if (lb == ub)
        return QList<int>() << lb;
    if (!step)
        step = (lb < ub) ? 1 : -1;
    if ((lb < ub && step < 0) || (lb > ub && step > 0))
        return QList<int>();
    QList<int> r;
    if (lb < ub)
        for (int i = lb; i <= ub; i += step)
            r << i;
    else
        for (int i = lb; i >= ub; i += step)
            r << i;
    return r;
}

QList<int> bRangeD(int lb, int ub, unsigned step)
{
    return bRange(lb, ub, (step ? step : 1));
}

QList<int> bRangeR(int lb, int ub, unsigned step)
{
    return bRange(lb, ub, -1 * (step ? step : 1));
}
