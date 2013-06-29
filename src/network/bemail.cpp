#include "bemail.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BeQt>

#include <QObject>
#include <QString>
#include <QStringList>

/*============================================================================
================================ BEmailPrivate ==========================
============================================================================*/

class BEmailPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BEmail)
public:
    explicit BEmailPrivate(BEmail *q);
    ~BEmailPrivate();
public:
    void init();
public:
    QString sender;
    QStringList receivers;
    QString subject;
    QString body;
private:
    Q_DISABLE_COPY(BEmailPrivate)
};

/*============================================================================
================================ BEmailPrivate ===============================
============================================================================*/

/*============================== Public constructors =======================*/

BEmailPrivate::BEmailPrivate(BEmail *q) :
    BBasePrivate(q)
{
    //
}

BEmailPrivate::~BEmailPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BEmailPrivate::init()
{
    //
}

/*============================================================================
================================ BEmail ======================================
============================================================================*/

/*============================== Public constructors =======================*/

BEmail::BEmail() :
    BBase(*new BEmailPrivate(this))
{
    d_func()->init();
}

BEmail::BEmail(const BEmail &other) :
    BBase(*new BEmailPrivate(this))
{
    d_func()->init();
    *this = other;
}

BEmail::~BEmail()
{
    //
}

/*============================== Protected constructors ====================*/

BEmail::BEmail(BEmailPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BEmail::setSender(const QString &sender)
{
    d_func()->sender = sender;
}

void BEmail::setReceiver(const QString &receiver)
{
    setReceivers(QStringList() << receiver);
}

void BEmail::setReceivers(const QStringList &list)
{
    B_D(BEmail);
    d->receivers.clear();
    foreach (const QString &r, list)
        if (!r.isEmpty())
            d->receivers << r;
}

void BEmail::setSubject(const QString &subject)
{
    d_func()->subject = subject;
}

void BEmail::setBody(const QString &body)
{
    d_func()->body = body;
}

QString BEmail::sender() const
{
    return d_func()->sender;
}

QString BEmail::receiver(int index) const
{
    const B_D(BEmail);
    return (!d->receivers.isEmpty() && index >= 0 && index < d->receivers.size()) ? d->receivers.at(index) : QString();
}

QStringList BEmail::receivers() const
{
    return d_func()->receivers;
}

int BEmail::receiversCount() const
{
    return d_func()->receivers.size();
}

QString BEmail::subject() const
{
    return d_func()->subject;
}

QString BEmail::body() const
{
    return d_func()->body;
}

bool BEmail::isValid() const
{
    const B_D(BEmail);
    return !d->receivers.isEmpty();
}

/*============================== Public operators ==========================*/

BEmail &BEmail::operator =(const BEmail &other)
{
    B_D(BEmail);
    const BEmailPrivate *dd = other.d_func();
    d->sender = dd->sender;
    d->receivers = dd->receivers;
    d->subject = dd->subject;
    d->body = dd->body;
    return *this;
}

bool BEmail::operator ==(const BEmail &other) const
{
    const B_D(BEmail);
    const BEmailPrivate *dd = other.d_func();
    return d->sender == dd->sender && d->receivers == dd->receivers && d->subject == dd->subject
            && d->body == dd->body;
}
