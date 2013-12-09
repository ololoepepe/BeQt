#include "btexttools.h"
#include "btextmatch.h"
#include "btextmatchlist.h"
#include "bnamespace.h"

#include <QString>
#include <QStringList>
#include <QRegExp>

#include <QDebug>

/*============================================================================
================================ BTextTools ==================================
============================================================================*/

namespace BTextTools
{

QString wrapped(const QString &text, const QString &wrappingText)
{
    if (text.isEmpty() || wrappingText.isEmpty())
        return text;
    int wl = wrappingText.length();
    QString ntext = text;
    if (text.left(wl) != wrappingText)
        ntext.prepend(wrappingText);
    if (text.right(wl) != wrappingText)
        ntext.append(wrappingText);
    return ntext;
}

QString unwrapped(const QString &text, const QString &wrappingText)
{
    if (text.isEmpty() || wrappingText.isEmpty())
        return text;
    int wl = wrappingText.length();
    QString ntext = text;
    if (ntext.left(wl) == wrappingText)
        ntext.remove(0, wl);
    if (ntext.right(wl) == wrappingText)
        ntext.remove(ntext.length() - wl, wl);
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

}
