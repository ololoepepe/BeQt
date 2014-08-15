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

#ifndef BTEXTTOOLS_H
#define BTEXTTOOLS_H

class BTextMatchList;

class QByteArray;
class QStringList;
class QTextCodec;

#include "bnamespace.h"

#include <QList>
#include <QLocale>
#include <QMap>
#include <QRegExp>
#include <QString>

/*============================================================================
================================ BTextTools ==================================
============================================================================*/

namespace BTextTools
{

enum OptionsParsingError
{
    NoError = 0,
    InvalidParametersError,
    MalformedOptionError,
    MissingOptionError,
    RepeatingOptionError,
    UnknownOptionError,
    UnknownOptionValueError
};

enum RegExpPattern
{
    EmailPattern
};

B_CORE_EXPORT QString appendTrailingSpaces(const QString &s, int lineLength);
B_CORE_EXPORT void appendTrailingSpaces(QString *s, int lineLength);
B_CORE_EXPORT QTextCodec *guessFileCodec(const QString &fileName, const QLocale locale = QLocale::system());
B_CORE_EXPORT QTextCodec *guessFileCodec(const QString &fileName, qint64 bytes,
                                         const QLocale locale = QLocale::system());
B_CORE_EXPORT QString guessFileCodecName(const QString &fileName, const QLocale locale = QLocale::system());
B_CORE_EXPORT QString guessFileCodecName(const QString &fileName, qint64 bytes,
                                         const QLocale locale = QLocale::system());
B_CORE_EXPORT QTextCodec *guessTextCodec(const QByteArray &data, const QLocale locale = QLocale::system());
B_CORE_EXPORT QString guessTextCodecName(const QByteArray &data, const QLocale locale = QLocale::system());
B_CORE_EXPORT int indexOf(const QString &where, const QString &what, int from = 0,
                          Qt::CaseSensitivity cs = Qt::CaseSensitive, bool wholeWords = false);
B_CORE_EXPORT bool intersects(const QStringList &list1, const QStringList &list2,
                              Qt::CaseSensitivity cs = Qt::CaseSensitive);
B_CORE_EXPORT double inverseDocumentFrequency(const QString &term, const QStringList &documents, QList<int> *tfs = 0);
B_CORE_EXPORT int lastIndexOf(const QString &where, const QString &what, int from = -1,
                              Qt::CaseSensitivity cs = Qt::CaseSensitive, bool wholeWords = false);
B_CORE_EXPORT BTextMatchList match(const QString &text, const QRegExp &what, const QRegExp &prefixedBy = QRegExp(),
                                   const QRegExp &postfixedBy = QRegExp());
B_CORE_EXPORT QString mergeArguments(const QStringList &list);
B_CORE_EXPORT QString mergeArguments(const QString &command, const QStringList &arguments);
B_CORE_EXPORT OptionsParsingError parseOptions(const QStringList &arguments, const QString &options,
                                               QMap<QString, QString> &result, bool allowOverride = false);
B_CORE_EXPORT QStringList removeAll(const QStringList &list, const QString &what,
                                    Qt::CaseSensitivity cs = Qt::CaseSensitive, int *count  = 0);
B_CORE_EXPORT int removeAll(QStringList *list, const QString &what, Qt::CaseSensitivity cs = Qt::CaseSensitive);
B_CORE_EXPORT QStringList removeDuplicates(const QStringList &list, Qt::CaseSensitivity cs = Qt::CaseSensitive,
                                          int *count  = 0);
B_CORE_EXPORT int removeDuplicates(QStringList *list, Qt::CaseSensitivity cs = Qt::CaseSensitive);
B_CORE_EXPORT QString removeTrailingSpaces(const QString &s);
B_CORE_EXPORT void removeTrailingSpaces(QString *s);
B_CORE_EXPORT void removeUnsuppottedSymbols(QString &s);
B_CORE_EXPORT QString &replace(QString &where, const QString &oldText, const QString &newText,
                               Qt::CaseSensitivity cs = Qt::CaseSensitive, bool wholeWords = false);
B_CORE_EXPORT QString replaceTabs(const QString &s, BeQt::TabWidth tw);
B_CORE_EXPORT void replaceTabs(QString *s, BeQt::TabWidth tw);
B_CORE_EXPORT QStringList sortComprising(const QStringList &list, Qt::CaseSensitivity cs = Qt::CaseSensitive);
B_CORE_EXPORT void sortComprising(QStringList *list, Qt::CaseSensitivity cs = Qt::CaseSensitive);
B_CORE_EXPORT QStringList splitCommand(const QString &cmd, bool *ok = 0);
B_CORE_EXPORT QStringList splitCommand(const QString &cmd, QString &command, bool *ok = 0);
B_CORE_EXPORT QString splitCommand(const QString &cmd, QStringList &arguments, bool *ok = 0);
B_CORE_EXPORT QRegExp standardRegExp(RegExpPattern type);
B_CORE_EXPORT QString standardRegExpPattern(RegExpPattern type);
B_CORE_EXPORT int termFrequency(const QString &term, const QString &document);
B_CORE_EXPORT QList<double> tfidf(const QString &term, const QStringList &documents);
B_CORE_EXPORT QList<double> tfidf(const QStringList &terms, const QStringList &documents);
B_CORE_EXPORT QList<int> tfidfSortedIndexes(const QString &term, const QStringList &documents);
B_CORE_EXPORT QList<int> tfidfSortedIndexes(const QStringList &terms, const QStringList &documents);
B_CORE_EXPORT QString toHtml(const QString &text, bool replaceSpaces = true);
B_CORE_EXPORT void unwrap(QString &text, const QString &wrappingText = "\"");
B_CORE_EXPORT QString unwrapped(const QString &text, const QString &wrappingText = "\"");
B_CORE_EXPORT QString withoutUnsuppottedSymbols(const QString &s);
B_CORE_EXPORT void wrap(QString &text, const QString &wrappingText = "\"");
B_CORE_EXPORT QString wrapped(const QString &text, const QString &wrappingText = "\"");

}

#endif // BTEXTTOOLS_H
