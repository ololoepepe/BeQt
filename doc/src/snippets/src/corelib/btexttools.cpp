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
QString where = "fork for a foreginer";
QString what = "for";
BTextTools::indexOf(where, what, 0, Qt::Caseinsensitive, false);    // returns 0
BTextTools::indexOf(where, what, 0, Qt::Caseinsensitive, true);     // returns 5
//! [0]

//! [1]
QString where = "fork for a foreginer";
QString what = "for";
BTextTools::lastIndexOf(where, what, -1, Qt::Caseinsensitive, false);    // returns 11
BTextTools::lastIndexOf(where, what, -1, Qt::Caseinsensitive, true);     // returns 5
//! [1]

//! [2]
QString text = "10 may 2014, eleventh of may 2014";
QRegExp what("\\w+");
QRegExp prefix("\\d+\\s");
QRegExp postfix("\\s\\d+");

BTextMatchList list = BTextTools::match(text, what, prefix, postfix);
qDebug() << list.size(); //will print: 1
qDebug() << list.texts(); //will print: QStringList("may")
//! [2]

//! [3]
"identifier:-id" //an option "-id" without any values
"identifier:-key=" //an option "-key" which must take any value, e.g. "-key=10"
"identifier:-key=1|2|3" //an option "-key" which must take a value of 1, 2, or 3, e.g. "-key=1"
"identifier:-key=,[other:-o]" //an obligatory option "-key" and a non-obligatory option "-o"
//the following cases are both valid:
//"-key=100"
//"-key=100 -o"
//! [3]

//! [4]
QStringList list = QStringList() << "a" << "abbot" << "z" << "zbot"  << "x" << "y";
BTextTools::sortComprising(&list); //"abbot", "zbot", "x", "y", "x", "a"
//! [4]
