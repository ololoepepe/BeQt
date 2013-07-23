#include "btextmatchlist.h"
#include "bglobal.h"

#include <QStringList>

/*============================================================================
================================ BTextMatchList ==============================
============================================================================*/

/*============================== Public methods ============================*/

QStringList BTextMatchList::texts() const
{
    QStringList list;
    foreach (const BTextMatch r, *this)
        list << r.text();
    return list;
}

/*============================== Public operators ==========================*/

BTextMatchList::operator QStringList() const
{
    return texts();
}
