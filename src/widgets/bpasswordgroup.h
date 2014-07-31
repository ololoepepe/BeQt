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

#ifndef BPASSWORDGROUP_H
#define BPASSWORDGROUP_H

class BPasswordGroupPrivate;

class BPasswordWidget;

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BPassword>

#include <QObject>

/*============================================================================
================================ BPasswordGroup ==============================
============================================================================*/

class B_WIDGETS_EXPORT BPasswordGroup : public QObject, public BBaseObject
{
    Q_OBJECT
    Q_PROPERTY(BPassword password READ password STORED false)
    Q_PROPERTY(bool passwordsMatch READ passwordsMatch NOTIFY passwordsMatchChanged STORED false)
    Q_PROPERTY(bool passwordsMatchAndAcceptable READ passwordsMatchAndAcceptable
               NOTIFY passwordsMatchAndAcceptableChanged STORED false)
    B_DECLARE_PRIVATE(BPasswordGroup)
public:
    explicit BPasswordGroup(QObject *parent = 0);
    ~BPasswordGroup();
protected:
    explicit BPasswordGroup(BPasswordGroupPrivate &d, QObject *parent = 0);
public:
    void addPasswordWidget(BPasswordWidget *pwdwgt);
    BPassword password() const;
    bool passwordsMatch() const;
    bool passwordsMatchAndAcceptable() const;
Q_SIGNALS:
    void passwordsMatchAndAcceptableChanged(bool match);
    void passwordsMatchChanged(bool match);
private:
    Q_DISABLE_COPY(BPasswordGroup)
};

#endif // BPASSWORDGROUP_H
