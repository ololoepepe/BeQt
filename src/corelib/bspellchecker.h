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

#ifndef BSPELLCHECKER_H
#define BSPELLCHECKER_H

class BSpellCheckerPrivate;

class BSpellCheckerDictionary;

class QLocale;
class QRegExp;
class QStringList;

#include "bbaseobject.h"

#include <QList>
#include <QObject>
#include <QString>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
class QRegularExpression; //It is here because Qt headers containing the wrapping macros must be included first
#endif

/*============================================================================
================================ BSpellChecker ===============================
============================================================================*/

class B_CORE_EXPORT BSpellChecker : public QObject, public BBaseObject
{
    Q_OBJECT
    Q_PROPERTY(QString userDictionary READ userDictionaryPath WRITE setUserDictionary)
    B_DECLARE_PRIVATE(BSpellChecker)
public:
    explicit BSpellChecker(QObject *parent = 0);
    explicit BSpellChecker(const QString &dictionaryPath, const QString &userDictionaryPath = QString());
    explicit BSpellChecker(QObject *parent, const QString &dictionaryPath,
                           const QString &userDictionaryPath = QString());
    explicit BSpellChecker(const QStringList &dictionaryPaths, const QString &userDictionaryPath = QString());
    explicit BSpellChecker(QObject *parent, const QStringList &dictionaryPaths,
                           const QString &userDictionaryPath = QString());
    ~BSpellChecker();
public:
    void addDictionary(const QString &path);
    void addDictionary(const QByteArray &affixData, const QByteArray &dictionaryData, const QString &locale);
    void addDictionary(const QByteArray &affixData, const QByteArray &dictionaryData, const QLocale &locale);
    void clearIgnored();
    void clearIgnoredImplicitly();
    void clearIgnoredImplicitlyRegExp();
    void considerLeftSurrounding(int charCount);
    void considerRightSurrounding(int charCount);
    QList<BSpellCheckerDictionary *> dictionaries() const;
    BSpellCheckerDictionary *dictionary(const QLocale &locale) const;
    BSpellCheckerDictionary *dictionary(const QString &localeName) const;
    QStringList dictionaryPaths() const;
    void ignoreImplicitlyRegExp(const QRegExp &rx, bool ignore = true);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    void ignoreImplicitlyRegExp(const QRegularExpression &rx, bool ignore = true);
#endif
    void ignoreWord(const QString &word, bool ignore = true);
    void ignoreWordImplicitly(const QString &word, bool ignore = true);
    void ignoreWords(const QStringList &words, bool ignore = true);
    void ignoreWordsImplicitly(const QStringList &words, bool ignore = true);
    bool isIgnored(const QString &word, bool *implicitly = 0) const;
    bool isIgnored(bool *implicitly, const QString &word, const QString &surrLeft = QString(),
                   const QString &surrRight = QString()) const;
    void removeDictionary(const QLocale &locale);
    void removeDictionary(const QString &localeName);
    void setUserDictionary(const QString &path);
    bool spell(const QString &word, const QString &surrLeft = QString(), const QString &surrRight = QString()) const;
    QStringList suggest(const QString &word) const;
    QString userDictionaryPath() const;
Q_SIGNALS:
    void changed();
private:
    Q_DISABLE_COPY(BSpellChecker)
};

#endif // BSPELLCHECKER_H
