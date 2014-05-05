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

#include "btranslatefunctor.h"
#include "btranslator.h"
#include "bglobal.h"
#include "bbase_p.h"

#include <QString>

#include <QDebug>

/*============================================================================
================================ BTranslateFunctorPrivate ====================
============================================================================*/

class B_CORE_EXPORT BTranslateFunctorPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTranslateFunctor)
public:
    explicit BTranslateFunctorPrivate(BTranslateFunctor *q);
    ~BTranslateFunctorPrivate();
public:
    void init();
public:
    BTranslator *translator;
private:
    Q_DISABLE_COPY(BTranslateFunctorPrivate)
};

/*============================================================================
================================ BTranslateFunctorPrivate ====================
============================================================================*/

/*============================== Public constructors =======================*/

BTranslateFunctorPrivate::BTranslateFunctorPrivate(BTranslateFunctor *q) :
    BBasePrivate(q)
{
    //
}

BTranslateFunctorPrivate::~BTranslateFunctorPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BTranslateFunctorPrivate::init()
{
    translator = 0;
}

/*============================================================================
================================ BTranslateFunctor ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BTranslateFunctor::BTranslateFunctor(BTranslator *t) :
    BBase(*new BTranslateFunctorPrivate(this))
{
    d_func()->init();
    d_func()->translator = t;
}

BTranslateFunctor::BTranslateFunctor(const BTranslateFunctor &other) :
   BBase(*new BTranslateFunctorPrivate(this))
{
    d_func()->init();
    *this = other;
}

BTranslateFunctor::~BTranslateFunctor()
{
    //
}

/*============================== Public methods ============================*/

void BTranslateFunctor::setTranslator(BTranslator *t)
{
    d_func()->translator = t;
}

BTranslator *BTranslateFunctor::translator() const
{
    return d_func()->translator;
}

/*============================== Public operators ==========================*/

BTranslateFunctor &BTranslateFunctor::operator =(const BTranslateFunctor &other)
{
    d_func()->translator = other.d_func()->translator;
    return *this;
}

QString BTranslateFunctor::operator ()(const char *context, const char *sourceText, const char *disambiguation,
                                       int n) const
{
    return d_func()->translator ? d_func()->translator->translate(context, sourceText, disambiguation, n) :
                                  QString(sourceText);
}
