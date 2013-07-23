#include "btranslation.h"
#include "bglobal.h"
#include "bbase.h"
#include "bbase_p.h"
#include "bnamespace.h"

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariant>
#include <QVariantMap>
#include <QDataStream>
#include <QDebug>

/*============================================================================
================================ BTranslationPrivate =========================
============================================================================*/

class BTranslationPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTranslation)
public:
    explicit BTranslationPrivate(BTranslation *q);
    ~BTranslationPrivate();
public:
    void init();
public:
    QString context;
    QString sourceText;
    QString disambiguation;
    int n;
private:
    Q_DISABLE_COPY(BTranslationPrivate)
};

/*============================================================================
================================ BTranslationPrivate ========================
============================================================================*/

/*============================== Public constructors =======================*/

BTranslationPrivate::BTranslationPrivate(BTranslation *q) :
    BBasePrivate(q)
{
    //
}

BTranslationPrivate::~BTranslationPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BTranslationPrivate::init()
{
    n = -1;
}

/*============================================================================
================================ BTranslation ================================
============================================================================*/

/*============================== Static public methods =====================*/

BTranslation BTranslation::translate(const char *context, const char *sourceText, const char *disambiguation, int n)
{
    if (n < 0)
        n = -1;
    BTranslation t;
    BTranslationPrivate *dd = t.d_func();
    dd->context = context;
    dd->sourceText = sourceText;
    dd->disambiguation = disambiguation;
    dd->n = n;
    return t;
}

BTranslation BTranslation::tr(const char *sourceText, const char *disambiguation, int n)
{
    return translate("BTranslation", sourceText, disambiguation, n);
}

QString BTranslation::translate(const BTranslation &t)
{
    return t.translate();
}

QString BTranslation::tr(const BTranslation &t)
{
    return t.translate();
}

/*============================== Public constructors =======================*/

BTranslation::BTranslation() :
    BBase(*new BTranslationPrivate(this))
{
    d_func()->init();
}

BTranslation::BTranslation(const BTranslation &other) :
    BBase(*new BTranslationPrivate(this))
{
    *this = other;
}

BTranslation::~BTranslation()
{
    //
}

/*============================== Protected constructors ====================*/

BTranslation::BTranslation(BTranslationPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QString BTranslation::context() const
{
    return d_func()->context;
}

QString BTranslation::sourceText() const
{
    return d_func()->sourceText;
}

QString BTranslation::disambiguation() const
{
    return d_func()->disambiguation;
}

int BTranslation::n() const
{
    return d_func()->n;
}

QString BTranslation::translate() const
{
    const B_D(BTranslation);
    return BeQt::translate(d->context.toUtf8().constData(), d->sourceText.toUtf8().constData(),
                           d->disambiguation.toUtf8().constData(), d->n);
}

QString BTranslation::tr() const
{
    return translate();
}

bool BTranslation::isValid() const
{
    return !d_func()->context.isEmpty() && !d_func()->sourceText.isEmpty();
}

/*============================== Public operators ==========================*/

BTranslation &BTranslation::operator =(const BTranslation &other)
{
    B_D(BTranslation);
    const BTranslationPrivate *dd = other.d_func();
    d->context = dd->context;
    d->sourceText = dd->sourceText;
    d->disambiguation = dd->disambiguation;
    d->n = dd->n;
    return *this;
}

bool BTranslation::operator ==(const BTranslation &other) const
{
    const B_D(BTranslation);
    const BTranslationPrivate *dd = other.d_func();
    return d->context == dd->context && d->sourceText == dd->sourceText && d->disambiguation == dd->disambiguation
            && d->n == dd->n;
}

bool BTranslation::operator !=(const BTranslation &other) const
{
    return !(*this == other);
}

BTranslation::operator QString() const
{
    return translate();
}

BTranslation::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*============================== Public friend operators ===================*/

QDataStream &operator <<(QDataStream &stream, const BTranslation &t)
{
    const BTranslationPrivate *d = t.d_func();
    QVariantMap m;
    m.insert("context", d->context);
    m.insert("source_text", d->sourceText);
    m.insert("disambiguation", d->disambiguation);
    m.insert("n", d->n);
    stream << m;
    return stream;
}

QDataStream &operator >>(QDataStream &stream, BTranslation &t)
{
    BTranslationPrivate *d = t.d_func();
    QVariantMap m;
    stream >> m;
    d->context = m.value("context").toString();
    d->sourceText = m.value("source_text").toString();
    d->disambiguation = m.value("disambiguation").toString();
    d->n = m.value("n", -1).toInt();
    if (d->n < 0)
        d->n = -1;
    return stream;
}

QDebug operator <<(QDebug dbg, const BTranslation &t)
{
    dbg.nospace() << "BTranslation(" << t.context() << "," << t.sourceText() << "," << t.disambiguation() << ")";
    return dbg.space();
}
