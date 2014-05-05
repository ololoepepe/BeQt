/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
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

#ifndef BPERSONINFOPROVIDER_H
#define BPERSONINFOPROVIDER_H

class BPersonInfoProviderPrivate;

class QString;

#include "bglobal.h"
#include "bbase.h"
#include "bpersoninfo.h"
#include "bcoreapplication.h"

#include <QObject>
#include <QLocale>
#include <QList>

/*============================================================================
================================BPersonInfoProvider ==========================
============================================================================*/

class B_CORE_EXPORT BPersonInfoProvider : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPersonInfoProvider)
public:
    static QString infoListToString(const BPersonInfoList &list);
    static QString infosString(const BPersonInfoProvider *prov, bool noDefault = false,
                               const QLocale &locale = BCoreApplication::locale());
    static QString infosString(const BPersonInfoProvider *prov, const QLocale &locale, bool noDefault = false);
public:
    explicit BPersonInfoProvider(QObject *parent = 0);
    explicit BPersonInfoProvider(const QString &fileName, QObject *parent = 0);
    ~BPersonInfoProvider();
protected:
    explicit BPersonInfoProvider(BPersonInfoProviderPrivate &d, QObject *parent = 0);
public:
    void setFileName(const QString &fileName);
    QString fileName() const;
    BPersonInfoList infos(bool noDefault = false, const QLocale &locale = BCoreApplication::locale()) const;
    BPersonInfoList infos(const QLocale &locale, bool noDefault = false) const;
    QString infosString(bool noDefault = false, const QLocale &locale = BCoreApplication::locale()) const;
    QString infosString(const QLocale &locale, bool noDefault = false) const;
public Q_SLOTS:
    void reload();
Q_SIGNALS:
    void reloaded();
private:
    Q_DISABLE_COPY(BPersonInfoProvider)
};

#endif // BPERSONINFOPROVIDER_H
