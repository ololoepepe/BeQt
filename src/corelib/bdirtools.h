#ifndef BDIRTOOLS_H
#define BDIRTOOLS_H

class QTextCodec;
class QByteArray;

#include "bglobal.h"
#include "bcoreapplication.h"

#include <QString>
#include <QStringList>
#include <QList>

/*============================================================================
================================ BDirTools ===================================
============================================================================*/

namespace BDirTools
{

enum ResourceLookupMode
{
    AllResources,
    GlobalOnly,
    UserOnly
};

B_CORE_EXPORT bool touch(const QString &filePath);
B_CORE_EXPORT bool mkpath(const QString &dirPath);
B_CORE_EXPORT bool rmdir(const QString &dirName);
B_CORE_EXPORT bool removeFilesInDir( const QString &dirName, const QStringList &nameFilters = QStringList() );
B_CORE_EXPORT bool copyDir(const QString &dirName, const QString &newDirName, bool recursively = false);
B_CORE_EXPORT QString localeBasedFileName( const QString &fileName, const QLocale &loc = BCoreApplication::locale() );
B_CORE_EXPORT QString localeBasedDirName( const QString &path, const QString &subpath = QString(),
                                          const QLocale &loc = BCoreApplication::locale() );
B_CORE_EXPORT QByteArray readFile(const QString &fileName, qint64 maxlen = -1);
B_CORE_EXPORT bool writeFile(const QString &fileName, const QByteArray &data);
B_CORE_EXPORT QString readTextFile(const QString &fileName, QTextCodec *codec = 0);
B_CORE_EXPORT QString readTextFile(const QString &fileName, const QString &codecName);
B_CORE_EXPORT bool writeTextFile(const QString &fileName, const QString &text, QTextCodec *codec = 0);
B_CORE_EXPORT bool writeTextFile(const QString &fileName, const QString &text, const QString &codecName);
B_CORE_EXPORT QString findResource(const QString &subpath, ResourceLookupMode mode = AllResources);
B_CORE_EXPORT QString findResource(const QString &subpath, const QStringList &locations);
B_CORE_EXPORT bool createUserLocation(BCoreApplication::Location loc);
B_CORE_EXPORT bool createUserLocation(const QString &subdir);
B_CORE_EXPORT bool createUserLocations(const QStringList &subdirs);

}

#endif // BDIRTOOLS_H
