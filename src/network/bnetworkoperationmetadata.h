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

#ifndef BNETWORKOPERATIONMETADATA_H
#define BNETWORKOPERATIONMETADATA_H

class BNetworkOperationMetaDataPrivate;
class BUuid;

class QString;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

/*============================================================================
================================ BNetworkOperationMetaData ===================
============================================================================*/

class B_NETWORK_EXPORT BNetworkOperationMetaData : public BBase
{
    B_DECLARE_PRIVATE(BNetworkOperationMetaData)
public:
    explicit BNetworkOperationMetaData();
    BNetworkOperationMetaData(const BNetworkOperationMetaData &other); //Should not be explicit
    explicit BNetworkOperationMetaData(const BUuid &id, bool request, const QString &operation);
protected:
    explicit BNetworkOperationMetaData(BNetworkOperationMetaDataPrivate &d);
public:
    void setId(const BUuid &id);
    void setIsRequest(bool request);
    void setOperation(const QString &operation);
    void invalidate();
    const BUuid id() const;
    bool isRequest() const;
    const QString operation() const;
    bool isValid() const;
public:
    BNetworkOperationMetaData &operator=(const BNetworkOperationMetaData &other);
    bool operator==(const BNetworkOperationMetaData &other) const;
    bool operator<(const BNetworkOperationMetaData &other) const;
};

#endif // BNETWORKOPERATIONMETADATA_H
