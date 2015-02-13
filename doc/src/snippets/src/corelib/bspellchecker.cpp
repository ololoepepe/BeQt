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
#include <BSpellChecker>

#include <QDebug>
#include <QString>

int main(int argc, char **argv)
{
    QString dictionaryPath = "/path/to/dictionary/en_US"; //directory
    QString userDictionaryPath = "/path/to/user/dictionary.txt"; //file

    BSpellChecker sc(dictionaryPath, userDictionaryPath);

    qDebug() << sc.spell("wor"); //false
    qDebug() << sc.spell("word"); //true

    sc.ignoreWord("wor");

    qDebug() << sc.spell("wor"); //true

    return 0;
}
//! [0]

//! [1]
BSpellChecker sc("/path/to/dictionary/en_US");
sc.spell("word"); //these is the same as below
sc.spell("or", "w", "d"); //this is the same as above
//! [1]
