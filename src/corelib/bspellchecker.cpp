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

#include "bspellchecker.h"
#include "bspellchecker_p.h"

#include "bbaseobject.h"
#include "bbaseobject_p.h"
#include "bdirtools.h"
#include "bnamespace.h"
#include "bspellcheckerdictionary.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QList>
#include <QLocale>
#include <QMap>
#include <QObject>
#include <QRegExp>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#endif
#include <QString>
#include <QStringList>
#include <QtConcurrentRun>
#include <QTextStream>
#include <QtGlobal>
#include <QTimer>

/*============================================================================
================================ BSpellCheckerPrivate ========================
============================================================================*/

/*============================== Public constructors =======================*/

BSpellCheckerPrivate::BSpellCheckerPrivate(BSpellChecker *q) :
    BBaseObjectPrivate(q)
{
    //
}

BSpellCheckerPrivate::~BSpellCheckerPrivate()
{
    if (userDictPath.isEmpty())
        return;
    QStringList words = ignored.keys();
    words.removeAll("");
    flush(userDictPath, words);
}

/*============================== Static public methods =====================*/

void BSpellCheckerPrivate::flush(const QString &fileName, const QStringList &words)
{
    BDirTools::writeTextFile(fileName, words.join("\n"), "UTF-8");
}

/*============================== Public methods ============================*/

void BSpellCheckerPrivate::init()
{
    considerLeft = -1;
    considerRight = -1;
}

/*============================== Public slots ==============================*/

void BSpellCheckerPrivate::flush()
{
    if (userDictPath.isEmpty())
        return;
    QStringList words = ignored.keys();
    words.removeAll("");
    QtConcurrent::run(&BSpellCheckerPrivate::flush, userDictPath, words);
    QTimer::singleShot(BeQt::Minute, this, SLOT(flush()));
}

/*============================================================================
================================ BSpellChecker ===============================
============================================================================*/

/*!
\class BSpellChecker
\inmodule BeQtCore
\brief The BSpellChecker class is used to check spelling of words.

The BSpellChecker contains one or more BSpellCheckerDictionary objects. It provides means of checking a word spelling,
searching for that word in each dictionary. Some words may also be marked as ignored (i.e. treated as correct, even if
they are not present in any of the dictionaries). These words are stored in a special file (user dictionary).

Example:

\snippet src/corelib/bspellchecker.cpp 0

\sa BSpellCheckerDictionary
*/

/*!
\property BSpellChecker::userDictionary

This property gets and sets the BSpellChecker's path to user dictionary.

By default, this property is an empty QString.
*/

/*!
\fn BSpellChecker::changed()

This signal is emitted immediately after a dictionary is added or removed, or the user dictionary path is changed, or
the user dictionary file is modified (cleared or populated with new words), or when a word is added to or removed from
the list of implicitly ignored words.
*/

/*============================== Public constructors =======================*/

/*!
Constructs a spell checker and sets it's parent to \a parent.
*/

BSpellChecker::BSpellChecker(QObject *parent) :
    QObject(parent), BBaseObject(*new BSpellCheckerPrivate(this))
{
    d_func()->init();
}

/*!
Constructs a spell checker. A dictionary is loaded using \a dictionaryPath. User dictionary path is set to
\a userDictionaryPath.
*/

BSpellChecker::BSpellChecker(const QString &dictionaryPath, const QString &userDictionaryPath) :
    QObject(0), BBaseObject(*new BSpellCheckerPrivate(this))
{
    d_func()->init();
    addDictionary(dictionaryPath);
    setUserDictionary(userDictionaryPath);
}

/*!
Constructs a spell checker and sets it's parent to \a parent. A dictionary is loaded using \a dictionaryPath. User
dictionary path is set to \a userDictionaryPath.
*/

BSpellChecker::BSpellChecker(QObject *parent, const QString &dictionaryPath, const QString &userDictionaryPath) :
    QObject(parent), BBaseObject(*new BSpellCheckerPrivate(this))
{
    d_func()->init();
    addDictionary(dictionaryPath);
    setUserDictionary(userDictionaryPath);
}

/*!
Constructs a spell checker. Dictionaries are loaded using \a dictionaryPaths. User dictionary path is set to
\a userDictionaryPath.
*/

BSpellChecker::BSpellChecker(const QStringList &dictionaryPaths, const QString &userDictionaryPath) :
    QObject(0), BBaseObject(*new BSpellCheckerPrivate(this))
{
    d_func()->init();
    foreach (const QString &path, dictionaryPaths)
        addDictionary(path);
    setUserDictionary(userDictionaryPath);
}

/*!
Constructs a spell checker and sets it's parent to \a parent. Dictionaries are loaded using \a dictionaryPaths. User
dictionary path is set to \a userDictionaryPath.
*/

BSpellChecker::BSpellChecker(QObject *parent, const QStringList &dictionaryPaths, const QString &userDictionaryPath) :
    QObject(parent), BBaseObject(*new BSpellCheckerPrivate(this))
{
    d_func()->init();
    foreach (const QString &path, dictionaryPaths)
        addDictionary(path);
    setUserDictionary(userDictionaryPath);
}

/*!
Destroys the object, deleting the associated data object.

The underlying dictionaries are also destroyed.
*/

BSpellChecker::~BSpellChecker()
{
    //
}

/*============================== Public methods ============================*/

/*!
Loads a dictionary from the dicectory \a path and adds it to the internal list of dictionaries.

If a dictionary with the same locale is already in the list, the new dictionary is not added.

\sa removeDictionary()
*/

void BSpellChecker::addDictionary(const QString &path)
{
    QString fn = QFileInfo(path).fileName();
    if (path.isEmpty() || d_func()->dicts.contains(fn))
        return;
    BSpellCheckerDictionary *dict = new BSpellCheckerDictionary(path);
    if (!dict->isValid()) {
        delete dict;
        return;
    }
    d_func()->dicts.insert(fn, dict);
    Q_EMIT changed();
}

/*!
\overload
Creates a dictionary with the raw data \a affixData and \a dictionaryData for locale with the name \a locale, and adds
it to the internal list of dictionaries.

If a dictionary with the same locale is already in the list, the new dictionary is not added.

\sa removeDictionary()
*/

void BSpellChecker::addDictionary(const QByteArray &affixData, const QByteArray &dictionaryData, const QString &locale)
{
    if (affixData.isEmpty() || dictionaryData.isEmpty() || locale.isEmpty() || d_func()->dicts.contains(locale))
        return;
    BSpellCheckerDictionary *dict = new BSpellCheckerDictionary(affixData, dictionaryData, locale);
    if (!dict->isValid()) {
        delete dict;
        return;
    }
    d_func()->dicts.insert(locale, dict);
    Q_EMIT changed();
}

/*!
\overload
Creates a dictionary with the raw data \a affixData and \a dictionaryData for locale \a locale, and adds it to the
internal list of dictionaries.

If a dictionary with the same locale is already in the list, the new dictionary is not added.

\sa removeDictionary()
*/

void BSpellChecker::addDictionary(const QByteArray &affixData, const QByteArray &dictionaryData, const QLocale &locale)
{
    addDictionary(affixData, dictionaryData, locale.name());
}

/*!
Clears the list of ignored words. This affects the file specified as a user dictionary.

\sa clearIgnoredImplicitly(), clearIgnoredImplicitlyRegExp()
*/

void BSpellChecker::clearIgnored()
{
    bool b = !d_func()->ignored.isEmpty();
    d_func()->ignored.clear();
    if (b)
        Q_EMIT changed();
}

/*!
Clears the list of implicitly ignored words. This does not affect the file specified as a user dictionary.

\sa clearIgnored(), clearIgnoredImplicitlyRegExp()
*/

void BSpellChecker::clearIgnoredImplicitly()
{
    bool b = !d_func()->ignoredImplicitly.isEmpty();
    d_func()->ignoredImplicitly.clear();
    if (b)
        Q_EMIT changed();
}

/*!
Clears the list of the words ignored implicitly (specified by a regular expression). This does not affect the file
specified as a user dictionary.

\sa clearIgnored(), clearIgnoredImplicitly()
*/

void BSpellChecker::clearIgnoredImplicitlyRegExp()
{
    bool b = !d_func()->ignoredRx4.isEmpty();
    d_func()->ignoredRx4.clear();
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    b = b || d_func()->ignoredRx5.isEmpty();
    d_func()->ignoredRx5.clear();
#endif
    if (b)
        Q_EMIT changed();
}

/*!
Sets the count of characters prepended to the word which spelling is checked when calling spell() to \a charCount.

See spell() for details.

\sa considerRightSurrounding()
*/

void BSpellChecker::considerLeftSurrounding(int charCount)
{
    if (charCount < 0)
        charCount = -1;
    if (charCount == d_func()->considerLeft)
        return;
    d_func()->considerLeft = charCount;
    Q_EMIT changed();
}

/*!
Sets the count of characters appended to the word which spelling is checked when calling spell() to \a charCount.

See spell() for details.

\sa considerLeftSurrounding()
*/

void BSpellChecker::considerRightSurrounding(int charCount)
{
    if (charCount < 0)
        charCount = -1;
    if (charCount == d_func()->considerRight)
        return;
    d_func()->considerRight = charCount;
    Q_EMIT changed();
}

/*!
Returns a list of pointers to all the underlying dictionaries.

\sa dictionary()
*/

QList<BSpellCheckerDictionary *> BSpellChecker::dictionaries() const
{
    return d_func()->dicts.values();
}

/*!
Returns a pointer to the underlying dictionary, which locale is \a locale.

\sa dictionaries()
*/

BSpellCheckerDictionary *BSpellChecker::dictionary(const QLocale &locale) const
{
    return dictionary(locale.name());
}

/*!
\overload
Returns a pointer to the underlying dictionary, which locale name is \a localeName.

\sa dictionaries()
*/

BSpellCheckerDictionary *BSpellChecker::dictionary(const QString &localeName) const
{
    return d_func()->dicts.value(localeName);
}

/*!
Returns a list of paths for all the underlying dictionaries.

\sa dictionaries()
*/

QStringList BSpellChecker::dictionaryPaths() const
{
    QStringList sl;
    foreach (BSpellCheckerDictionary *dict, d_func()->dicts)
        sl << dict->path();
    return sl;
}

/*!
If \a ignore is true, all words that match the QRegExp \a rx will be ignored (considered correct) when calling spell().
If \a ignore is false, the words will not be ignored.

This function does not affect the user dictionary file.

\sa ignoreWord(), ignoreWordImplicitly()
*/

void BSpellChecker::ignoreImplicitlyRegExp(const QRegExp &rx, bool ignore)
{
    if (ignore)
        d_func()->ignoredRx4 << rx;
    else
        d_func()->ignoredRx4.removeAll(rx);
    Q_EMIT changed();
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

/*!
\overload
If \a ignore is true, all words that match the QRegularExpression \a rx will be ignored (considered correct) when
calling spell(). If \a ignore is false, the words will not be ignored.

This function does not affect the user dictionary file.

\note This function is only available for Qt5.

\sa ignoreWord(), ignoreWordImplicitly()
*/

void BSpellChecker::ignoreImplicitlyRegExp(const QRegularExpression &rx, bool ignore)
{
    if (ignore) {
        d_func()->ignoredRx5 << rx;
        QRegularExpression &rxx = d_func()->ignoredRx5.last();
        QString pattern = rxx.pattern();
        if (!pattern.startsWith('^'))
            pattern.prepend('^');
        if (!pattern.endsWith('$'))
            pattern.append('$');
    } else {
        d_func()->ignoredRx5.removeAll(rx);
    }
    Q_EMIT changed();
}

#endif

/*!
If \a ignore is true, the \a word will be ignored (considered correct) when calling spell(). If \a ignore is false, the
word will not be ignored.

The ignored words are stored in the user dictionary file.

\sa ignoreWords(), ignoreWordImplicitly()
*/

void BSpellChecker::ignoreWord(const QString &word, bool ignore)
{
    ignoreWords(QStringList() << word, ignore);
}

/*!
If \a ignore is true, the \a word will be ignored (considered correct) when calling spell(). If \a ignore is false, the
word will not be ignored.

This function does not affect the user dictionary file.

\sa ignoreImplicitlyRegExp(), ignoreWord()
*/

void BSpellChecker::ignoreWordImplicitly(const QString &word, bool ignore)
{
    ignoreWordsImplicitly(QStringList() << word, ignore);
}

/*!
If \a ignore is true, the \a words will be ignored (considered correct) when calling spell(). If \a ignore is false,
the words will not be ignored.

The ignored words are stored in the user dictionary file.

\sa ignoreWord(), ignoreWordsImplicitly()
*/

void BSpellChecker::ignoreWords(const QStringList &words, bool ignore)
{
    foreach (const QString &w, words) {
        if (ignore)
            d_func()->ignored.insert(w, true);
        else
            d_func()->ignored.remove(w);
    }
    if (!words.isEmpty())
        Q_EMIT changed();
}

/*!
If \a ignore is true, the \a words will be ignored (considered correct) when calling spell(). If \a ignore is false,
the words will not be ignored.

This function does not affect the user dictionary file.

\sa ignoreImplicitlyRegExp(), ignoreWord()
*/

void BSpellChecker::ignoreWordsImplicitly(const QStringList &words, bool ignore)
{
    foreach (const QString &w, words) {
        if (ignore)
            d_func()->ignoredImplicitly.insert(w, true);
        else
            d_func()->ignoredImplicitly.remove(w);
    }
    if (!words.isEmpty())
        Q_EMIT changed();
}

/*!
Returns true if the specified \a word is ignored; otherwise returns false.

If \a implicitly is a non-null pointer, it's valuse is set to true if the word is ignored implicitly, and to false if
the word is ignored explicitly (i.e. is stroded in the user dictionary file).
*/

bool BSpellChecker::isIgnored(const QString &word, bool *implicitly) const
{
    return isIgnored(implicitly, word);
}

/*!
\overload
Returns true if the specified \a word is ignored; otherwise returns false.

If \a implicitly is a non-null pointer, it's valuse is set to true if the word is ignored implicitly, and to false if
the word is ignored explicitly (i.e. is stroded in the user dictionary file).

If the considerLeftSurrounding() was called with a positive number argument, the specified number of characters from
the right of the \a surrLeft string will be prepended to the word.

If the considerRightSurrounding() was called with a positive number argument, the specified number of characters from
the left of the \a surrRight string will be appended to the word.
*/

bool BSpellChecker::isIgnored(bool *implicitly, const QString &word, const QString &surrLeft,
                              const QString &surrRight) const
{
    if (word.isEmpty())
        return false;
    QString w = word;
    if (d_func()->considerLeft)
        w.prepend(surrLeft.right(d_func()->considerLeft));
    if (d_func()->considerRight)
        w.append(surrRight.left(d_func()->considerRight));
    foreach (const QRegExp &rx, d_func()->ignoredRx4) {
        if (rx.exactMatch(w))
            return bRet(implicitly, true, true);
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    foreach (const QRegularExpression &rx, d_func()->ignoredRx5) {
        if (rx.match(w).hasMatch())
            return bRet(implicitly, true, true);
    }
#endif
    if (d_func()->ignored.contains(word))
        return bRet(implicitly, false, true);
    else if (d_func()->ignoredImplicitly.contains(word))
        return bRet(implicitly, true, true);
    else
        return false;
}

/*!
Removes the dictionary with the locale \a locale form the internal list of dictionaries.

\sa addDictionary()
*/

void BSpellChecker::removeDictionary(const QLocale &locale)
{
    removeDictionary(locale.name());
}

/*!
\overload
Removes the dictionary with the locale which name is \a localeName form the internal list of dictionaries.

\sa addDictionary()
*/

void BSpellChecker::removeDictionary(const QString &localeName)
{
    if (localeName.isEmpty())
        return;
    delete d_func()->dicts.take(localeName);
    Q_EMIT changed();
}

void BSpellChecker::setUserDictionary(const QString &path)
{
    if (d_func()->userDictPath == path)
        return;
    d_func()->ignored.clear();
    d_func()->userDictPath.clear();
    if (path.isEmpty()) {
        Q_EMIT changed();
        return;
    }
    bool ok = false;
    ignoreWords(BDirTools::readTextFile(path, "UTF-8", &ok).split('\n'));
    if (!ok && !BDirTools::touch(path)) {
        Q_EMIT changed();
        return;
    }
    d_func()->userDictPath = path;
    QTimer::singleShot(d_func()->ignored.size() * 100 + BeQt::Minute, d_func(), SLOT(flush()));
    Q_EMIT changed();
}

/*!
Checks if the \a word is correct (i.e. the dictionary contains it). In case the word is correct, returns true;
otherwise returns false.

The word is searched for in all the underlying dictionaries.

If the considerLeftSurrounding() was called with a positive number argument, the specified number of characters from
the right of the \a surrLeft string will be prepended to the word.

If the considerRightSurrounding() was called with a positive number argument, the specified number of characters from
the left of the \a surrRight string will be appended to the word.

Example:

\snippet src/corelib/bspellchecker.cpp 1

This may be useful when checking spelling of words in QTextEdit and navigating through it's text.

\sa BSpellCheckerDictionary::spell()
*/

bool BSpellChecker::spell(const QString &word, const QString &surrLeft, const QString &surrRight) const
{
    if (word.isEmpty())
        return false;
    QString w = word;
    if (d_func()->considerLeft)
        w.prepend(surrLeft.right(d_func()->considerLeft));
    if (d_func()->considerRight)
        w.append(surrRight.left(d_func()->considerRight));
    foreach (const QRegExp &rx, d_func()->ignoredRx4) {
        if (rx.exactMatch(w))
            return true;
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    foreach (const QRegularExpression &rx, d_func()->ignoredRx5) {
        if (rx.match(w).hasMatch())
            return true;
    }
#endif
    if (d_func()->ignored.contains(word) || d_func()->ignoredImplicitly.contains(word))
        return true;
    foreach (BSpellCheckerDictionary *dict, d_func()->dicts) {
        if (dict->spell(word))
            return true;
    }
    return false;
}

/*!
Checks if all the \a words are correct (i.e. the dictionary contains them all). In case all the words are correct,
returns true. If any word is incorrect, returns false.

The words are searched for in all the underlying dictionaries.

\sa BSpellCheckerDictionary::spell()
*/

bool BSpellChecker::spell(const QStringList &words) const
{
    foreach (const QString &word, words) {
        if (!spell(word))
            return false;
    }
    return true;
}

/*!
Returns a list of the words resembling the given \a word. The word is searched for in all the underlying dictionaries.

This function may be used to suggest a list of possible variants of corrections for a word written incorrectly.

For example, for the incorrectly written word "spon" the following words will be suggested: "soon", "spin", "son",
"spoon", "pons", "span", "spot", "spun", "upon", "sponge".

\sa BSpellCheckerDictionary::suggest()
*/

QStringList BSpellChecker::suggest(const QString &word) const
{
    if (word.isEmpty())
        return QStringList();
    QStringList sl;
    foreach (BSpellCheckerDictionary *dict, d_func()->dicts)
        sl << dict->suggest(word);
    return sl;
}

QString BSpellChecker::userDictionaryPath() const
{
    return d_func()->userDictPath;
}
