/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
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

#include "blocationprovider.h"

#include "bbase_p.h"
#include "bapplicationbase.h"

#include <QMap>
#include <QString>

/*============================================================================
================================ BLocationProviderPrivate ====================
============================================================================*/

class BLocationProviderPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BLocationProvider)
public:
    explicit BLocationProviderPrivate(BLocationProvider *q);
    ~BLocationProviderPrivate();
public:
    void init();
public:
    QString name;
    QMap<BApplicationBase::ResourcesType, QString> paths;
private:
    Q_DISABLE_COPY(BLocationProviderPrivate)
};

/*============================================================================
================================ BLocationProviderPrivate ====================
============================================================================*/

/*============================== Public constructors =======================*/

BLocationProviderPrivate::BLocationProviderPrivate(BLocationProvider *q) :
    BBasePrivate(q)
{
    //
}

BLocationProviderPrivate::~BLocationProviderPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BLocationProviderPrivate::init()
{
    //
}

/*============================================================================
================================ BLocationProvider ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BLocationProvider::BLocationProvider(const QString &locationName) :
    BAbstractLocationProvider(), BBase(*new BLocationProviderPrivate(this))
{
    d_func()->init();
    d_func()->name = locationName;
}

BLocationProvider::BLocationProvider(const BLocationProvider &other) :
    BAbstractLocationProvider(), BBase(*new BLocationProviderPrivate(this))
{
    *this = other;
}

BLocationProvider::~BLocationProvider()
{
    //
}

/*============================== Protected constructors ====================*/

BLocationProvider::BLocationProvider(BLocationProviderPrivate &d) :
    BBase(d)
{
    //
}

/*============================== Public methods ============================*/

void BLocationProvider::setLocationName(const QString &name)
{
    d_func()->name = name;
}

void BLocationProvider::setLocationPath(BApplicationBase::ResourcesType type, const QString &path)
{
    if (!path.isEmpty())
        d_func()->paths.insert(type, path);
    else
        d_func()->paths.remove(type);
}

QString BLocationProvider::locationName() const
{
    return d_func()->name;
}

QString BLocationProvider::locationPath(BApplicationBase::ResourcesType type) const
{
    return d_func()->paths.value(type);
}

bool BLocationProvider::isValid() const
{
    if (d_func()->name.isEmpty())
        return false;
    foreach (const QString &p, d_func()->paths.values()) {
        if (!p.isEmpty())
            return true;
    }
    return false;
}

/*============================== Public operators ==========================*/

BLocationProvider &BLocationProvider::operator=(const BLocationProvider &other)
{
    B_D(BLocationProvider);
    const BLocationProviderPrivate *dd = other.d_func();
    d->name = dd->name;
    d->paths = dd->paths;
    return *this;
}

bool BLocationProvider::operator==(const BLocationProvider &other) const
{
    const B_D(BLocationProvider);
    const BLocationProviderPrivate *dd = other.d_func();
    return d->name == dd->name && d->paths == dd->paths;
}

bool BLocationProvider::operator!=(const BLocationProvider &other) const
{
    return !(*this == other);
}
