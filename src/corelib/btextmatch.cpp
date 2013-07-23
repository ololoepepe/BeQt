#include "btextmatch.h"
#include "bglobal.h"
#include "bbase.h"
#include "bbase_p.h"
#include "bnamespace.h"

#include <QObject>
#include <QString>
#include <QDebug>

/*============================================================================
================================ BTextMatchPrivate ===========================
============================================================================*/

class BTextMatchPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTextMatch)
public:
    explicit BTextMatchPrivate(BTextMatch *q);
    ~BTextMatchPrivate();
public:
    void init();
public:
    const QString *txt;
    int pos;
    int len;
private:
    Q_DISABLE_COPY(BTextMatchPrivate)
};

/*============================================================================
================================ BTextMatchPrivate ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BTextMatchPrivate::BTextMatchPrivate(BTextMatch *q) :
    BBasePrivate(q)
{
    //
}

BTextMatchPrivate::~BTextMatchPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BTextMatchPrivate::init()
{
    txt = 0;
    pos = -1;
    len = -1;
}

/*============================================================================
================================ BTextMatch ==================================
============================================================================*/

/*============================== Public constructors =======================*/

BTextMatch::BTextMatch() :
    BBase(*new BTextMatchPrivate(this))
{
    d_func()->init();
}

BTextMatch::BTextMatch(const QString *const text, int position, int length) :
    BBase(*new BTextMatchPrivate(this))
{
    B_D(BTextMatch);
    d->init();
    d->txt = text;
    d->pos = position;
    d->len = length;
}

BTextMatch::BTextMatch(const BTextMatch &other) :
    BBase(*new BTextMatchPrivate(this))
{
    d_func()->init();
    *this = other;
}

BTextMatch::~BTextMatch()
{
    //
}

/*============================== Public methods ============================*/

QString BTextMatch::text() const
{
    const B_D(BTextMatch);
    return d->txt ? d->txt->mid(d->pos, d->len) : QString();
}

int BTextMatch::position() const
{
    return d_func()->pos;
}

int BTextMatch::length() const
{
    return d_func()->len;
}

bool BTextMatch::isValid() const
{
    const B_D(BTextMatch);
    return d->txt && d->pos >= 0 && d->len >= 0;
}

/*============================== Public operators ==========================*/

BTextMatch &BTextMatch::operator =(const BTextMatch &other)
{
    B_D(BTextMatch);
    const BTextMatchPrivate *dd = other.d_func();
    d->txt = dd->txt;
    d->pos = dd->pos;
    d->len = dd->len;
    return *this;
}

bool BTextMatch::operator ==(const BTextMatch &other) const
{
    const B_D(BTextMatch);
    const BTextMatchPrivate *dd = other.d_func();
    return d->txt == dd->txt && d->pos == dd->pos && d->len == dd->len;
}

BTextMatch::operator QString() const
{
    return text();
}

/*============================== Public friend operators ===================*/

QDebug operator <<(QDebug dbg, const BTextMatch &tm)
{
    dbg.nospace() << "BTextMatch(" << tm.text() << ")";
    return dbg.space();
}
