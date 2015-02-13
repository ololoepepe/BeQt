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

#include "btexttools.h"

#include "bdirtools.h"
#include "bnamespace.h"
#include "btextmatch.h"
#include "btextmatchlist.h"

#include <QByteArray>
#include <QDebug>
#include <QList>
#include <QLocale>
#include <QMap>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QTextCodec>

#if defined(BUILDING_LIBENCA)
#include "../3rdparty/enca/lib/enca.h"
#else
#include <lib/enca.h>
#endif

#include <math.h>

/*============================================================================
================================ BParsingOption ==============================
============================================================================*/

struct BParsingOption
{
    bool assignable;
    QString id;
    QStringList keys;
    bool optional;
    QStringList values;
public:
    explicit BParsingOption();
};

/*============================================================================
================================ BParsingOption ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BParsingOption::BParsingOption()
{
    assignable = false;
    optional = false;
}

/*============================================================================
================================ Global static functions =====================
============================================================================*/

static QList<BParsingOption> createOptions(const QString &options, BTextTools::OptionsParsingError *error)
{
    QList<BParsingOption> list;
    QStringList sl = options.split(QRegExp("\\,\\s*"), QString::SkipEmptyParts);
    if (sl.isEmpty())
        return bRet(error, BTextTools::InvalidParametersError, list);
    foreach (const QString &ss, sl) {
        BParsingOption o;
        if (ss.isEmpty())
            return bRet(error, BTextTools::InvalidParametersError, list);
        QString s = ss;
        if (s.startsWith('[')) {
            if (s.endsWith(']'))
                s = s.mid(1, s.length() - 2);
            else
                return bRet(error, BTextTools::InvalidParametersError, list);
        }
        if (ss != s)
            o.optional = true;
        QStringList idsl = s.split(':');
        if (idsl.size() > 2)
            return bRet(error, BTextTools::InvalidParametersError, list);
        if (idsl.size() == 2) {
            if (idsl.first().isEmpty() || idsl.last().isEmpty())
                return bRet(error, BTextTools::InvalidParametersError, list);
            o.id = idsl.first();
        }
        QStringList vsl = idsl.last().split('=');
        if (vsl.first().isEmpty() || vsl.size() > 2)
            return bRet(error, BTextTools::InvalidParametersError, list);
        if (vsl.size() == 2) {
            o.assignable = true;
            if (!vsl.last().isEmpty())
                o.values = vsl.last().split('|');
            if (o.values.contains(""))
                return bRet(error, BTextTools::InvalidParametersError, list);
        }
        o.keys = vsl.first().split('|');
        if (o.keys.contains(""))
            return bRet(error, BTextTools::InvalidParametersError, list);
        if (o.id.isEmpty())
            o.id = o.keys.first();
        list << o;
    }
    return bRet(error, BTextTools::NoError, list);
}

/*============================================================================
================================ BTextTools ==================================
============================================================================*/

/*!
\namespace BTextTools
\inmodule BeQtCore

\brief The BTextTools namespace contains miscellaneous functions for string processing used throughout the BeQt
library.
*/

/*!
\enum BTextTools::OptionsParsingError

This type is used to specify which error occured during options parsing.

\value NoError
No error occured, everything is OK.

\value InvalidParametersError
The syntax of a parsing rule is invalid.

\value MalformedOptionError
The option syntax of a string parsed is invalid.

\value MissingOptionError
The required option is missing.

\value RepeatingOptionError
There is a duplicate option which must not be duplicated.

\value UnknownOptionError
There is an unknown option.

\value UnknownOptionValueError
The value of an option is unknown.
*/

/*!
\enum BTextTools::RegExpPattern

This type is used to specify the standard regular expression pattern for common cases.

\value EmailPattern
A pattern used to match an e-mail address.
*/

namespace BTextTools
{

/*!
\fn QString BTextTools::appendTrailingSpaces(const QString &s, int lineLength)
Appends spaces to the string \a s, so that it's length becomes equal to \a lineLength.

Returns the new string.

If \a s is already \a lineLength symbols long or longer, no spaces are appended, and the string is returned as is.

\sa removeTrailingSpaces()
*/

QString appendTrailingSpaces(const QString &s, int lineLength)
{
    QString ns = s;
    appendTrailingSpaces(&ns, lineLength);
    return ns;
}

/*!
\fn void BTextTools::appendTrailingSpaces(QString *s, int lineLength)
\overload
Appends spaces to the string \a s, so that it's length becomes equal to \a lineLength.

If \a s is already \a lineLength symbols long or longer, no spaces are appended, and the string is returned as is.

If \a s is a null pointer, nothing is done.

\sa removeTrailingSpaces()
*/

void appendTrailingSpaces(QString *s, int lineLength)
{
    if (!s)
        return;
    int len = s->length();
    if (len < lineLength)
        s->append(QString().fill(' ', lineLength - len));
}

/*!
\fn QTextCodec *BTextTools::guessFileCodec(const QString &fileName, const QLocale locale)
Tries to guess the encoding of the file with name \a fileName.

The \a locale is used as a supposed locale (typically the system locale).

The entire file is read and all it's data is used.

Returns a pointer to the corresponding QTextCodec. If it is not possible to determine the codec, 0 is retuned.
*/

QTextCodec *guessFileCodec(const QString &fileName, const QLocale locale)
{
    return BeQt::codec(guessFileCodecName(fileName, locale));
}

/*!
\fn QTextCodec *BTextTools::guessFileCodec(const QString &fileName, qint64 bytes, const QLocale locale)
\overload
Tries to guess the encoding of the file with name \a fileName.

The \a locale is used as a supposed locale (typically the system locale).

Only the first \a bytes bytes of the file are read and used for guessing. If \a bytes is -1 of less, the entire file is
read.

Returns a pointer to the corresponding QTextCodec. If it is not possible to determine the codec, 0 is retuned.
*/

QTextCodec *guessFileCodec(const QString &fileName, qint64 bytes, const QLocale locale)
{
    return BeQt::codec(guessFileCodecName(fileName, bytes, locale));
}

/*!
\fn QString BTextTools::guessFileCodecName(const QString &fileName, const QLocale locale)
Tries to guess the encoding of the file with name \a fileName.

The \a locale is used as a supposed locale (typically the system locale).

The entire file is read and all it's data is used.

Returns the name of the corresponding QTextCodec. If it is not possible to determine the codec, an empty QString is
retuned.
*/

QString guessFileCodecName(const QString &fileName, const QLocale locale)
{
    return guessFileCodecName(fileName, -1, locale);
}

/*!
\fn QString BTextTools::guessFileCodecName(const QString &fileName, qint64 bytes, const QLocale locale)
\overload
Tries to guess the encoding of the file with name \a fileName.

The \a locale is used as a supposed locale (typically the system locale).

Only the first \a bytes bytes of the file are read and used for guessing. If \a bytes is -1 of less, the entire file is
read.

Returns the name of the corresponding QTextCodec. If it is not possible to determine the codec, an empty QString is
retuned.
*/

QString guessFileCodecName(const QString &fileName, qint64 bytes, const QLocale locale)
{
    if (fileName.isEmpty())
        return "";
    bool ok = false;
    QByteArray data = BDirTools::readFile(fileName, bytes, &ok);
    return ok ? guessTextCodecName(data, locale) : QString();
}

/*!
\fn QTextCodec *BTextTools::guessTextCodec(const QByteArray &data, const QLocale locale)
Tries to guess the encoding of the text \a data.

The \a locale is used as a supposed locale (typically the system locale).

Returns a pointer to the corresponding QTextCodec. If it is not possible to determine the codec, 0 is retuned.
*/

QTextCodec *guessTextCodec(const QByteArray &data, const QLocale locale)
{
    return BeQt::codec(guessTextCodecName(data, locale));
}

/*!
\fn QString BTextTools::guessTextCodecName(const QByteArray &data, const QLocale locale)
Tries to guess the encoding of the text \a data.

The \a locale is used as a supposed locale (typically the system locale).

Returns the name of the corresponding QTextCodec. If it is not possible to determine the codec, an empty QString is
retuned.
*/

QString guessTextCodecName(const QByteArray &data, const QLocale locale)
{
    if (data.isEmpty())
        return "";
    EncaAnalyser an = enca_analyser_alloc(locale.name().left(2).toLocal8Bit().constData());
    if (!an)
        return "";
    enca_set_threshold(an, 1.38);
    enca_set_multibyte(an, 1);
    enca_set_ambiguity(an, 1);
    enca_set_garbage_test(an, 1);
    EncaEncoding result = enca_analyse_const(an, (const unsigned char *) data.constData(), data.size());
    enca_analyser_free(an);
    if (!enca_charset_is_known(result.charset))
        return "";
    return QString(enca_charset_name(result.charset, ENCA_NAME_STYLE_MIME));
}

/*!
\fn int BTextTools::indexOf(const QString &where, const QString &what, int from, Qt::CaseSensitivity cs,
                            bool wholeWords)
Returns the index position of the first occurrence of the string \a what in the string \a where, searching forward from
index position \a from. Returns -1 if \a what is not found.

If \a cs is Qt::CaseSensitive, the search is case sensitive; otherwise the search is case insensitive.

If \a from is -1, the search starts at the last character; if it is -2, at the next to last character and so on.

See QString::indexOf() documentation for details.

Unlike QString::indexOf(), this function may also search for whole words only if the \a wholeWords parameter is true.

Example:

\snippet src/corelib/btexttools.cpp 0

\sa lastIndexOf()
*/

int indexOf(const QString &where, const QString &what, int from, Qt::CaseSensitivity cs, bool wholeWords)
{
    if (where.isEmpty() || what.isEmpty() || where.length() < what.length() || from < 0)
        return -1;
    int ind = where.indexOf(what, from, cs);
    while (ind >= 0) {
        if (!wholeWords)
            return ind;
        if (ind > 0 && where.at(ind - 1).isLetterOrNumber() && !where.at(ind - 1).isNumber()) {
            ind = where.indexOf(what, ind + what.length(), cs);
            continue;
        }
        if (ind + what.length() < where.length() - 1 && where.at(ind + what.length()).isLetterOrNumber()
                && !where.at(ind + what.length()).isNumber()) {
            ind = where.indexOf(what, ind + what.length(), cs);
            continue;
        }
        return ind;
    }
    return -1;
}

/*!
\fn bool BTextTools::intersects(const QStringList &list1, const QStringList &list2, Qt::CaseSensitivity cs)
Returns true if at least one element in \a list1 is equal to at least one element if \a list2. Otherwise returns false.

If \a cs is Qt::CaseSensitive, the copmarison is case sensitive; otherwise the comparison is case insensitive.
*/

bool intersects(const QStringList &list1, const QStringList &list2, Qt::CaseSensitivity cs)
{
    foreach (const QString &s1, list1) {
        if (list2.contains(s1, cs))
            return true;
    }
    return false;
}

/*!
\fn double BTextTools::inverseDocumentFrequency(const QString &term, const QStringList &documents, QList<int> *tfs)
Returns the frequency of occurance  of the string \a term in \a documents.

If \a tfs is a non-null pointer, it's populated with the term frequancy values for each document.

For details, see \l https://en.wikipedia.org/wiki/Tf–idf

\sa termFrequency(), tfidf(), tfidfSortedIndexes()
*/

double inverseDocumentFrequency(const QString &term, const QStringList &documents, QList<int> *tfs)
{
    if (term.isEmpty() || documents.isEmpty())
        return 0.0;
    int count = 0;
    if (tfs) {
        tfs->clear();
        foreach (const QString &document, documents) {
            int tf = termFrequency(term, document);
            if (tf)
                ++count;
            tfs->append(tf);
        }
    } else {
        foreach (const QString &document, documents) {
            if (document.toLower().contains(QRegExp("\\b" + term.toLower() + "\\b")))
                ++count;
        }
    }
    if (!count)
        return 0.0;
    if (documents.size() == count)
        return ++count;
    return log(double(documents.size()) / double(count));
}

/*!
\fn int BTextTools::lastIndexOf(const QString &where, const QString &what, int from, Qt::CaseSensitivity cs,
                                bool wholeWords)
Returns the index position of the last occurrence of the string \a what in the string \a where, searching backward from
index position \a from. If from is -1, the search starts at the last character; if from is -2, at the next to last
character and so on. Returns -1 if \a what is not found.

If \a cs is Qt::CaseSensitive, the search is case sensitive; otherwise the search is case insensitive.

See QString::lastIndexOf() documentation for details.

Unlike QString::lastIndexOf(), this function may also search for whole words only if the \a wholeWords parameter is
true.

Example:

\snippet src/corelib/btexttools.cpp 1

\sa indexOf()
*/

int lastIndexOf(const QString &where, const QString &what, int from, Qt::CaseSensitivity cs, bool wholeWords)
{
    if (where.isEmpty() || what.isEmpty() || where.length() < what.length() || from > 0)
        return -1;
    int ind = where.lastIndexOf(what, from, cs);
    while (ind >= 0) {
        if (!wholeWords)
            return ind;
        if (ind > 0 && where.at(ind - 1).isLetterOrNumber() && !where.at(ind - 1).isNumber()) {
            ind = where.lastIndexOf(what, ind - where.length() - 1, cs);
            continue;
        }
        if (ind + what.length() < where.length() - 1 && where.at(ind + what.length()).isLetterOrNumber()
                && !where.at(ind + what.length()).isNumber()) {
            ind = where.lastIndexOf(what, ind - where.length() - 1, cs);
            continue;
        }
        return ind;
    }
    return -1;
}

/*!
\fn BTextMatchList BTextTools::match(const QString &text, const QRegExp &what, const QRegExp &prefixedBy,
                                     const QRegExp &postfixedBy)
Matches the regular expression \a what against the string \a text.

If \a prefixedBy is a valid QRegExp, only the substrings prefixed by that regular expression are matched.

If \a postfixedBy is a valid QRegExp, only the substrings prefixed by that regular expression are matched.

Returns the list of BTextMatch objects.

Example

\snippet src/corelib/btexttools.cpp 2

\sa BTextMatch, BTextMatchList
*/

BTextMatchList match(const QString &text, const QRegExp &what, const QRegExp &prefixedBy, const QRegExp &postfixedBy)
{
    BTextMatchList list;
    if (text.isEmpty() || !what.isValid())
        return list;
    QStringList sl = text.split('\n');
    int coveredLength = 0;
    foreach (int i, bRangeD(0, sl.size() - 1)) {
        const QString &line = sl.at(i);
        int pos = what.indexIn(line);
        while (pos >= 0) {
            bool b = true;
            int len = what.matchedLength();
            if (!prefixedBy.isEmpty() && prefixedBy.isValid()) {
                int prind = prefixedBy.indexIn(line.mid(0, pos));
                if (prind < 0 || prind + prefixedBy.matchedLength() != pos) {
                    prind = prefixedBy.indexIn(line.mid(pos, len));
                    if (prind == 0) {
                        pos += prefixedBy.matchedLength();
                        len -= prefixedBy.matchedLength();
                    } else {
                        b = false;
                    }
                }
            }
            if (!postfixedBy.isEmpty() && postfixedBy.isValid()) {
                int poind = postfixedBy.indexIn(line.mid(pos + len));
                if (poind != 0) {
                    QString s = line.mid(pos, len);
                    poind = postfixedBy.indexIn(s);
                    if (poind >= 0 && poind + postfixedBy.matchedLength() == s.length())
                        len -= postfixedBy.matchedLength();
                    else
                        b = false;
                }
            }
            if (b && line.mid(pos, len).contains(what))
                list << BTextMatch(&text, coveredLength + pos, len);
            pos = what.indexIn(line, pos + (len ? len : 1));
        }
        coveredLength += line.length() + 1;
    }
    return list;
}

/*!
\fn QString BTextTools::mergeArguments(const QStringList &list)
Merges the arguments \a list into a single string. The arguments are separated by spaces.

The arguments containing spaces are wrapped into double quotes.

Returns the resulting string.

\sa splitCommand()
*/

QString mergeArguments(const QStringList &list)
{
    QString args;
    foreach (const QString &a, list)
        args += (a.contains(' ') ? BTextTools::wrapped(a, "\"") : a) + " ";
    if (!args.isEmpty())
        args.remove(args.length() - 1, 1);
    return args;
}

/*!
\fn QString BTextTools::mergeArguments(const QString &command, const QStringList &arguments)
\overload
Merges the \a command and \a arguments into a single string. The arguments are separated by spaces.

The arguments containing spaces are wrapped into double quotes.

Returns the resulting string.

\sa splitCommand()
*/

QString mergeArguments(const QString &command, const QStringList &arguments)
{
    return mergeArguments(QStringList() << command << arguments);
}

/*!
\fn OptionsParsingError BTextTools::parseOptions(const QStringList &arguments, const QString &options,
                                                 QMap<QString, QString> &result, bool allowOverride)
Parses the options list \a arguments. The available options are listed in the \a options string. The results are
inserted into \a result map.

The options listed in the \a options parameter must be separated by commas. A single option must consist of an
identifier, a colon, and list of keys separated by vertical bars. The list of keys may be followed by an equality sign
and (optionally) by a list of possible values separated by vertical bars. Options, which are not required, must be
enclosed in brackets.

Example:

\snippet src/corelib/btexttools.cpp 3

If \a allowOverride is true and an option is mentioned several times, it's previous value is overvritten with the new
one. If \a allowOverride is false and an option is mentioned several times, a RepeatingOptionError is returned.

If there are no errors, NoError is returned. See OptionsParsingError for details.
*/

OptionsParsingError parseOptions(const QStringList &arguments, const QString &options, QMap<QString, QString> &result,
                                 bool allowOverride)
{
    QString s;
    return parseOptions(arguments, options, result, s, allowOverride);
}

/*!
\fn OptionsParsingError BTextTools::parseOptions(const QStringList &arguments, const QString &options,
                                                 QMap<QString, QString> &result, QString &errorData,
                                                 bool allowOverride)
\overload
Parses the options list \a arguments. The available options are listed in the \a options string. The results are
inserted into \a result map.

If \a allowOverride is true and an option is mentioned several times, it's previous value is overvritten with the new
one. If \a allowOverride is false and an option is mentioned several times, a RepeatingOptionError is returned.

If an error occurs, \a errorData is set to error-specific information.

If there are no errors, NoError is returned. See OptionsParsingError for details.
*/

OptionsParsingError parseOptions(const QStringList &arguments, const QString &options, QMap<QString, QString> &result,
                                 QString &errorData, bool allowOverride)
{
    result.clear();
    errorData.clear();
    if (arguments.isEmpty() || options.isEmpty())
        return NoError;
    OptionsParsingError error = NoError;
    QList<BParsingOption> list = createOptions(options, &error);
    if (NoError != error)
        return error;
    foreach (const QString &a, arguments) {
        bool found = false;
        foreach (const BParsingOption &o, list) {
            foreach (const QString &key, o.keys) {
                if (a.startsWith(key)) {
                    if (result.contains(o.id) && !allowOverride) {
                        errorData = o.id;
                        return RepeatingOptionError;
                    }
                    if (o.assignable) {
                        QStringList sl = a.split('=');
                        if (sl.size() != 2) {
                            errorData = a;
                            return MalformedOptionError;
                        }
                        if (!o.values.isEmpty() && !o.values.contains(sl.last())) {
                            errorData = sl.last();
                            return UnknownOptionValueError;
                        }
                        result.insert(o.id, sl.last());
                    } else {
                        result.insert(o.id, "");
                    }
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
        if (!found) {
            errorData = a;
            return UnknownOptionError;
        }
    }
    foreach (const BParsingOption &o, list) {
        if (!o.optional && !result.contains(o.id)) {
            errorData = o.id;
            return MissingOptionError;
        }
    }
    return NoError;
}

/*!
\fn QStringList BTextTools::removeAll(const QStringList &list, const QString &what, Qt::CaseSensitivity cs, int *count)
Removes all strings which are equal to \a what from the QStringList \a list. The list itself is not modified.

If \a cs is Qt::CaseSensitive, the comparison is case sensitive; otherwise the comparison is case insensitive.

If \a count is a non-null pointer, it's value is set to the nubmer of removed elements.

Returns the new string list.
*/

QStringList removeAll(const QStringList &list, const QString &what, Qt::CaseSensitivity cs, int *count)
{
    QStringList nlist = list;
    int c = removeAll(&nlist, what, cs);
    return bRet(count, c, nlist);
}

/*!
\fn int BTextTools::removeAll(QStringList *list, const QString &what, Qt::CaseSensitivity cs)
\overload
Removes all strings which are equal to \a what from the QStringList \a list.

If \a cs is Qt::CaseSensitive, the comparison is case sensitive; otherwise the comparison is case insensitive.

Returns the number of removed elements.
*/

int removeAll(QStringList *list, const QString &what, Qt::CaseSensitivity cs)
{
    if (!list)
        return 0;
    int count = 0;
    foreach (int i, bRangeR(list->size() - 1, 0)) {
        if (!what.compare(list->at(i), cs)) {
            list->removeAt(i);
            ++count;
        }
    }
    return count;
}

/*!
\fn QStringList BTextTools::removeDuplicates(const QStringList &list, Qt::CaseSensitivity cs, int *count)
Removes all duplicate strings from the QStringList \a list. The list itself is not modified.

If \a cs is Qt::CaseSensitive, the comparison is case sensitive; otherwise the comparison is case insensitive.

If \a count is a non-null pointer, it's value is set to the nubmer of removed elements.

Returns the new string list.
*/

QStringList removeDuplicates(const QStringList &list, Qt::CaseSensitivity cs, int *count)
{
    QStringList nlist = list;
    int c = removeDuplicates(&nlist, cs);
    return bRet(count, c, nlist);
}

/*!
\fn int BTextTools::removeDuplicates(QStringList *list, Qt::CaseSensitivity cs)
\overload
Removes all duplicate strings from the QStringList \a list.

If \a cs is Qt::CaseSensitive, the comparison is case sensitive; otherwise the comparison is case insensitive.

Returns the number of removed elements.
*/

int removeDuplicates(QStringList *list, Qt::CaseSensitivity cs)
{
    if (!list)
        return 0;
    int count = 0;
    foreach (int i, bRangeR(list->size() - 1, 1)) {
        foreach (int j, bRangeR(i - 1, 0)) {
            if (!list->at(i).compare(list->at(j), cs)) {
                list->removeAt(i);
                ++count;
                continue;
            }
        }
    }
    return count;
}

/*!
\fn QString BTextTools::removeTrailingSpaces(const QString &s)
Removes all trailing spaces (the ones at the end of the string) from the string \a s. The string itself is not
modified.

Returns the new string.

\sa appendTrailingSpaces()
*/

QString removeTrailingSpaces(const QString &s)
{
    QString ns = s;
    removeTrailingSpaces(&ns);
    return ns;
}

/*!
\fn void BTextTools::removeTrailingSpaces(QString *s)
\overload
Removes all trailing spaces (the ones at the end of the string) from the string \a s.

If \a s is a null pointer, nothing is done.

\sa appendTrailingSpaces()
*/

void removeTrailingSpaces(QString *s)
{
    if (!s || s->isEmpty())
        return;
    QStringList sl = s->split('\n');
    foreach (int i, bRangeD(0, sl.size() - 1))
        sl[i].remove(QRegExp("\\s+$"));
    *s = sl.join("\n");
}

/*!
\fn void BTextTools::removeUnsuppottedSymbols(QString &s)
Removes unsupported symbols form the string \a s.

The unsupported are some special symbols, sucha as the invisible space of some control symbols.

The CRLF is also replaced by LF.

See \l https://en.wikipedia.org/wiki/ASCII for details.

\sa withoutUnsuppottedSymbols()
*/

void removeUnsuppottedSymbols(QString &s)
{
    typedef QList<QChar> CharList;
    init_once(CharList, list, CharList()) {
        list << QChar(1) << QChar(2) << QChar(3) << QChar(4) << QChar(5) << QChar(6) << QChar(7) << QChar(8);
        list << QChar(11) << QChar(12) << QChar(14) << QChar(15) << QChar(16) << QChar(17) << QChar(18) << QChar(19);
        list << QChar(20) << QChar(21) << QChar(22) << QChar(23)  << QChar(24) << QChar(25) << QChar(26) << QChar(27);
        list << QChar(28) << QChar(29) << QChar(30) << QChar(31);
    }
    if (s.isEmpty())
        return;
    foreach (const QChar &c, list)
        s.remove(c);
    s.replace("\r\n", "\n");
    s.replace('\r', '\n');
}

/*!
\fn QString &BTextTools::replace(QString &where, const QString &oldText, const QString &newText,
                                 Qt::CaseSensitivity cs, bool wholeWords)
Replaces all occurance of string \a oldText in the string \a where with the string \a newText.

If \a cs is Qt::CaseSensitive, the comparison is case sensitive; otherwise the comparison is case insensitive.

This function may also search for whole words only if the \a wholeWords parameter is true.

Returns a reference to the string \a where.
*/

QString &replace(QString &where, const QString &oldText, const QString &newText, Qt::CaseSensitivity cs,
                 bool wholeWords)
{
    if (where.isEmpty() || oldText.isEmpty() || where.length() < oldText.length())
        return where;
    int ind = indexOf(where, oldText, 0, cs, wholeWords);
    while (ind >= 0) {
        where.replace(ind, oldText.length(), newText);
        ind = indexOf(where, oldText, ind + newText.length(), cs, wholeWords);
    }
    return where;
}

/*!
\fn QString BTextTools::replaceTabs(const QString &s, BeQt::TabWidth tw)
Replaces all tabulation symbols in the string \a s with spaces. \a tw is used to specify the tabulation equivalent in
spaces. The string itself is not modified.

Returns the new string.
*/

QString replaceTabs(const QString &s, BeQt::TabWidth tw)
{
    QString ns = s;
    replaceTabs(&ns, tw);
    return ns;
}

/*!
\fn void BTextTools::replaceTabs(QString *s, BeQt::TabWidth tw)
Replaces all tabulation symbols in the string \a s with spaces. \a tw is used to specify the tabulation equivalent in
spaces.
*/

void replaceTabs(QString *s, BeQt::TabWidth tw)
{
    if (!s)
        return;
    int i = 0;
    while (i < s->length()) {
        if (s->at(i) == '\t') {
            int spcount = tw - (i < tw ? i : i % tw);
            s->replace(i, 1, QString().fill(' ', spcount ? spcount : tw));
            i += spcount;
        } else {
            ++i;
        }
    }
}

/*!
\fn QStringList BTextTools::sortComprising(const QStringList &list, Qt::CaseSensitivity cs)
Sorts the \a list in such a way that the strings which contains other string(s) as their part are placed before the
strings that they contain. The order of substrings is undefined.

If \a cs is Qt::CaseSensitive, the comparison is case sensitive; otherwise the comparison is case insensitive.

Returns the new string list.

Example:

\snippet src/corelib/btexttools.cpp 4
*/

QStringList sortComprising(const QStringList &list, Qt::CaseSensitivity cs)
{
    QStringList nlist = list;
    sortComprising(&nlist, cs);
    return nlist;
}

/*!
\fn void BTextTools::sortComprising(QStringList *list, Qt::CaseSensitivity cs)
Sorts the \a list in such a way that the strings which contains other string(s) as their part are placed before the
strings that they contain. The order of substrings is undefined.

If \a cs is Qt::CaseSensitive, the comparison is case sensitive; otherwise the comparison is case insensitive.

If \a list is a null pointer, nothing is done.
*/

void sortComprising(QStringList *list, Qt::CaseSensitivity cs)
{
    if (!list || list->size() < 2)
        return;
    QStringList nlist;
    foreach (int i, bRangeR(list->size() - 1, 0)) {
        foreach (int j, bRangeR(list->size() - 1, 0)) {
            if (i == j || !list->at(j).contains(list->at(i), cs))
                continue;
            QString s = list->takeAt(i);
            bool b = false;
            foreach (int k, bRangeD(0, nlist.size() - 1)) {
                if (!s.contains(nlist.at(k), cs))
                    continue;
                if (!s.compare(nlist.at(k), cs))
                    nlist.insert(k, s);
                else
                    nlist.prepend(s);
                b = true;
                break;
            }
            if (!b)
                nlist.append(s);
            break;
        }
    }
    *list += nlist;
}

/*!
\fn QStringList BTextTools::splitCommand(const QString &cmd, bool *ok)
Splist the command \a cmd into a list of arguments.

The arguments wrapped into double quotes are unwrapped.

If \a ok is a non-null pointer, it's value is set to false if an error occurs, or to true if there is no error.

Returns the resulting string list.

\sa mergeArguments()
*/

QStringList splitCommand(const QString &cmd, bool *ok)
{
    bool b = false;
    QStringList args;
    QString command = splitCommand(cmd, args, &b);
    if (!b)
        return bRet(ok, false, QStringList());
    args.prepend(command);
    return bRet(ok, true, args);
}

/*!
\fn QStringList BTextTools::splitCommand(const QString &cmd, QString &command, bool *ok)
\overload
Splist the command \a cmd into a command name \a command and a list of arguments.

The arguments wrapped into double quotes are unwrapped.

If \a ok is a non-null pointer, it's value is set to false if an error occurs, or to true if there is no error.

Returns the resulting string list of arguments.

\sa mergeArguments()
*/

QStringList splitCommand(const QString &cmd, QString &command, bool *ok)
{
    bool b = false;
    QStringList args;
    QString c = splitCommand(cmd, args, &b);
    if (!b) {
        command.clear();
        return bRet(ok, false, QStringList());
    }
    command = c;
    return bRet(ok, true, args);
}

/*!
\fn QString BTextTools::splitCommand(const QString &cmd, QStringList &arguments, bool *ok)
\overload
Splist the command \a cmd into a command name and a list of arguments \a arguments.

The arguments wrapped into double quotes are unwrapped.

If \a ok is a non-null pointer, it's value is set to false if an error occurs, or to true if there is no error.

Returns the resulting command name.

\sa mergeArguments()
*/

QString splitCommand(const QString &cmd, QStringList &arguments, bool *ok)
{
    QStringList args;
    QString arg;
    int quot = 0;
    for (int i = 0; i < cmd.length(); ++i) {
        const QChar &c = cmd.at(i);
        if (c.isSpace()) {
            if (quot) {
                arg.append(c);
            } else if (!arg.isEmpty()) {
                args << arg;
                arg.clear();
            }
        } else {
            if (c == '\"' && (i < 1 || cmd.at(i - 1) != '\\')) {
                switch (quot) {
                case 1:
                    quot = 0;
                    break;
                case -1:
                    arg.append(c);
                    break;
                case 0:
                default:
                    quot = 1;
                    break;
                }
            } else if (c == '\'' && (i < 1 || cmd.at(i - 1) != '\\')) {
                switch (quot) {
                case 1:
                    arg.append(c);
                    break;
                case -1:
                    quot = 0;
                    break;
                case 0:
                default:
                    quot = -1;
                    break;
                }
            } else {
                if ((c == '\"' || c == '\'') && (i > 0 || cmd.at(i - 1) == '\\') && !arg.isEmpty())
                    arg.remove(arg.length() - 1, 1);
                arg.append(c);
            }
        }
    }
    if (!arg.isEmpty()) {
        if (quot) {
            arguments.clear();
            return bRet(ok, false, QString());
        }
        args << arg;
    }
    QString command;
    if (!args.isEmpty())
        command = args.takeFirst();
    arguments = args;
    return bRet(ok, true, command);
}

/*!
\fn QRegExp BTextTools::standardRegExp(RegExpPattern type)
Returns a QRegExp object for a standard regular expression type \a type.

\sa standardRegExpPattern()
*/

QRegExp standardRegExp(RegExpPattern type)
{
    switch (type) {
    case EmailPattern:
        return QRegExp(standardRegExpPattern(type));
    default:
        return QRegExp();
    }
}

/*!
\fn QString BTextTools::standardRegExpPattern(RegExpPattern type)
Returns a QRegExp object for a standard regular expression type \a type.

\sa standardRegExp()
*/

QString standardRegExpPattern(RegExpPattern type)
{
    static const QString email = "^[-a-z0-9!#$%&'*+/=?^_`{|}~]+(?:\\.[-a-z0-9!#$%&'*+/=?^_`{|}~]+)*@"
        "(?:[a-z0-9]([-a-z0-9]{0,61}[a-z0-9])?\\.)*"
        "(?:aero|arpa|asia|biz|cat|com|coop|edu|gov|info|int|jobs|mil|mobi|museum|name|net|org|pro|tel|travel|"
        "[a-z][a-z])$";
    switch (type) {
    case EmailPattern:
        return email;
    default:
        return "";
    }
}

/*!
\fn int BTextTools::termFrequency(const QString &term, const QString &document)
Returns the frequency of occurance of the string \a term in \a document.

For details, see \l https://en.wikipedia.org/wiki/Tf–idf

\sa inverseDocumentFrequency(), tfidf(), tfidfSortedIndexes()
*/

int termFrequency(const QString &term, const QString &document)
{
    if (term.isEmpty() || document.isEmpty())
        return 0;
    return document.toLower().count(QRegExp("\\b" + term.toLower() + "\\b"));
}

/*!
\fn QList<double> BTextTools::tfidf(const QString &term, const QStringList &documents)
Returns the Tf-idf of the string \a term in \a documents.

For details, see \l https://en.wikipedia.org/wiki/Tf–idf

\sa inverseDocumentFrequency(), termFrequency(), tfidfSortedIndexes()
*/

QList<double> tfidf(const QString &term, const QStringList &documents)
{
    if (term.isEmpty() || documents.isEmpty())
        return QList<double>();
    QList<double> list;
    QList<int> tfs;
    double idf = inverseDocumentFrequency(term, documents, &tfs);
    foreach (int i, bRangeD(0, documents.size() - 1))
        list << double(tfs.at(i)) * idf;
    return list;
}

/*!
\fn QList<double> BTextTools::tfidf(const QStringList &terms, const QStringList &documents)
\overload
Returns the Tf-idf of the string list \a terms in \a documents.

For details, see \l https://en.wikipedia.org/wiki/Tf–idf

\sa inverseDocumentFrequency(), termFrequency(), tfidfSortedIndexes()
*/

QList<double> tfidf(const QStringList &terms, const QStringList &documents)
{
    if (terms.isEmpty() || documents.isEmpty())
        return QList<double>();
    QList<double> idfs;
    QList<double> list;
    QList< QList<int> > tfs;
    foreach (const QString &term, terms) {
        tfs << QList<int>();
        idfs << inverseDocumentFrequency(term, documents, &tfs.last());
    }
    foreach (int i, bRangeD(0, documents.size() - 1)) {
        double d = 0.0;
        foreach (int j, bRangeD(0, idfs.size() - 1))
            d += double(tfs.at(j).at(i)) * idfs.at(j);
        list << d;
    }
    return list;
}

/*!
\fn QList<int> BTextTools::tfidfSortedIndexes(const QString &term, const QStringList &documents)
Returns the Tf-idf indexes of the string \a term in \a documents.

For details, see \l https://en.wikipedia.org/wiki/Tf–idf

\sa inverseDocumentFrequency(), termFrequency(), tfidf()
*/

QList<int> tfidfSortedIndexes(const QString &term, const QStringList &documents)
{
    QList<double> list = tfidf(term, documents);
    QMap<double, int> map;
    foreach(int i, bRangeD(0, list.size() - 1))
        map.insertMulti(list.at(i), i);
    QList<int> ret;
    list = map.uniqueKeys();
    foreach (int i, bRangeR(list.size() - 1, 0)) {
        double d = list.at(i);
        if (qFuzzyIsNull(d))
            continue;
        ret << map.values(d);
    }
    return ret;
}

/*!
\fn QList<int> BTextTools::tfidfSortedIndexes(const QStringList &terms, const QStringList &documents)
Returns the Tf-idf indexes of the string list \a terms in \a documents.

For details, see \l https://en.wikipedia.org/wiki/Tf–idf

\sa inverseDocumentFrequency(), termFrequency(), tfidf()
*/

QList<int> tfidfSortedIndexes(const QStringList &terms, const QStringList &documents)
{
    QList<double> list = tfidf(terms, documents);
    QMap<double, int> map;
    foreach(int i, bRangeD(0, list.size() - 1))
        map.insertMulti(list.at(i), i);
    QList<int> ret;
    list = map.uniqueKeys();
    foreach (int i, bRangeR(list.size() - 1, 0)) {
        double d = list.at(i);
        if (qFuzzyIsNull(d))
            continue;
        ret << map.values(d);
    }
    return ret;
}

/*!
\fn QString BTextTools::toHtml(const QString &text, bool replaceSpaces)
Replaces special symbols in \a text into their HTML equivalents. The string itself is not modified.

If \a replaceSpaces is true, the spaces are also replaced.

Returns the resulting string.
*/

QString toHtml(const QString &text, bool replaceSpaces)
{
    typedef QMap<QChar, QString> KeywordMap;
    init_once(KeywordMap, keywords, KeywordMap()) {
        keywords.insert('&', "&amp;");
        keywords.insert('<', "&lt;");
        keywords.insert('>', "&gt;");
        keywords.insert(' ', "&nbsp;");
        keywords.insert('\"', "&quot;");
        keywords.insert('\n', "<br />");
    }
    if (text.isEmpty())
        return text;
    QString html = text;
    html.remove('\r');
    foreach (int i, bRangeR(html.length() - 1, 0)) {
        const QChar &c = html.at(i);
        if (!replaceSpaces && ' ' == c)
            continue;
        if (keywords.contains(c))
            html.replace(i, 1, keywords.value(c));
    }
    return html;
}

/*!
\fn void BTextTools::unwrap(QString &text, const QString &leftWrappingText, const QString &rightWrappingText)
Removes the \a leftWrappingText from the left and \a rightWrappingText from the right of the string \a text.

\sa wrap(), unwrapped()
*/

void unwrap(QString &text, const QString &leftWrappingText, const QString &rightWrappingText)
{
    if (text.isEmpty() || leftWrappingText.isEmpty() || rightWrappingText.isEmpty())
        return;
    if (text.startsWith(leftWrappingText))
        text.remove(0, leftWrappingText.length());
    if (text.endsWith(rightWrappingText))
        text.remove(text.length() - rightWrappingText.length(), rightWrappingText.length());
}

/*!
\fn void BTextTools::unwrap(QString &text, const QString &wrappingText)
\overload
Removes the \a wrappingText from the left and from the right of the string \a text.

\sa wrap(), unwrapped()
*/

void unwrap(QString &text, const QString &wrappingText)
{
    unwrap(text, wrappingText, wrappingText);
}

/*!
\fn QString BTextTools::unwrapped(const QString &text, const QString &leftWrappingText,
                                  const QString &rightWrappingText)
Removes the \a leftWrappingText from the left and \a rightWrappingText from the right of the string \a text. The string
itself is not modified.

Returns the resultin string.

\sa wrapped(), unwrap()
*/

QString unwrapped(const QString &text, const QString &leftWrappingText, const QString &rightWrappingText)
{
    QString ntext = text;
    unwrap(ntext, leftWrappingText, rightWrappingText);
    return ntext;
}

/*!
\fn QString BTextTools::unwrapped(const QString &text, const QString &wrappingText)
\overload
Removes the \a wrappingText from the left and from the right of the string \a text. The string itself is not modified.

Returns the resultin string.

\sa wrapped(), unwrap()
*/

QString unwrapped(const QString &text, const QString &wrappingText)
{
    return unwrapped(text, wrappingText, wrappingText);
}

/*!
\fn QString BTextTools::withoutUnsuppottedSymbols(const QString &s)
Removes unsupported symbols form the string \a s. The string itself is not modified.

Returns the resulting string.

The unsupported are some special symbols, sucha as the invisible space of some control symbols.

The CRLF is also replaced by LF.

See \l https://en.wikipedia.org/wiki/ASCII for details.

\sa removeUnsuppottedSymbols()
*/

QString withoutUnsuppottedSymbols(const QString &s)
{
    if (s.isEmpty())
        return s;
    QString ns = s;
    removeUnsuppottedSymbols(ns);
    return ns;
}

/*!
\fn void BTextTools::wrap(QString &text, const QString &leftWrappingText, const QString &rightWrappingText)
Wraps the string \a text with \a leftWrappingText on the left and \a rightWrappingText on the right.

If the string already has \a leftWrappingText on the left, that text is \e not added again.

If the string already has \a rightWrappingText on the right, that text is \e not added again.

\sa unwrap(), wrapped()
*/

void wrap(QString &text, const QString &leftWrappingText, const QString &rightWrappingText)
{
    if (text.isEmpty() || leftWrappingText.isEmpty() || rightWrappingText.isEmpty())
        return;
    if (!text.startsWith(leftWrappingText))
        text.prepend(leftWrappingText);
    if (!text.endsWith(rightWrappingText))
        text.append(rightWrappingText);
}

/*!
\fn void BTextTools::wrap(QString &text, const QString &wrappingText)
\overload
Wraps the string \a text with \a wrappingText on the left and on the right.

If the string already has \a wrappingText on the left, that text is \e not added again.

If the string already has \a wrappingText on the right, that text is \e not added again.

\sa unwrap(), wrapped()
*/

void wrap(QString &text, const QString &wrappingText)
{
    wrap(text, wrappingText, wrappingText);
}

/*!
\fn QString BTextTools::wrapped(const QString &text, const QString &leftWrappingText, const QString &rightWrappingText)
Wraps the string \a text with \a leftWrappingText on the left and \a rightWrappingText on the right. The string itself
is not modified.

If the string already has \a leftWrappingText on the left, that text is \e not added again.

If the string already has \a rightWrappingText on the right, that text is \e not added again.

Returns the resultin string.

\sa unwrapped(), wrap()
*/

QString wrapped(const QString &text, const QString &leftWrappingText, const QString &rightWrappingText)
{
    QString ntext = text;
    wrap(ntext, leftWrappingText, rightWrappingText);
    return ntext;
}

/*!
\fn QString BTextTools::wrapped(const QString &text, const QString &wrappingText)
\overload
Wraps the string \a text with \a wrappingText on the left and on the right. The string itself is not modified.

If the string already has \a wrappingText on the left, that text is \e not added again.

If the string already has \a wrappingText on the right, that text is \e not added again.

Returns the resultin string.

\sa unwrapped(), wrap()
*/

QString wrapped(const QString &text, const QString &wrappingText)
{
    return wrapped(text, wrappingText, wrappingText);
}

}
