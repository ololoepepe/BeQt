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

class QByteArray;
class QLocale;
class QString;
class QStringList;
class QTextCodec;

#include "bbase.h"

/*============================================================================
================================ BSpellCheckerDictionary =====================
============================================================================*/

class B_CORE_EXPORT BSpellCheckerDictionary : public BBase
{
    B_DECLARE_PRIVATE(BSpellCheckerDictionary)
public:
    explicit BSpellCheckerDictionary(const QString &path);
    explicit BSpellCheckerDictionary(const QByteArray &affixData, const QByteArray &dictionaryData,
                                     const QString &locale);
    explicit BSpellCheckerDictionary(const QByteArray &affixData, const QByteArray &dictionaryData,
                                     const QLocale &locale);
    ~BSpellCheckerDictionary();
public:
    void addWord(const QString &word);
    QByteArray affixData() const;
    QTextCodec *codec() const;
    QString codecName() const;
    QByteArray dictionaryData() const;
    bool isValid() const;
    QLocale locale() const;
    QString localeName() const;
    QString path() const;
    void removeWord(const QString &word);
    bool spell(const QString &word) const;
    QStringList suggest(const QString &word) const;
private:
    Q_DISABLE_COPY(BSpellCheckerDictionary)
};

#endif // BSPELLCHECKERDICTIONARY_H
