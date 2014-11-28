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

int main(int argc, char **argv)
{
    BCoreApplication app(argc, argv, "myapp", "user");
    
    app.loadPlugins();
    app.setPluginActivated("some_plugin", false);
    
    return 0;
}
//! [0]

//! [1]
#include "MyWidget.h" //Custom QWidget-based class

#include <BApplication>
#include <BTranslator>

int main(int argc, char **argv)
{
    BApplication app(argc, argv, "myapp", "user");
    
    app.installBeQtTranslator("myapp");
    app.installBeQtTranslator(new BTranslator("extra"));
    
    MyWidget *w = new MyWidget;
    connect(bApp, SIGNAL(languageChanged()), w, SLOT(retranslateUi()));
    w.show();
    
    app.setLocale(QLocale("ru_RU"));
    
    return app.exec();
}
//! [1]

//! [2]
#include <BApplication>

#include <QDebug>
#include <QString>

int main(int argc, char **argv)
{
    BApplication app(argc, argv, "myapp", "user");
    
    app.setApplicationVersion("1.0.0");
    app.setOrganizationDomain("http://myapp.user.com/home");
    app.setApplicationCopyrightPeriod("2012-2014");
    
    app.setApplicationDescriptionFile(":/DESCRIPTION.txt");
    app.setApplicationChangeLogFile(":/ChangeLog.txt");
    app.setApplicationLicenseFile(":/COPYING.txt");
    app.setApplicationAuthorsFile(":/infos/authors.beqt-info";
    app.setApplicationTranslationsFile(":/infos/translators.beqt-info");
    app.setApplicationThanksToFile(":/infos/thanks-to.beqt-info");
    
    qDebug() << app.beqtInfo(BApplication::Translators);
    
    //Output:
    //"Name: Andrey Bogdanov
    //Role: Translator
    //Website: https://github.com/ololoepepe
    //E-mail: ololoepepe@gmail.com"
    
    return 0;
}
//! [2]
