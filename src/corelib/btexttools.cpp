#include "btexttools.h"
#include "btextmatch.h"
#include "btextmatchlist.h"
#include "bnamespace.h"
#include "bdirtools.h"

#if defined(BUILDING_LIBENCA)
#include "lib/enca.h"
#else
#include <lib/enca.h>
#endif

#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QList>
#include <QMap>
#include <QByteArray>
#include <QLocale>
#include <QTextCodec>

#include <math.h>

#include <QDebug>

/*============================================================================
================================ BTextTools ==================================
============================================================================*/

namespace BTextTools
{

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

QString wrapped(const QString &text, const QString &wrappingText)
{
    QString ntext = text;
    wrap(ntext, wrappingText);
    return ntext;
}

QString unwrapped(const QString &text, const QString &wrappingText)
{
    QString ntext = text;
    unwrap(ntext, wrappingText);
    return ntext;
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

QString appendTrailingSpaces(const QString &s, int ll)
{
    QString ns = s;
    appendTrailingSpaces(&ns, ll);
    return ns;
}

void appendTrailingSpaces(QString *s, int ll)
{
    if (!s)
        return;
    int len = s->length();
    if (len < ll)
        s->append(QString().fill(' ', ll - len));
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
    while (i < s->length())
    {
        if (s->at(i) == '\t')
        {
            int spcount = tw - (i < tw ? i : i % tw);
            s->replace(i, 1, QString().fill(' ', spcount ? spcount : tw));
            i += spcount;
        }
        else
        {
            ++i;
        }
    }
}

QString standardRegExpPattern(RegExpPattern type)
{
    static const QString email = "^[-a-z0-9!#$%&'*+/=?^_`{|}~]+(?:\\.[-a-z0-9!#$%&'*+/=?^_`{|}~]+)*@"
        "(?:[a-z0-9]([-a-z0-9]{0,61}[a-z0-9])?\\.)*"
        "(?:aero|arpa|asia|biz|cat|com|coop|edu|gov|info|int|jobs|mil|mobi|museum|name|net|org|pro|tel|travel|"
        "[a-z][a-z])$";
    switch (type)
    {
    case EmailPattern:
        return email;
    default:
        return "";
    }
}

QRegExp standardRegExp(RegExpPattern type)
{
    return QRegExp(standardRegExpPattern(type));
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
    foreach (int i, bRangeR(list->size() - 1, 1))
    {
        foreach (int j, bRangeR(i - 1, 0))
        {
            if (!list->at(i).compare(list->at(j), cs))
            {
                list->removeAt(i);
                ++count;
                continue;
            }
        }
    }
    return count;
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
    foreach (int i, bRangeR(list->size() - 1, 0))
    {
        if (!what.compare(list->at(i), cs))
        {
            list->removeAt(i);
            ++count;
        }
    }
    return count;
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
    foreach (int i, bRangeR(list->size() - 1, 0))
    {
        foreach (int j, bRangeR(list->size() - 1, 0))
        {
            if (i != j && list->at(j).contains(list->at(i), cs))
            {
                QString s = list->takeAt(i);
                bool b = false;
                foreach (int k, bRangeD(0, nlist.size() - 1))
                {
                    if (s.contains(nlist.at(k), cs))
                    {
                        if (!s.compare(nlist.at(k), cs))
                            nlist.insert(k, s);
                        else
                            nlist.prepend(s);
                        b = true;
                        break;
                    }
                }
                if (!b)
                    nlist.append(s);
                break;
            }
        }
    }
    *list += nlist;
}

BTextMatchList match(const QString &text, const QRegExp &what, const QRegExp &prefixedBy, const QRegExp &postfixedBy)
{
    BTextMatchList list;
    if (text.isEmpty() || !what.isValid())
        return list;
    QStringList sl = text.split('\n');
    int coveredLength = 0;
    foreach (int i, bRangeD(0, sl.size() - 1))
    {
        const QString &line = sl.at(i);
        int pos = what.indexIn(line);
        while (pos >= 0)
        {
            bool b = true;
            int len = what.matchedLength();
            if (!prefixedBy.isEmpty() && prefixedBy.isValid())
            {
                int prind = prefixedBy.indexIn(line.mid(0, pos));
                if (prind < 0 || prind + prefixedBy.matchedLength() != pos)
                {
                    prind = prefixedBy.indexIn(line.mid(pos, len));
                    if (prind == 0)
                    {
                        pos += prefixedBy.matchedLength();
                        len -= prefixedBy.matchedLength();
                    }
                    else
                    {
                        b = false;
                    }
                }
            }
            if (!postfixedBy.isEmpty() && postfixedBy.isValid())
            {
                int poind = postfixedBy.indexIn(line.mid(pos + len));
                if (poind != 0)
                {
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

bool intersects(const QStringList &list1, const QStringList &list2, Qt::CaseSensitivity cs)
{
    foreach (const QString &s1, list1)
        if (list2.contains(s1, cs))
            return true;
    return false;
}

int termFrequency(const QString &term, const QString &document)
{
    if (term.isEmpty() || document.isEmpty())
        return 0;
    return document.toLower().count(QRegExp("\\b" + term.toLower() + "\\b"));
}

double inverseDocumentFrequency(const QString &term, const QStringList &documents, QList<int> *tfs)
{
    if (term.isEmpty() || documents.isEmpty())
        return 0.0;
    int count = 0;
    if (tfs)
    {
        tfs->clear();
        foreach (const QString &document, documents)
        {
            int tf = termFrequency(term, document);
            if (tf)
                ++count;
            tfs->append(tf);
        }
    }
    else
    {
        foreach (const QString &document, documents)
            if (document.toLower().contains(QRegExp("\\b" + term.toLower() + "\\b")))
                ++count;
    }
    if (!count)
        return 0.0;
    if (documents.size() == count)
        return ++count;
    return log(double(documents.size()) / double(count));
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
    foreach (const QString &term, terms)
    {
        tfs << QList<int>();
        idfs << inverseDocumentFrequency(term, documents, &tfs.last());
    }
    foreach (int i, bRangeD(0, documents.size() - 1))
    {
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
    foreach (int i, bRangeR(list.size() - 1, 0))
    {
        double d = list.at(i);
        if ((d + 1.0) == 1.0)
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
    foreach (int i, bRangeR(list.size() - 1, 0))
    {
        double d = list.at(i);
        if ((d + 1.0) == 1.0)
            continue;
        ret << map.values(d);
    }
    return ret;
}

QString guessTextCodecName(const QByteArray &data, const QLocale locale)
{
    struct Buffer
    {
        size_t size;
        ssize_t pos;
        const unsigned char *data;
    };
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

QTextCodec *guessFileCodec(const QString &fileName, const QLocale locale)
{
    return BeQt::codec(guessFileCodecName(fileName, locale));
}

QTextCodec *guessFileCodec(const QString &fileName, qint64 bytes, const QLocale locale)
{
    return BeQt::codec(guessFileCodecName(fileName, bytes, locale));
}

}
