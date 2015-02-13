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
#include <BCoreApplication>
#include <BLogger>

#include <QDir>
#include <QString>

int main(int argc, char **argv)
{
    BCoreApplication app(argc, argv, "myapp", "user");

    bLog("Application started"); //Logs to stdout

    app.logger()->setDateTimeFormat("yyyy.MM.dd hh:mm:ss");
    QString logfn = QDir::homePath() + ".myapp/logs/";
    logfn += QDateTime::currentDateTime().toString("yyyy.MM.dd-hh.mm.ss") + ".txt";
    app.logger()->setFileName(logfn);
    app.logger()->setLogToFileEnabled(true);

    bLog("Application finished"); //Logs both to stdout and to file

    return 0;
}
//! [0]
