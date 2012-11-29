#include "bdirtools.h"
#include "bcoreapplication.h"
#include "bcoreapplication_p.h"

#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QFile>
#include <QLocale>
#include <QTextStream>
#include <QTextCodec>
#include <QList>
#include <QByteArray>

#include <QDebug>

/*============================================================================
================================ BDirTools ===================================
============================================================================*/

namespace BDirTools
{

bool mkpath(const QString &dirPath)
{
    return QDir(dirPath).mkpath(dirPath);
}

bool rmdir(const QString &dirName)
{
    QDir d(dirName);
    QStringList dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < dirs.size(); ++i)
        if ( !rmdir( d.filePath( dirs.at(i) ) ) )
            return false;
    return removeFilesInDir(dirName) && d.rmdir(dirName);
}

bool removeFilesInDir(const QString &dirName, const QStringList &nameFilters)
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

bool copyDir(const QString &dirName, const QString &newDirName, bool recursively)
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

LocaleBasedSource createLocaleBasedSource(const QString &fileName, const QString &defaultFileName,
                                          const QString &possibleSuffix)
{
    LocaleBasedSource r;
    r.fileName = fileName;
    r.defaultFileName = defaultFileName;
    r.possibleSuffix = possibleSuffix;
    return r;
}

QString localeBasedFileName(const LocaleBasedSource &src)
{
    return localeBasedFileName(src.fileName, src.defaultFileName, src.possibleSuffix);
}

QString localeBasedFileName(const QString &fileName, const QString &defaultFileName,
                                   const QString &possibleSuffix)
{
    if ( fileName.isEmpty() )
        return "";
    QString lname = BCoreApplication::locale().name().left(5);
    QFileInfo fi(fileName);
    QString bfn = fi.path() + "/" + fi.baseName();
    QString suff = fi.suffix();
    suff = suff.isEmpty() ? possibleSuffix : "";
    if ( !suff.isEmpty() )
        suff.prepend('.');
    QFile f(bfn + "_" + lname);
    if ( !f.exists() )
        f.setFileName(bfn + "_" + lname + suff);
    lname = lname.left(2);
    if ( !f.exists() )
        f.setFileName(bfn + "_" + lname);
    if ( !f.exists() )
        f.setFileName(bfn + "_" + lname + suff);
    if ( !f.exists() )
        f.setFileName(defaultFileName);
    if ( !f.exists() )
        f.setFileName(defaultFileName + suff);
    if ( !f.exists() )
        return "";
    return f.fileName();
}

QString localeBasedDirName(const QString &dir)
{
    if ( dir.isEmpty() )
        return "";
    QString lname = BCoreApplication::locale().name().left(5);
    QDir d(dir + "/" + lname);
    if ( !d.exists() )
        d.setPath( dir + "/" + lname.left(2) );
    if ( !d.exists() )
        d.setPath(dir + "/" + "en");
    if ( !d.exists() )
        d.setPath(dir);
    if ( !d.exists() )
        return "";
    return d.path();
}

QString readTextFile(const QString &fileName, QTextCodec *codec)
{
    if ( fileName.isEmpty() )
        return "";
    QFile f(fileName);
    if ( !f.open(QFile::ReadOnly) )
        return "";
    QTextStream in(&f);
    if (codec)
        in.setCodec(codec);
    QString text = in.readAll();
    f.close();
    return text;
}

QString readTextFile(const QString &fileName, const QString &codecName)
{
    return readTextFile( fileName, QTextCodec::codecForName( codecName.toLatin1() ) );
}

QString findResource(const QString &subpath, ResourceLookupMode mode)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BDirTools") )
        return "";
    QStringList sl;
    switch (mode)
    {
    case GlobalOnly:
    {
        sl << BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::SharedResources);
#if defined(Q_OS_MAC)
        sl << BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::BundleResources);
#endif
        sl << BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::BuiltinResources);
        break;
    }
    case UserOnly:
    {
        sl << BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::UserResources);
        break;
    }
    case AllResources:
    default:
    {
        sl << BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::SharedResources);
#if defined(Q_OS_MAC)
        sl << BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::BundleResources);
#endif
        sl << BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::BuiltinResources);
        sl << BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::UserResources);
        break;
    }
    }
    return findResource(subpath, sl);
}

QString findResource(const QString &subpath, const QStringList &locations)
{
    if ( subpath.isEmpty() || locations.isEmpty() )
        return "";
    foreach (const QString &loc, locations)
        if ( !loc.isEmpty() && QFileInfo(loc + "/" + subpath).exists() )
            return loc + "/" + subpath;
    return "";
}

bool createUserLocation(BCoreApplication::Location loc)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BDirTools") )
        return false;
    return mkpath( BCoreApplicationPrivate::instance()->userPrefix + "/" + BCoreApplicationPrivate::subdir(loc) );
}

bool createUserLocation(const QString &subdir)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BDirTools") )
        return false;
    return mkpath(BCoreApplicationPrivate::instance()->userPrefix + "/" + subdir);
}

bool createUserLocations(const QStringList &subdirs)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BDirTools") )
        return false;
    foreach (const QString &subdir, subdirs)
        if ( !createUserLocation(subdir) )
            return false;
    return true;
}

}
