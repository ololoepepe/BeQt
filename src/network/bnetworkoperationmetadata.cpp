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

#include <BeQtCore/BUuid>
#include <BeQtCore/private/bbase_p.h>

#include <QString>

/*============================================================================
================================ BNetworkOperationMetaDataPrivate ============
============================================================================*/

class BNetworkOperationMetaDataPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BNetworkOperationMetaData)
public:
    BUuid id;
    QString operation;
    bool request;
public:
    explicit BNetworkOperationMetaDataPrivate(BNetworkOperationMetaData *q);
    ~BNetworkOperationMetaDataPrivate();
public:
    void init();
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
    BBase(*new BNetworkOperationMetaDataPrivate(this))
{
    d_func()->init();
}

BNetworkOperationMetaData::BNetworkOperationMetaData(const BNetworkOperationMetaData &other) :
    BBase(*new BNetworkOperationMetaDataPrivate(this))
{
    d_func()->init();
    *this = other;
}

BNetworkOperationMetaData::BNetworkOperationMetaData(const BUuid &id, bool request, const QString &operation) :
    BBase(*new BNetworkOperationMetaDataPrivate(this))
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

const BUuid BNetworkOperationMetaData::id() const
{
    return d_func()->id;
}

void BNetworkOperationMetaData::invalidate()
{
    B_D(BNetworkOperationMetaData);
    d->id = BUuid();
    d->request = true;
    d->operation.clear();
}

bool BNetworkOperationMetaData::isRequest() const
{
    return d_func()->request;
}

bool BNetworkOperationMetaData::isValid() const
{
    const B_D(BNetworkOperationMetaData);
    return !d->id.isNull() && !d->operation.isEmpty();
}

const QString BNetworkOperationMetaData::operation() const
{
    return d_func()->operation;
}

void BNetworkOperationMetaData::setId(const BUuid &id)
{
    d_func()->id = id;
}

void BNetworkOperationMetaData::setIsRequest(bool request)
{
    d_func()->request = request;
}

void BNetworkOperationMetaData::setOperation(const QString &operation)
{
    d_func()->operation = operation;
}

/*============================== Public operators ==========================*/

BNetworkOperationMetaData &BNetworkOperationMetaData::operator=(const BNetworkOperationMetaData &other)
{
    setId(other.id());
    setIsRequest(other.isRequest());
    setOperation(other.operation());
    return *this;
}

bool BNetworkOperationMetaData::operator==(const BNetworkOperationMetaData &other) const
{
    return (id() == other.id()) && (isRequest() == other.isRequest()) && (operation() == other.operation());
}

bool BNetworkOperationMetaData::operator<(const BNetworkOperationMetaData &other) const
{
    return id() < other.id();
}
