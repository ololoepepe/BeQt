/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtNetwork module of the BeQt library.
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

#ifndef BEMAIL_H
#define BEMAIL_H

class BEmailPrivate;

class QString;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QStringList>

/*============================================================================
================================ BEmail ======================================
============================================================================*/

class B_NETWORK_EXPORT BEmail : public BBase
{
    B_DECLARE_PRIVATE(BEmail)
public:
    explicit BEmail();
    BEmail(const BEmail &other);
    ~BEmail();
protected:
    explicit BEmail(BEmailPrivate &d);
public:
    void setSender(const QString &sender);
    void setReceiver(const QString &receiver);
    void setReceivers(const QStringList &list);
    void setSubject(const QString &subject);
    void setBody(const QString &body);
    QString sender() const;
    QString receiver(int index = 0) const;
    QStringList receivers() const;
    int receiversCount() const;
    QString subject() const;
    QString body() const;
    bool isValid() const;
public:
    BEmail &operator =(const BEmail &other);
    bool operator ==(const BEmail &other) const;
};

#endif // BEMAIL_H
