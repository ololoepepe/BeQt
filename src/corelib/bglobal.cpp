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
    return "2.0.0-beta2";
}

QList<int> bRange(int lb, int ub, int step)
{
    if (lb == ub)
        return QList<int>() << lb;
    if (!step)
        step = 1;
    if ( (lb > ub && step > 0) || (lb < ub && step < 0) )
        step *= -1;
    QList<int> r;
    if (lb <= ub)
        for (int i = lb; i <= ub; i += step)
            r << i;
    else
        for (int i = lb; i >= ub; i += step)
            r << i;
    return r;
}
