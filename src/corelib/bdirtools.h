#ifndef BDIRTOOLS_H
#define BDIRTOOLS_H

#include <BeQt>

#include <QString>
#include <QStringList>

class B_CORE_EXPORT BDirTools
{
public:
    static bool mkpath(const QString &dirPath);
    static bool rmdir(const QString &dirName);
    static bool removeFilesInDir( const QString &dirName, const QStringList &nameFilters = QStringList() );
    static bool copyDir(const QString &dirName, const QString &newDirName, bool recursively = false);
private:
    BDirTools();
};

#endif // BDIRTOOLS_H
