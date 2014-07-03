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

#ifndef BPERSONINFOPROVIDER_P_H
#define BPERSONINFOPROVIDER_P_H

class QString;

#include "bpersoninfoprovider.h"
#include "bpersoninfo.h"

#include "bglobal.h"
#include "bbase_p.h"

#include <QList>
#include <QMap>

/*============================================================================
================================ BPersonInfoProviderPrivate ==================
============================================================================*/

class B_CORE_EXPORT BPersonInfoProviderPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPersonInfoProvider)
public:
    typedef QMap<QString, BPersonInfo> PersonInfoMap;
public:
    explicit BPersonInfoProviderPrivate(BPersonInfoProvider *q);
    ~BPersonInfoProviderPrivate();
public:
    static BPersonInfo infoForLocale(const PersonInfoMap &map, const QString &localeName, bool noDefault);
    static void tryAppendInfo(QList<PersonInfoMap> &where, PersonInfoMap what);
public:
    void init();
    void setFileName(const QString &fn);
private:
    QString fileName;
    QList<PersonInfoMap> infos;
private:
    Q_DISABLE_COPY(BPersonInfoProviderPrivate)
};

#endif // BPERSONINFOPROVIDER_P_H
