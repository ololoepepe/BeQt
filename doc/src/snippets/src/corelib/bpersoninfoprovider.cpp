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
#include <BPersonInfoProvider>

#include <QDebug>
#include <QLocale>
#include <QString>

int main(int argc, char **argv)
{
    BPersonInfoProvider provider("/path/to/authors.beqt-info");

    qDebug() << provider.infosString(QLocale("en_US"));

    //Name: Andrey Bogdanov
    //Role: Main developer
    //Website: https://github.com/ololoepepe
    //E-mail: ololoepepe@gmail.com

    return 0;
}
//! [0]
