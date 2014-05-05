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

#ifndef BTRANSLATOR_P_H
#define BTRANSLATOR_P_H

class BTranslator;

class QString;
class QTranslator;

#include "bglobal.h"
#include "bbase_p.h"
#include "btranslator.h"

#include <QList>
#include <QtGlobal>
#include <QLocale>

/*============================================================================
================================ BTranslatorPrivate ==========================
============================================================================*/

class B_CORE_EXPORT BTranslatorPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BTranslator)
public:
    explicit BTranslatorPrivate(BTranslator *q);
    ~BTranslatorPrivate();
public:
    void init();
    bool install();
    void remove();
public:
    QString fileName;
    QLocale locale;
    QList<QTranslator *> translators;
    bool loaded;
    bool installed;
private:
    Q_DISABLE_COPY(BTranslatorPrivate)
};

#endif // BTRANSLATOR_P_H
