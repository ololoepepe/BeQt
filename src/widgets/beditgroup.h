/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtWidgets module of the BeQt library.
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

#ifndef BEDITGROUP_H
#define BEDITGROUP_H

class BEditGroupPrivate;

class QLineEdit;
class QString;

#include <BeQtCore/BBaseObject>

#include <QObject>

/*============================================================================
================================ BEditGroup ==================================
============================================================================*/

class B_WIDGETS_EXPORT BEditGroup : public QObject, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BEditGroup)
public:
    explicit BEditGroup(QObject *parent = 0);
    ~BEditGroup();
protected:
    explicit BEditGroup(BEditGroupPrivate &d, QObject *parent = 0);
public:
    void addEdit(QLineEdit *ledt);
    QString text() const;
    bool textsMatch() const;
    bool textsMatchAndAcceptable() const;
Q_SIGNALS:
    void textsMatchAndAcceptableChanged(bool match);
    void textsMatchChanged(bool match);
private:
    Q_DISABLE_COPY(BEditGroup)
};

#endif // BEDITGROUP_H
