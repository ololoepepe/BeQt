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
#include <BApplication>
#include <BSpamNotifier>

#include <QMessageBox>
#include <QPushButton>
#include <QString>

int main(int argc, char **argv)
{
    BApplication app(argc, argv, "myapp", "orgname");

    QMessageBox msg;
    msg.setIcon(QMessageBox::Information);
    msg.setText("Congratulations! You are clicking fast enough.");

    QPushButton btn("Click me!");

    BSpamNotifier notifier(0, 500, 2); //A user must click at least 2 times in a half of a second
    connect(&btn, SIGNAL(clicked()), &notifier, SLOT(spam()));
    connect(&notifier, SIGNAL(spammed(int)), &msg, SLOT(show()));

    btn.show();

    return app.exec();
}
//! [0]
