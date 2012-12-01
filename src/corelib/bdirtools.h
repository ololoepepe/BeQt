#ifndef BDIRTOOLS_H
#define BDIRTOOLS_H

class QTextCodec;

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

struct LocaleBasedSource
{
    QString fileName;
    QString defaultFileName;
    QString possibleSuffix;
};

B_CORE_EXPORT bool mkpath(const QString &dirPath);
B_CORE_EXPORT bool rmdir(const QString &dirName);
B_CORE_EXPORT bool removeFilesInDir( const QString &dirName, const QStringList &nameFilters = QStringList() );
B_CORE_EXPORT bool copyDir(const QString &dirName, const QString &newDirName, bool recursively = false);
/*B_CORE_EXPORT LocaleBasedSource createLocaleBasedSource( const QString &fileName, const QString &defaultFileName,
                                                         const QString &possibleSuffix = QString() );*/
B_CORE_EXPORT QString localeBasedFileName( const QString &fileName, const QLocale &loc = BCoreApplication::locale() );
/*B_CORE_EXPORT QString localeBasedFileName( const QString &fileName, const QString &defaultFileName,
                                           const QString &possibleSuffix = QString(),
                                           const QLocale &loc = BCoreApplication::locale() );*/
B_CORE_EXPORT QString localeBasedDirName( const QString &path, const QString &subpath = QString(),
                                          const QLocale &loc = BCoreApplication::locale() );
B_CORE_EXPORT QString readTextFile(const QString &fileName, QTextCodec *codec = 0);
B_CORE_EXPORT QString readTextFile(const QString &fileName, const QString &codecName);
B_CORE_EXPORT QString findResource(const QString &subpath, ResourceLookupMode mode = AllResources);
B_CORE_EXPORT QString findResource(const QString &subpath, const QStringList &locations);
B_CORE_EXPORT bool createUserLocation(BCoreApplication::Location loc);
B_CORE_EXPORT bool createUserLocation(const QString &subdir);
B_CORE_EXPORT bool createUserLocations(const QStringList &subdirs);

}

#endif // BDIRTOOLS_H
