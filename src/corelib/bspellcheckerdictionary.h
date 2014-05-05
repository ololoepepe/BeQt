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

#ifndef BSPELLCHECKERDICTIONARY_H
#define BSPELLCHECKERDICTIONARY_H

class BSpellCheckerDictionaryPrivate;

class QString;
class QTextCodec;
class QLocale;

#include "bglobal.h"
#include "bbase.h"

#include <QStringList>

/*============================================================================
================================ BSpellCheckerDictionary =====================
============================================================================*/

class B_CORE_EXPORT BSpellCheckerDictionary : public BBase
{
    B_DECLARE_PRIVATE(BSpellCheckerDictionary)
public:
    explicit BSpellCheckerDictionary(const QString &path);
    ~BSpellCheckerDictionary();
public:
    bool spell(const QString &word) const;
    QStringList suggest(const QString &word) const;
    void addWord(const QString &word);
    void removeWord(const QString &word);
    QString path() const;
    QLocale locale() const;
    QTextCodec *codec() const;
    QString codecName() const;
    bool isValid() const;
private:
    Q_DISABLE_COPY(BSpellCheckerDictionary)
};

#endif // BSPELLCHECKERDICTIONARY_H
