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
int fileSize(const QString &fileName, bool *ok = 0)
{
    QFile f(fileName);
    if (!f.exists())
        return bRet(ok, false, 0);
    //Instead of:
    //if (ok) ok = false;
    //return 0;
    return bRet(ok, true, f.size());
    //Instead of:
    //if (ok) ok = true;
    //return f.size();
}
//! [0]

//! [1]
enum MyEnum
{
    Zero = 0,
    One,
    Two,
    Three
};

QVariant v = getTheValue();

MyEnum e = enum_cast(v, QList<MyEnum>() << Zero << One << Two << Three, Zero);
//! [1]

//! [2]
bool isValid(const QString &s)
{
    init_once(QString, list, QString()) { //This block of code is called only once
        list << "one";
        list << "two";
        list << "three";
        list << "four";
        //100 more strings
    }
    return list.contains(s);
}
//! [2]

//! [3]
MyObject *obj = new MyObject;
QLineEdit *ledt = new QLineEdit;
QSignalMapper *mpr = new QSignalMapper;

connect(mpr, SIGNAL(mapped(QString)), ledt, SLOT(setText(QString)));

bSetMapping(mpr, obj, SIGNAL(someSignal()), QString("some string"));
//Instead of:
//mpr->setMapping(obj, QString("some string"));
//connect(obj, SIGNAL(someSignal()), mpr, SLOT(map()));
//! [3]
