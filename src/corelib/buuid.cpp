#include "buuid.h"
#include "bglobal.h"
#include "bbase.h"
#include "bbase_p.h"

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QtGlobal>
#include <QUuid>

/*============================================================================
================================ BUuidPrivate ================================
============================================================================*/

class BUuidPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BUuid)
public:
    explicit BUuidPrivate(BUuid *q);
    ~BUuidPrivate();
public:
    void init();
public:
    QUuid muuid;
private:
    Q_DISABLE_COPY(BUuidPrivate)
};

/*============================================================================
================================ BUuidPrivate ================================
============================================================================*/

/*============================== Public constructors =======================*/

BUuidPrivate::BUuidPrivate(BUuid *q) :
    BBasePrivate(q)
{
    //
}

BUuidPrivate::~BUuidPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BUuidPrivate::init()
{
    //
}

/*============================================================================
================================ BUuid =======================================
============================================================================*/

/*============================== Public constructors =======================*/

BUuid::BUuid(const QUuid &uuid) :
    BBase(*new BUuidPrivate(this))
{
    d_func()->init();
    d_func()->muuid = uuid;
}

BUuid::BUuid(uint l, ushort w1, ushort w2, uchar b1, uchar b2, uchar b3, uchar b4, uchar b5, uchar b6, uchar b7,
             uchar b8) :
    BBase(*new BUuidPrivate(this))
{
    d_func()->init();
    d_func()->muuid = QUuid(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8);
}

BUuid::BUuid(const QString &text) :
    BBase(*new BUuidPrivate(this))
{
    d_func()->init();
    d_func()->muuid = QUuid(text);
}

BUuid::BUuid(const char *text) :
    BBase(*new BUuidPrivate(this))
{
    d_func()->init();
    d_func()->muuid = QUuid(text);
}

BUuid::BUuid(const QByteArray &text) :
    BBase(*new BUuidPrivate(this))
{
    d_func()->init();
    d_func()->muuid = QUuid(text);
}

#if defined(Q_OS_WIN)
BUuid::BUuid(const GUID &guid) :
    BBase(*new BUuidPrivate(this))
{
    d_func()->init();
    d_func()->muuid = QUuid(guid);
}
#endif

BUuid::BUuid(const BUuid &other) :
    BBase(*new BUuidPrivate(this))
{
    d_func()->init();
    *this = other;
}

BUuid::~BUuid()
{
    //
}

/*============================== Static public methods =====================*/

BUuid BUuid::createUuid()
{
    return BUuid(QUuid::createUuid());
}

BUuid BUuid::fromRfc4122(const QByteArray &bytes)
{
    return BUuid(QUuid::fromRfc4122(bytes));
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
BUuid BUuid::createUuidV3(const BUuid &ns, const QByteArray &baseData)
{
    return BUuid(QUuid::createUuidV3(ns.d_func()->muuid, baseData));
}

BUuid BUuid::createUuidV3(const BUuid &ns, const QString &baseData)
{
    return BUuid(QUuid::createUuidV3(ns.d_func()->muuid, baseData));
}

BUuid BUuid::createUuidV5(const BUuid &ns, const QByteArray &baseData)
{
    return BUuid(QUuid::createUuidV5(ns.d_func()->muuid, baseData));
}

BUuid BUuid::createUuidV5(const BUuid &ns, const QString &baseData)
{
    return BUuid(QUuid::createUuidV5(ns.d_func()->muuid, baseData));
}
#endif

/*============================== Public methods ============================*/

bool BUuid::isNull() const
{
    return d_func()->muuid.isNull();
}

QUuid BUuid::toUuid() const
{
    return d_func()->muuid;
}

QByteArray BUuid::toRfc4122() const
{
    return d_func()->muuid.toRfc4122();
}

QString BUuid::toString() const
{
    return d_func()->muuid.toString();
}

QByteArray BUuid::toByteArray() const
{
    return d_func()->muuid.toByteArray();
}

QUuid::Variant BUuid::variant() const
{
    return d_func()->muuid.variant();
}

QUuid::Version BUuid::version() const
{
    return d_func()->muuid.version();
}

/*============================== Public operators ==========================*/

#if defined(Q_OS_WIN)
BUuid::operator GUID() const
{
    const QUuid &u = d_func()->muuid;
    GUID guid = {u.data1, u.data2, u.data3,
                 {u.data4[0], u.data4[1], u.data4[2], u.data4[3], u.data4[4], u.data4[5], u.data4[6], u.data4[7]}};
    return guid;
}

#endif
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
BUuid::operator QString() const
{
    return toString();
}
#endif

BUuid &BUuid::operator= (const BUuid &other)
{
    d_func()->muuid = other.d_func()->muuid;
    return *this;
}

bool BUuid::operator!= (const BUuid &other) const
{
    return d_func()->muuid != other.d_func()->muuid;
}

bool BUuid::operator< (const BUuid &other) const
{
    return d_func()->muuid < other.d_func()->muuid;
}

bool BUuid::operator== (const BUuid &other) const
{
    return d_func()->muuid == other.d_func()->muuid;
}

bool BUuid::operator> (const BUuid &other) const
{
    return d_func()->muuid > other.d_func()->muuid;
}

#if defined(Q_OS_WIN)
bool BUuid::operator!= (const GUID &guid) const
{
    return d_func()->muuid != guid;
}

BUuid &BUuid::operator= (const GUID &guid)
{
    d_func()->muuid = guid;
    return *this;
}

bool BUuid::operator== (const GUID &guid) const
{
    return d_func()->muuid == guid;
}
#endif

/*============================== Public friend operators ===================*/

QDataStream &operator<< (QDataStream &s, const BUuid &id)
{
    s << id.d_func()->muuid.toString();
    return s;
}

QDataStream &operator>> (QDataStream &s, BUuid &id)
{
    QString string;
    s >> string;
    id.d_func()->muuid = QUuid(string);
    return s;
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
uint qHash(const BUuid &uuid, uint seed)
{
    return qHash(uuid.d_func()->muuid, seed);
}

QDebug operator<< (QDebug dbg, const BUuid &id)
{
    dbg << id.d_func()->muuid;
    return dbg;
}
#endif
