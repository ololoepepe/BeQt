#include "bdirtools.h"

#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QFile>

bool BDirTools::mkpath(const QString &dirPath)
{
    return QDir(dirPath).mkpath(dirPath);
}

bool BDirTools::rmdir(const QString &dirName)
{
    QDir d(dirName);
    QStringList dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < dirs.size(); ++i)
        if ( !rmdir( d.filePath( dirs.at(i) ) ) )
            return false;
    return removeFilesInDir(dirName) && d.rmdir(dirName);
}

bool BDirTools::removeFilesInDir(const QString &dirName, const QStringList &nameFilters)
{
    QDir d(dirName);
    if ( !QFileInfo(dirName).isDir() )
        return false;
    QStringList files = d.entryList(nameFilters, QDir::Files);
    for (int i = 0; i < files.size(); ++i)
        if ( !d.remove( files.at(i) ) )
            return false;
    return true;
}

bool BDirTools::copyDir(const QString &dirName, const QString &newDirName, bool recursively)
{
    QDir d(dirName);
    QDir nd(newDirName);
    if ( !d.exists() || !mkpath(newDirName) )
        return false;
    QStringList files = d.entryList(QDir::Files);
    for (int i = 0; i < files.size(); ++i)
    {
        const QString &bfn = files.at(i);
        if ( !QFile::copy( d.filePath(bfn), nd.filePath(bfn) ) )
            return false;

    }
    if (recursively)
    {
        QStringList dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (int i = 0; i < dirs.size(); ++i)
        {
            const QString &bdn = dirs.at(i);
            if ( !copyDir( d.filePath(bdn), nd.filePath(bdn) ) )
                return false;
        }
    }
    return true;
}

//

BDirTools::BDirTools()
{
}
