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
#include <BDynamicTranslator>

#include <QString>
#include <QWidget>

int main(int argc, char **argv)
{
    BApplication app(argc, argv, "myapp", "user");
    app.installBeqtTranslator("myapp");
    
    BTranslation t = BTranslation::translate("main", "Widget title");
    
    QWidget *w = new QWidget;
    w->setWindowTitle(t.translate());
    new BDynamicTranslator(w, "windowTitle", t);
    w.show();
    
    return app.exec();
}
//! [0]
