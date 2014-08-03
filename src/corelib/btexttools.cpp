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
================================ BTextTools ==================================
============================================================================*/

namespace BTextTools
{

QString appendTrailingSpaces(const QString &s, int lineLength)
{
    QString ns = s;
    appendTrailingSpaces(&ns, lineLength);
    return ns;
}

void appendTrailingSpaces(QString *s, int lineLength)
{
    if (!s)
        return;
    int len = s->length();
    if (len < lineLength)
        s->append(QString().fill(' ', lineLength - len));
}

QTextCodec *guessFileCodec(const QString &fileName, const QLocale locale)
{
    return BeQt::codec(guessFileCodecName(fileName, locale));
}

QTextCodec *guessFileCodec(const QString &fileName, qint64 bytes, const QLocale locale)
{
    return BeQt::codec(guessFileCodecName(fileName, bytes, locale));
}

QString guessFileCodecName(const QString &fileName, const QLocale locale)
{
    return guessFileCodecName(fileName, -1, locale);
}

QString guessFileCodecName(const QString &fileName, qint64 bytes, const QLocale locale)
{
    if (fileName.isEmpty())
        return "";
    bool ok = false;
    QByteArray data = BDirTools::readFile(fileName, bytes, &ok);
    return ok ? guessTextCodecName(data, locale) : QString();
}

QTextCodec *guessTextCodec(const QByteArray &data, const QLocale locale)
{
    return BeQt::codec(guessTextCodecName(data, locale));
}

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

bool intersects(const QStringList &list1, const QStringList &list2, Qt::CaseSensitivity cs)
{
    foreach (const QString &s1, list1) {
        if (list2.contains(s1, cs))
            return true;
    }
    return false;
}

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

QString mergeArguments(const QStringList &list)
{
    QString args;
    foreach (const QString &a, list)
        args += (a.contains(' ') ? BTextTools::wrapped(a) : a) + " ";
    if (!args.isEmpty())
        args.remove(args.length() - 1, 1);
    return args;
}

QString mergeArguments(const QString &command, const QStringList &arguments)
{
    return mergeArguments(QStringList() << command << arguments);
}

QStringList removeAll(const QStringList &list, const QString &what, Qt::CaseSensitivity cs, int *count)
{
    QStringList nlist = list;
    int c = removeAll(&nlist, what, cs);
    return bRet(count, c, nlist);
}

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

QStringList removeDuplicates(const QStringList &list, Qt::CaseSensitivity cs, int *count)
{
    QStringList nlist = list;
    int c = removeDuplicates(&nlist, cs);
    return bRet(count, c, nlist);
}

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

QString removeTrailingSpaces(const QString &s)
{
    QString ns = s;
    removeTrailingSpaces(&ns);
    return ns;
}

void removeTrailingSpaces(QString *s)
{
    if (!s || s->isEmpty())
        return;
    QStringList sl = s->split('\n');
    foreach (int i, bRangeD(0, sl.size() - 1))
        sl[i].remove(QRegExp("\\s+$"));
    *s = sl.join("\n");
}

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

QString replaceTabs(const QString &s, BeQt::TabWidth tw)
{
    QString ns = s;
    replaceTabs(&ns, tw);
    return ns;
}

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

QStringList sortComprising(const QStringList &list, Qt::CaseSensitivity cs)
{
    QStringList nlist = list;
    sortComprising(&nlist, cs);
    return nlist;
}

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

QRegExp standardRegExp(RegExpPattern type)
{
    switch (type) {
    case EmailPattern:
        return QRegExp(standardRegExpPattern(type));
    default:
        return QRegExp();
    }
}

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

int termFrequency(const QString &term, const QString &document)
{
    if (term.isEmpty() || document.isEmpty())
        return 0;
    return document.toLower().count(QRegExp("\\b" + term.toLower() + "\\b"));
}

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

QString toHtml(const QString &text, bool replaceSpaces)
{
    typedef QMap<QChar, QString> KeywordMap;
    init_once(KeywordMap, keywords, KeywordMap()) {
        keywords.insert('&', "&amp;");
        keywords.insert('<', "&lt;");
        keywords.insert('>', "&gt;");
        keywords.insert(' ', "&nbsp;");
        keywords.insert('\"', "&quot;");
    }
    if (text.isEmpty())
        return text;
    QString html = text;
    foreach (int i, bRangeR(html.length() - 1, 0)) {
        const QChar &c = html.at(i);
        if (!replaceSpaces && ' ' == c)
            continue;
        if (keywords.contains(c))
            html.replace(i, 1, keywords.value(c));
    }
    return html;
}

void unwrap(QString &text, const QString &wrappingText)
{
    if (text.isEmpty() || wrappingText.isEmpty())
        return;
    int wl = wrappingText.length();
    if (text.left(wl) == wrappingText)
        text.remove(0, wl);
    if (text.right(wl) == wrappingText)
        text.remove(text.length() - wl, wl);
}

QString unwrapped(const QString &text, const QString &wrappingText)
{
    QString ntext = text;
    unwrap(ntext, wrappingText);
    return ntext;
}

QString withoutUnsuppottedSymbols(const QString &s)
{
    if (s.isEmpty())
        return s;
    QString ns = s;
    removeUnsuppottedSymbols(ns);
    return ns;
}

void wrap(QString &text, const QString &wrappingText)
{
    if (text.isEmpty() || wrappingText.isEmpty())
        return;
    int wl = wrappingText.length();
    if (text.left(wl) != wrappingText)
        text.prepend(wrappingText);
    if (text.right(wl) != wrappingText)
        text.append(wrappingText);
}

QString wrapped(const QString &text, const QString &wrappingText)
{
    QString ntext = text;
    wrap(ntext, wrappingText);
    return ntext;
}

}
