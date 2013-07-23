#ifndef BTEXTMATCH_H
#define BTEXTMATCH_H

class BTextMatchPrivate;

class QString;
class QDebug;

#include "bglobal.h"
#include "bbase.h"

/*============================================================================
================================ BTextMatch ==================================
============================================================================*/

class B_CORE_EXPORT BTextMatch : public BBase
{
    B_DECLARE_PRIVATE(BTextMatch)
public:
    explicit BTextMatch();
    explicit BTextMatch(const QString *const text, int position, int length);
    BTextMatch(const BTextMatch &other);
    ~BTextMatch();
public:
    QString text() const;
    int position() const;
    int length() const;
    bool isValid() const;
public:
    BTextMatch &operator =(const BTextMatch &other);
    bool operator ==(const BTextMatch &other) const;
    operator QString() const;
public:
    friend QDebug operator <<(QDebug dbg, const BTextMatch &tm);
};

#endif // BTEXTMATCH_H
