#ifndef BDIRTOOLS_H
#define BDIRTOOLS_H

#include "bnamespace.h"

#include <QString>
#include <QStringList>

class B_CORE_EXPORT BDirTools
{
public:
    static bool mkpath(const QString &dirPath);
    static bool rmdir(const QString &dirName);
    static bool removeFilesInDir( const QString &dirName, const QStringList &nameFilters = QStringList() );
    static bool copyDir(const QString &dirName, const QString &newDirName, bool recursively = false);
    static QString localeBasedFileName( const QString &fileName, const QString &defaultFileName,
                                        const QString &possibleSuffix = QString() );
    static QString localeBasedDirName(const QString &dir);
private:
    BDirTools();
};

#endif // BDIRTOOLS_H
