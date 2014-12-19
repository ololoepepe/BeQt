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
#include "mymodel.h" //Some QAbstractItemModel-based model with a huge amount of data
#include "mysortmodel.h" //Some QSortFilterProxyModel-based filter model with an O(n) sorting algorythm

#include <BApplication>
#include <BSignalDelayProxy>

#include <QLineEdit>
#include <QString>
#include <QTableView>
#include <QWidget>
#include <QVBoxLayout>

int main(int argc, char **argv)
{
    BApplication app(argc, argv, "myapp", "orgname");

    MyModel model;
    model.loadData();

    MySortModel sortModel();
    sortModel.setSourceModel(&model);

    QWidget wgt;
    wgt.setLayout(new QVBoxLayout);
    QLineEdit *ledt = new QLineEdit;
    wgt.layout()->addWidget(ledt);
    QTableWidget *twgt = new QTableWidget;
    wgt.layout()->addWidget(twgt);

    twgt->setModel(&sortModel);

    BSignalDelayProxy proxy(1000, 2000);
    proxy.setStringConnection(ledt, SIGNAL(textChanged(QString)), &sortModel, SLOT(setSortingPattern(QString)));

    wgt.show();

    return app.exec();
}
//! [0]
