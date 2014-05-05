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

#ifndef BTRANSLATION_H
#define BTRANSLATION_H

class BTranslationPrivate;

class QDataStream;
class QDebug;
class QVariant;

#include "bglobal.h"
#include "bbase.h"

#include <QString>
#include <QMetaType>

/*============================================================================
================================ BTranslation ================================
============================================================================*/

class B_CORE_EXPORT BTranslation : public BBase
{
    B_DECLARE_PRIVATE(BTranslation)
public:
    static BTranslation translate(const char *context, const char *sourceText, const char *disambiguation = 0,
                                  int n = -1);
    static BTranslation tr(const char *sourceText, const char *disambiguation = 0, int n = -1);
    static QString translate(const BTranslation &t);
    static QString tr(const BTranslation &t);
public:
    explicit BTranslation();
    BTranslation(const BTranslation &other);
    ~BTranslation();
protected:
    explicit BTranslation(BTranslationPrivate &d);
public:
    QString context() const;
    QString sourceText() const;
    QString disambiguation() const;
    int n() const;
    QString translate() const;
    QString tr() const;
    bool isValid() const;
public:
    BTranslation &operator =(const BTranslation &other);
    bool operator ==(const BTranslation &other) const;
    bool operator !=(const BTranslation &other) const;
    operator QString() const;
    operator QVariant() const;
public:
    friend QDataStream &operator <<(QDataStream &stream, const BTranslation &t);
    friend QDataStream &operator >>(QDataStream &stream, BTranslation &t);
    friend QDebug operator <<(QDebug dbg, const BTranslation &t);
};

typedef BTranslation BTr;

Q_DECLARE_METATYPE(BTranslation)

#endif // BTRANSLATION_H

