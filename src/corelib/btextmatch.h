#ifndef BTEXTMATCH_H
#define BTEXTMATCH_H

class BTextMatchPrivate;
class BTextMatchList;

class QString;
class QDebug;
class QRegExp;

#include "bglobal.h"
#include "bbase.h"
#include "btexttools.h"

/*============================================================================
================================ BTextMatch ==================================
============================================================================*/

class B_CORE_EXPORT BTextMatch : public BBase
{
    B_DECLARE_PRIVATE(BTextMatch)
public:
    explicit BTextMatch();
    BTextMatch(const BTextMatch &other);
    ~BTextMatch();
private:
    explicit BTextMatch(const QString *const text, int position, int length);
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
    friend BTextMatchList BTextTools::match(const QString &text, const QRegExp &what, const QRegExp &prefixedBy,
                                            const QRegExp &postfixedBy);
public:
    friend QDebug operator <<(QDebug dbg, const BTextMatch &tm);
};

#endif // BTEXTMATCH_H
