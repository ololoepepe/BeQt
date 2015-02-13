/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the documentation of the BeQt library.
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

//! [0]
#include <BAbstractLocationProvider>
#include <BApplication>
#include <BDirTools>

#include <QDebug>
#include <QStringList>

class MyLocationProvider : public BAbstractLocationProvider
{
public:
    MyLocationProvider() {}

    bool canCreateLocationPath(const QString &locationName, BApplicationBase::ResourceType type) const
    {
        return ("my_location" == locationName && BApplication::UserResource == type);
    }

    bool createLocationPath(const QString &locationName, BApplicationBase::ResourceType type)
    {
        return canCreateLocationPath(locationName, type) && BDirTools::mkpath("/home/user/my_dir");
    }

    QStringList locationNames() const { return QStringList() << "my_location"; }

    QString locationPath(const QString &locationName, BApplicationBase::ResourceType type) const
    {
        if ("my_location" != locationName || BApplication::UserResource != type)
            return QString();
        return "/home/user/my_dir";
    }
};

int main(int argc, char **argv)
{
    BApplication app(argc, argv, "myapp", "user");
    
    MyLocationProvider *provider = new MyLocationProvider;
    BApplication::installLocationProvider(provider);

    qDebug() << BApplication::location("my_location", BApplication::UserResource); //Output: "/home/user/my_dir"
    
    return 0;
}
//! [0]
