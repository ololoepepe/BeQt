#include "bpassword.h"
#include "bglobal.h"
#include "bbase_p.h"

#include <QString>
#include <QByteArray>
#include <QCryptographicHash>

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
    charCount = 0;
    algorithm = QCryptographicHash::Sha1;
    isEncrypted = false;
}

/*============================================================================
================================ BPassword ===================================
============================================================================*/

/*============================== Public constructors =======================*/

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
    setEncryptedPassword(a, ba, charCountHint);
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

void BPassword::setPassword(const QString &s)
{
    clear();
    d_func()->open = s;
    d_func()->isEncrypted = false;
}

void BPassword::setEncryptedPassword(QCryptographicHash::Algorithm a, const QByteArray &ba, int charCountHint)
{
    clear();
    B_D(BPassword);
    d->encrypted = ba;
    d->charCount = (charCountHint > 0 && !ba.isEmpty()) ? charCountHint : 0;
    d->algorithm = a;
    d->isEncrypted = true;
}

void BPassword::encrypt(QCryptographicHash::Algorithm a)
{
    int count = 0;
    QByteArray ba = encryptedPassword(a, &count);
    setEncryptedPassword(a, ba, count);
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

QString BPassword::password() const
{
    return d_func()->open;
}

QByteArray BPassword::encryptedPassword() const
{
    return d_func()->encrypted;
}

QByteArray BPassword::encryptedPassword(QCryptographicHash::Algorithm a, int *charCountHint) const
{
    if (charCountHint)
        *charCountHint = d_func()->open.length();
    return QCryptographicHash::hash(d_func()->open.toUtf8(), a);
}

int BPassword::charCountHint() const
{
    return d_func()->charCount;
}

QCryptographicHash::Algorithm BPassword::algorithm() const
{
    return d_func()->algorithm;
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
    if (d->isEncrypted != dd->isEncrypted)
        return false;
    if (d->isEncrypted)
        return d->encrypted == dd->encrypted && d->charCount == dd->charCount && d->algorithm == dd->algorithm;
    else
        return d->open == dd->open;
}
