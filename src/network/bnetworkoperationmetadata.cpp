/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtNetwork module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "bnetworkoperationmetadata.h"

#include <BeQtCore/private/bbase_p.h>

#include <QString>
#include <QUuid>

/*============================================================================
================================ BNetworkOperationMetaDataPrivate ============
============================================================================*/

class BNetworkOperationMetaDataPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BNetworkOperationMetaData)
public:
    explicit BNetworkOperationMetaDataPrivate(BNetworkOperationMetaData *q);
    ~BNetworkOperationMetaDataPrivate();
public:
    void init();
public:
    QUuid id;
    bool request;
    QString operation;
private:
    Q_DISABLE_COPY(BNetworkOperationMetaDataPrivate)
};

/*============================================================================
================================ BNetworkOperationMetaDataPrivate ============
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkOperationMetaDataPrivate::BNetworkOperationMetaDataPrivate(BNetworkOperationMetaData *q) :
    BBasePrivate(q)
{
    request = true;
}

BNetworkOperationMetaDataPrivate::~BNetworkOperationMetaDataPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BNetworkOperationMetaDataPrivate::init()
{
    //
}

/*============================================================================
================================ BNetworkOperationMetaData ===================
============================================================================*/

/*============================== Public constructors =======================*/

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

/*============================== Protected constructors ====================*/

BNetworkOperationMetaData::BNetworkOperationMetaData(BNetworkOperationMetaDataPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

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

/*============================== Public operators ==========================*/

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
