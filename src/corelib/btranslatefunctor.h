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

#ifndef BTRANSLATEFUNCTOR_H
#define BTRANSLATEFUNCTOR_H

class BTranslateFunctorPrivate;
class BTranslator;

class QString;

#include "bglobal.h"
#include "bbase.h"

/*============================================================================
================================ BTranslateFunctor ===========================
============================================================================*/

class B_CORE_EXPORT BTranslateFunctor : public BBase
{
    B_DECLARE_PRIVATE(BTranslateFunctor)
public:
    explicit BTranslateFunctor(BTranslator *t = 0);
    BTranslateFunctor(const BTranslateFunctor &other);
    ~BTranslateFunctor();
public:
    void setTranslator(BTranslator *t);
    BTranslator * translator() const;
public:
    BTranslateFunctor &operator =(const BTranslateFunctor &other);
    QString operator ()(const char *context, const char *sourceText, const char *disambiguation = 0, int n = -1) const;
};

#endif // BTRANSLATEFUNCTOR_H
