/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the examples of the BeQt library.
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

#include <BCoreApplication>
#include <BLogger>
#include <BTerminalIOHandler>
#include <BTranslator>

#include <QCoreApplication>
#include <QString>
#include <QSize>

#include <QDebug>

int main(int argc, char **argv)
{
    //Creating QCoreApplication instance
    QCoreApplication *app = new QCoreApplication(argc, argv);
    Q_UNUSED(app)
    //QApplication initialization
    QCoreApplication::setApplicationName("Console app");
    QCoreApplication::setOrganizationName("Andrey Bogdanov");
    QCoreApplication::setOrganizationDomain("https://github.com/the-dark-angel");
    QCoreApplication::setApplicationVersion("0.1.0");
    //Creating BCoreApplication instance
    BCoreApplication *bapp = new BCoreApplication;
    Q_UNUSED(bapp)
    bLogger->setIncludeDateTime(false);
    bLog("Application started", BLogger::InfoLevel);
    BCoreApplication::installTranslator(new BTranslator("beqt"));
    //Communicating with user through BTerminalIOHandler
    QString s = BTerminalIOHandler::readLine("Enter something: ");
    BTerminalIOHandler::writeLine("You entered: \"" + s + "\"");
    QSize sz = BTerminalIOHandler::terminalSize();
    bWriteLine("By the way, terminal size is " + QString::number(sz.width()) + "x" + QString::number(sz.height()));
    bWriteLine("Goodbye!");
    return 0;
}
