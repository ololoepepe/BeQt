#include "bnetworkoperationmetadata.h"

#include <BeQtCore/private/bbase_p.h>

#include <QString>
#include <QUuid>

/*============================================================================
================================ Network Operation Meta Data Private (declaration)
============================================================================*/

class BNetworkOperationMetaDataPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BNetworkOperationMetaData)
public:
    explicit BNetworkOperationMetaDataPrivate(BNetworkOperationMetaData *q);
    ~BNetworkOperationMetaDataPrivate();
    //
    void init();
    //
    QUuid id;
    bool request;
    QString operation;
private:
    Q_DISABLE_COPY(BNetworkOperationMetaDataPrivate)
};

/*============================================================================
================================ Network Operation Meta Data Private (definition)
============================================================================*/

BNetworkOperationMetaDataPrivate::BNetworkOperationMetaDataPrivate(BNetworkOperationMetaData *q) :
    BBasePrivate(q)
{
    request = true;
}

BNetworkOperationMetaDataPrivate::~BNetworkOperationMetaDataPrivate()
{
    //
}

//

void BNetworkOperationMetaDataPrivate::init()
{
    //
}

/*============================================================================
================================ Network Operation Meta Data
============================================================================*/

BNetworkOperationMetaData::BNetworkOperationMetaData() :
    BBase( *new BNetworkOperationMetaDataPrivate(this) )
{
    d_func()->init();
}

BNetworkOperationMetaData::BNetworkOperationMetaData(const BNetworkOperationMetaData &other) :
    BBase( *new BNetworkOperationMetaDataPrivate(this) )
{
    d_func()->init();
    *this = other;
}

BNetworkOperationMetaData::BNetworkOperationMetaData(const QUuid &id, bool request, const QString &operation) :
    BBase( *new BNetworkOperationMetaDataPrivate(this) )
{
    d_func()->init();
    setId(id);
    setIsRequest(request);
    setOperation(operation);
}

//

void BNetworkOperationMetaData::setIsRequest(bool request)
{
    d_func()->request = request;
}

void BNetworkOperationMetaData::setId(const QUuid &id)
{
    d_func()->id = id;
}

void BNetworkOperationMetaData::setOperation(const QString &operation)
{
    d_func()->operation = operation;
}

void BNetworkOperationMetaData::invalidate()
{
    B_D(BNetworkOperationMetaData);
    d->id = QUuid();
    d->request = true;
    d->operation.clear();
}

const QUuid BNetworkOperationMetaData::id() const
{
    return d_func()->id;
}

bool BNetworkOperationMetaData::isRequest() const
{
    return d_func()->request;
}

const QString BNetworkOperationMetaData::operation() const
{
    return d_func()->operation;
}

bool BNetworkOperationMetaData::isValid() const
{
    const B_D(BNetworkOperationMetaData);
    return !d->id.isNull() && !d->operation.isEmpty();
}

//

BNetworkOperationMetaData &BNetworkOperationMetaData::operator=(const BNetworkOperationMetaData &other)
{
    setId( other.id() );
    setIsRequest( other.isRequest() );
    setOperation( other.operation() );
    return *this;
}

bool BNetworkOperationMetaData::operator==(const BNetworkOperationMetaData &other) const
{
    return id() == other.id() && isRequest() == other.isRequest() && operation() == other.operation();
}

bool BNetworkOperationMetaData::operator<(const BNetworkOperationMetaData &other) const
{
    return id() < other.id();
}
