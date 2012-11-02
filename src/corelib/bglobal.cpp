#include "bglobal.h"

#include <QString>
#include <QDebug>

bool bTest(bool condition, const char *where, const char *what)
{
    if (!condition)
        qCritical() << qPrintable( QString(where) + ": " + QString(what) );
    return condition;
}

const char *bVersion()
{
    return "2.0.0pa1";
}
