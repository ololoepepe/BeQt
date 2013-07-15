#include "bpassword.h"
#include "bglobal.h"
#include "bbase_p.h"
#include "bnamespace.h"

#include <QString>
#include <QByteArray>
#include <QCryptographicHash>
#include <QVariant>
#include <QVariantMap>
#include <QDataStream>
#include <QDebug>

/*============================================================================
================================ BPasswordPrivate ============================
============================================================================*/

class BPasswordPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPassword)
public:
    explicit BPasswordPrivate(BPassword *q);
    ~BPasswordPrivate();
public:
    void init();
public:
    BPassword::Mode mode;
    QString open;
    QByteArray encrypted;
    int charCount;
    QCryptographicHash::Algorithm algorithm;
    bool isEncrypted;
private:
    Q_DISABLE_COPY(BPasswordPrivate)
};

/*============================================================================
================================ BPasswordPrivate ============================
============================================================================*/

/*============================== Public constructors =======================*/

BPasswordPrivate::BPasswordPrivate(BPassword *q) :
    BBasePrivate(q)
{
    //
}

BPasswordPrivate::~BPasswordPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BPasswordPrivate::init()
{
    mode = BPassword::FlexibleMode;
    charCount = 0;
    algorithm = QCryptographicHash::Sha1;
    isEncrypted = false;
}

/*============================================================================
================================ BPassword ===================================
============================================================================*/

/*============================== Public constructors =======================*/

BPassword::BPassword(Mode m) :
    BBase(*new BPasswordPrivate(this))
{
    d_func()->init();
    setMode(m);
}

BPassword::BPassword(const QString &s) :
    BBase(*new BPasswordPrivate(this))
{
    d_func()->init();
    setPassword(s);
}

BPassword::BPassword(QCryptographicHash::Algorithm a, const QByteArray &ba, int charCountHint) :
    BBase(*new BPasswordPrivate(this))
{
    d_func()->init();
    setPassword(a, ba, charCountHint);
}

BPassword::BPassword(const BPassword &other) :
    BBase(*new BPasswordPrivate(this))
{
    d_func()->init();
    *this = other;
}

BPassword::~BPassword()
{
    //
}

/*============================== Protected constructors ====================*/

BPassword::BPassword(BPasswordPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BPassword::setMode(Mode m)
{
    if (m == d_func()->mode)
        return;
    d_func()->mode = m;
    switch (m)
    {
    case AlwaysOpenMode:
        clear();
        break;
    case AlwaysEncryptedMode:
        encrypt(d_func()->algorithm);
        break;
    case FlexibleMode:
    default:
        break;
    }
}

void BPassword::setPassword(const QString &s, QCryptographicHash::Algorithm a)
{
    clear();
    B_D(BPassword);
    d->open = s;
    if (AlwaysEncryptedMode == d->mode)
        encrypt(a);
}

void BPassword::setPassword(QCryptographicHash::Algorithm a, const QByteArray &ba, int charCountHint)
{
    clear();
    B_D(BPassword);
    if (AlwaysOpenMode == d->mode)
        return;
    d->encrypted = ba;
    d->charCount = (charCountHint > 0 && !ba.isEmpty()) ? charCountHint : 0;
    d->algorithm = a;
    d->isEncrypted = true;
}

void BPassword::encrypt(QCryptographicHash::Algorithm a)
{
    int count = 0;
    QByteArray ba = encryptedPassword(a, &count);
    setPassword(a, ba, count);
}

void BPassword::clear()
{
    B_D(BPassword);
    d->open.clear();
    d->encrypted.clear();
    d->charCount = 0;
    d->algorithm = QCryptographicHash::Sha1;
    d->isEncrypted = false;
}

void BPassword::restore(const QByteArray &data)
{
    QVariantMap m = BeQt::deserialize(data).toMap();
    if (m.value("encrypted").toBool())
        setPassword(static_cast<QCryptographicHash::Algorithm>(m.value("algorithm").toInt()),
                    m.value("encrypted_password").toByteArray(), m.value("char_count_hint").toInt());
    else
        setPassword(m.value("open_password").toString());
}

BPassword::Mode BPassword::mode() const
{
    return d_func()->mode;
}

QString BPassword::openPassword() const
{
    return d_func()->open;
}

QByteArray BPassword::encryptedPassword(int *hint) const
{
    return encryptedPassword(d_func()->algorithm, hint);
}

QByteArray BPassword::encryptedPassword(QCryptographicHash::Algorithm a, int *hint) const
{
    if (hint)
        *hint = charCountHint();
    return !d_func()->encrypted.isEmpty() ? d_func()->encrypted : QCryptographicHash::hash(d_func()->open.toUtf8(), a);
}

int BPassword::charCountHint(Mode m) const
{
    switch (m)
    {
    case AlwaysOpenMode:
        return d_func()->open.length();
    case AlwaysEncryptedMode:
        return d_func()->charCount;
    case FlexibleMode:
    default:
        return d_func()->isEncrypted ? d_func()->charCount : d_func()->open.length();
    }
}

QCryptographicHash::Algorithm BPassword::algorithm() const
{
    return d_func()->algorithm;
}

QByteArray BPassword::save(Mode mode) const
{
    const B_D(BPassword);
    QVariantMap m;
    bool enc = AlwaysEncryptedMode == mode || d->isEncrypted;
    m.insert("encrypted", enc);
    if (enc)
    {
        m.insert("encrypted_password", encryptedPassword());
        m.insert("char_count_hint", charCountHint());
        m.insert("algorithm", d->algorithm);
    }
    else
    {
        m.insert("open_password", d->open);
    }
    return BeQt::serialize(m);
}

bool BPassword::isEncrypted() const
{
    return d_func()->isEncrypted;
}

/*============================== Public operators ==========================*/

BPassword &BPassword::operator =(const BPassword &other)
{
    B_D(BPassword);
    const BPasswordPrivate *dd = other.d_func();
    d->mode = dd->mode;
    d->open = dd->open;
    d->encrypted = dd->encrypted;
    d->charCount = dd->charCount;
    d->algorithm = dd->algorithm;
    d->isEncrypted = dd->isEncrypted;
    return *this;
}

bool BPassword::operator ==(const BPassword &other) const
{
    const B_D(BPassword);
    const BPasswordPrivate *dd = other.d_func();
    if (d->mode != dd->mode || d->isEncrypted != dd->isEncrypted)
        return false;
    if (d->isEncrypted)
        return d->encrypted == dd->encrypted && d->charCount == dd->charCount && d->algorithm == dd->algorithm;
    else
        return d->open == dd->open;
}

BPassword::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*============================== Public friend operators ===================*/

QDataStream &operator <<(QDataStream &stream, const BPassword &pwd)
{
    const BPasswordPrivate *d = pwd.d_func();
    QVariantMap m;
    m.insert("mode", d->mode);
    m.insert("open", d->open);
    m.insert("encrypted", d->encrypted);
    m.insert("char_count", d->charCount);
    m.insert("algorythm", d->algorithm);
    m.insert("is_encrypted", d->isEncrypted);
    stream << m;
    return stream;
}

QDataStream &operator >>(QDataStream &stream, BPassword &pwd)
{
    BPasswordPrivate *d = pwd.d_func();
    QVariantMap m;
    stream >> m;
    d->mode = static_cast<BPassword::Mode>(m.value("mode", BPassword::FlexibleMode).toInt());
    d->open = m.value("open").toString();
    d->encrypted = m.value("encrypted").toByteArray();
    d->charCount = m.value("char_count").toInt();
    d->algorithm = static_cast<QCryptographicHash::Algorithm>(m.value("algorythm", QCryptographicHash::Sha1).toInt());
    d->isEncrypted = m.value("is_encrypted").toBool();
    return stream;
}

QDebug operator <<(QDebug dbg, const BPassword &pwd)
{
    dbg.nospace() << "BPassword(" << pwd.mode() << "," << pwd.openPassword() << ")";
    return dbg.space();
}
