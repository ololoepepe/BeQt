#ifndef BTEXTMATCHLIST_H
#define BTEXTMATCHLIST_H

class QStringList;

#include "btextmatch.h"
#include "bglobal.h"

#include <QList>

/*============================================================================
================================ BTextMatchList ==============================
============================================================================*/

class B_CORE_EXPORT BTextMatchList : public QList<BTextMatch>
{
public:
    QStringList texts() const;
public:
    operator QStringList() const;
};

#endif // BTEXTMATCHLIST_H
