#ifndef BTEXTTOOLS_H
#define BTEXTTOOLS_H

class BTextMatchList;

class QStringList;
class QString;

#include "bglobal.h"
#include "bnamespace.h"

#include <QtGlobal>
#include <QRegExp>

/*============================================================================
================================ BTextTools ==================================
============================================================================*/

namespace BTextTools
{

enum RegExpPattern
{
    EmailPattern
};

B_CORE_EXPORT QString wrapped(const QString &text, const QString &wrappingText = "\"");
B_CORE_EXPORT QString unwrapped(const QString &text, const QString &wrappingText = "\"");
B_CORE_EXPORT QString removeTrailingSpaces(const QString &s);
B_CORE_EXPORT void removeTrailingSpaces(QString *s);
B_CORE_EXPORT QString appendTrailingSpaces(const QString &s, int ll);
B_CORE_EXPORT void appendTrailingSpaces(QString *s, int ll);
B_CORE_EXPORT QString replaceTabs(const QString &s, BeQt::TabWidth tw);
B_CORE_EXPORT void replaceTabs(QString *s, BeQt::TabWidth tw);
B_CORE_EXPORT QString standardRegExpPattern(RegExpPattern type);
B_CORE_EXPORT QStringList removeDuplicates(const QStringList &list, Qt::CaseSensitivity cs = Qt::CaseSensitive,
                                          int *count  = 0);
B_CORE_EXPORT int removeDuplicates(QStringList *list, Qt::CaseSensitivity cs = Qt::CaseSensitive);
B_CORE_EXPORT QStringList removeAll(const QStringList &list, const QString &what,
                                    Qt::CaseSensitivity cs = Qt::CaseSensitive, int *count  = 0);
B_CORE_EXPORT int removeAll(QStringList *list, const QString &what, Qt::CaseSensitivity cs = Qt::CaseSensitive);
B_CORE_EXPORT QStringList sortComprising(const QStringList &list, Qt::CaseSensitivity cs = Qt::CaseSensitive);
B_CORE_EXPORT void sortComprising(QStringList *list, Qt::CaseSensitivity cs = Qt::CaseSensitive);
B_CORE_EXPORT BTextMatchList match(const QString &text, const QRegExp &what, const QRegExp &prefixedBy = QRegExp(),
                                  const QRegExp &postfixedBy = QRegExp());
B_CORE_EXPORT bool intersects(const QStringList &list1, const QStringList &list2,
                              Qt::CaseSensitivity cs = Qt::CaseSensitive);

}

#endif // BTEXTTOOLS_H
