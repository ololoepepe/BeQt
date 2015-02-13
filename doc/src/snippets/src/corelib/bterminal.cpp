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
#include <BTerminal>

#include <QString>

int main(int argc, char **argv)
{
    BTerminal::setTitle("My Application v0.1.0");

    BTerminal::setBackgroundColor(BTerminal::Lightgray);
    BTerminal::setColor(BTerminal::Red);

    BTerminal::writeLine("Hello, BeQt!");
    QString s = BTerminal::readLine("Enter your name: ");

    if (s.isEmpty()) {
        BTerminal::writeLineErr("Empty name! Will now exit...");
        return -1;
    }

    QString pwd = BTerminal::readLineSecure(s + ", enter your password, please: ");
    bWriteLine("Aha! Your password is " + pwd);

    return 0;
}
//! [0]

//! [1]
#include <BCoreApplication>
#include <BTerminal>
#include <BTranslation>

#include <QString>
#include <QStringList>

static bool handleHelloCommand(const QString &, const QStringList &args)
{
    bWrite("Hello, BeQt! ");
    bWriteLine("Number of arguments: " + QStrin::number(args.size()));
    return true;
}

int main(int argc, char **argv)
{
    BCoreApplication app(argc, argv, "myapp", "orgname");

    BTerminal::setMode(StandardMode);

    BTerminal::installHandler("hello", &handleHelloCommand); //Custom command handler
    BTerminal::installHandler(BTerminal::QuitCommand); //Standard command handler provided by BeQt

    BTerminal::CommandHelp h;
    h.usage = "hello [args...]";
    h.description = BTranslation::tr("Prints \"Hello, BeQt!\" and shows the number of arguments");
    BTerminal::setCommandHelp("hello", h);

    return app.exec();
}
//! [1]

//! [2]
#include <BCoreApplication>
#include <BSettingsNode>
#include <BTerminal>
#include <BTranslation>

#include <QString>

int main(int argc, char **argv)
{
    BCoreApplication app(argc, argv, "myapp", "orgname");

    BTerminal::setMode(StandardMode);

    BTerminal::installHandler(BTerminal::SetCommand);
    BTerminal::installHandler(BTerminal::QuitCommand);

    BSettingsNode *root = new BSettingsNode;
    BSettingsNode *n = new BSettingsNode("Core", root);
    BSettingsNode *nn = new BSettingsNode("my_string", n);
    nn->setDescription(BTranslation::tr("Some magic string."));
    BTerminal::createBeQtSettingsNode(root);
    BTerminal::setRootSettingsNode(root);

    //Enter "set --show BeQt.Core.locale" to see the current application locale name
    //Now enter "set Core.my_string <something>"
    //And then "set --show Core.my_string" to see how the setting changed

    return app.exec();
}
//! [2]
