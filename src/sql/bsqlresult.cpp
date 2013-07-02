#include "bsqlresult.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BeQt>

#include <QObject>
#include <QVariant>
#include <QVariantMap>
#include <QSqlError>
#include <QList>
#include <QString>

/*============================================================================
================================ BSqlResultPrivate ===========================
============================================================================*/

class BSqlResultPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSqlResult)
public:
    explicit BSqlResultPrivate(BSqlResult *q);
    ~BSqlResultPrivate();
public:
    void init();
public:
    bool success;
    BSqlResult::BVariantMapList values;
    QVariant insertId;
    QSqlError error;
private:
    Q_DISABLE_COPY(BSqlResultPrivate)
};

/*============================================================================
================================ BSqlResultPrivate ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BSqlResultPrivate::BSqlResultPrivate(BSqlResult *q) :
    BBasePrivate(q)
{
    //
}

BSqlResultPrivate::~BSqlResultPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BSqlResultPrivate::init()
{
    success = false;
}

/*============================================================================
================================ BSqlResult ==================================
============================================================================*/

/*============================== Public constructors =======================*/

BSqlResult::BSqlResult(bool success, const QSqlError &error) :
    BBase(*new BSqlResultPrivate(this))
{
    d_func()->init();
    setSuccess(success);
    setLastError(error);
}

BSqlResult::BSqlResult(const QSqlError &error) :
    BBase(*new BSqlResultPrivate(this))
{
    d_func()->init();
    setSuccess(false);
    setLastError(error);
}

BSqlResult::BSqlResult(const BSqlResult &other) :
    BBase(*new BSqlResultPrivate(this))
{
    d_func()->init();
    *this = other;
}

BSqlResult::~BSqlResult()
{
    //
}

/*============================== Public methods ============================*/

void BSqlResult::setSuccess(bool b)
{
    d_func()->success = b;
}

void BSqlResult::setValues(const BVariantMapList &list)
{
    d_func()->values = list;
}

void BSqlResult::setLastInsertId(const QVariant &id)
{
    d_func()->insertId = id;
}

void BSqlResult::setLastError(const QSqlError &e)
{
    d_func()->error = e;
}

bool BSqlResult::success() const
{
    return d_func()->success;
}

BSqlResult::BVariantMapList BSqlResult::values() const
{
    return d_func()->values;
}

QVariantMap BSqlResult::value() const
{
    return !d_func()->values.isEmpty() ? d_func()->values.first() : QVariantMap();
}

QVariant BSqlResult::value(const QString &key) const
{
    return (!key.isEmpty() && !d_func()->values.isEmpty()) ? d_func()->values.first().value(key) : QVariant();
}

QVariant BSqlResult::lastInsertId(QVariant::Type t) const
{
    const B_D(BSqlResult);
    if (QVariant::Invalid == t || !d->insertId.isNull() || !d->insertId.canConvert(t))
        return d->insertId;
    QVariant id = d->insertId;
    id.convert(t);
    return id;
}

QSqlError BSqlResult::lastError() const
{
    return d_func()->error;
}

/*============================== Public operators ==========================*/

BSqlResult &BSqlResult::operator =(const BSqlResult &other)
{
    B_D(BSqlResult);
    const BSqlResultPrivate *dd = other.d_func();
    d->success = dd->success;
    d->values = dd->values;
    d->insertId = dd->insertId;
    d->error = dd->error;
    return *this;
}

bool BSqlResult::operator ==(const BSqlResult &other) const
{
    const B_D(BSqlResult);
    const BSqlResultPrivate *dd = other.d_func();
    return d->success == dd->success && d->values == dd->values && d->insertId == dd->insertId
            && d->error.type() == dd->error.type();
}

BSqlResult::operator bool() const
{
    return d_func()->success;
}
