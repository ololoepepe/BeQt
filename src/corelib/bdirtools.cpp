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
#include <QRegExp>

#include <QDebug>

/*============================================================================
================================ BDirTools ===================================
============================================================================*/

namespace BDirTools
{

bool fileExists(const QString &fileName)
{
    if (fileName.isEmpty())
        return false;
    QFileInfo fi(fileName);
    return fi.isAbsolute() && fi.isFile();
}

bool mayBeExecutable(const QString &fileName, bool considerSuffix)
{
    return mayBeExecutable(readFile(fileName, 4), considerSuffix ? fileName : QString());
}

bool mayBeExecutable(const QByteArray &data, const QString &fileName)
{
    if (data.mid(0, 4) == "\x7F""ELF" || data.mid(0, 2) == "MZ" || data.mid(0, 2) == "#!")
        return true;
    if (!fileName.isEmpty())
        return false;
    QString suffix = QFileInfo(fileName).suffix();
    if (!suffix.compare("dylib", Qt::CaseInsensitive) || !suffix.compare("dll", Qt::CaseInsensitive)
            || !suffix.compare("bat", Qt::CaseInsensitive))
        return true;
    if (!fileName.indexOf(QRegExp("so(\\.\\d+)?(\\.\\d+)?(\\.\\d+)?$")))
        return true;
    return false;
}

bool dirExists(const QString &dirName)
{
    if (dirName.isEmpty())
        return false;
    QFileInfo fi(dirName);
    return fi.isAbsolute() && fi.isDir();
}

bool touch(const QString &filePath)
{
    if ( filePath.isEmpty() )
        return false;
    QFileInfo fi(filePath);
    if ( fi.exists() && fi.isFile() )
        return true;
    if ( !mkpath( fi.path() ) )
        return false;
    QFile f(filePath);
    if ( !f.open(QFile::WriteOnly) )
        return false;
    f.close();
    return true;
}

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

bool removeFilesInDir(const QString &dirName, const QStringList &nameFilters, const QStringList &excluding)
{
    QDir d(dirName);
    if ( !QFileInfo(dirName).isDir() )
        return false;
    foreach ( const QString &fn, d.entryList(nameFilters, QDir::Files) )
    {
        if ( excluding.contains(fn) )
            continue;
        if ( !d.remove(fn) )
            return false;
    }
    return true;
}

bool removeSubdirs(const QString &dirName, const QStringList &nameFilters, const QStringList &excluding)
{
    QDir d(dirName);
    if (!QFileInfo(dirName).isDir())
        return false;
    foreach (const QString &dn, d.entryList(nameFilters, QDir::Dirs | QDir::NoDotAndDotDot))
    {
        if (excluding.contains(dn))
            continue;
        if (!rmdir(d.absoluteFilePath(dn)))
            return false;
    }
    return true;
}

bool removeEntries(const QString &dirName, const QStringList &nameFilters, const QStringList &excluding)
{
    QDir d(dirName);
    if (!QFileInfo(dirName).isDir())
        return false;
    foreach (const QString &fn, d.entryList(nameFilters, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot))
    {
        if (excluding.contains(fn))
            continue;
        QFileInfo fi(d.absoluteFilePath(fn));
        if ((fi.isFile() && !d.remove(fn)) || (fi.isDir() && !rmdir(d.absoluteFilePath(fn))))
            return false;
    }
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

bool renameDir(const QString &oldName, const QString &newName)
{
    if ( oldName.isEmpty() || newName.isEmpty() )
        return false;
    return QDir(oldName).rename(oldName, newName);
}

bool moveDir(const QString &oldName, const QString &newName)
{
    return renameDir(oldName, newName) || (copyDir(oldName, newName, true) && rmdir(oldName));
}

QStringList entryList(const QString &dirName, const QStringList &nameFilters, QDir::Filters filters, QDir::SortFlags sort)
{
    QStringList sl;
    QDir d(dirName);
    foreach (const QString &fn, d.entryList(nameFilters, filters, sort))
        sl << d.absoluteFilePath(fn);
    return sl;
}

QStringList entryList(const QString &dirName, QDir::Filters filters, QDir::SortFlags sort)
{
    return entryList(dirName, QStringList(), filters, sort);
}

QString localeBasedFileName(const QString &fileName, const QLocale &loc)
{
    if ( fileName.isEmpty() )
        return "";
    QString lname = loc.name().left(5);
    QFileInfo fi(fileName);
    QString bfn = fi.path() + "/" + fi.baseName();
    QString suff = fi.suffix();
    if ( !suff.isEmpty() )
        suff.prepend('.');
    QStringList sl;
    sl << bfn + "_" + lname + suff;
    sl << bfn + "_" + lname.left(2) + suff;
    sl << bfn + "_" + "en" + suff;
    sl << fileName;
    foreach (const QString &fn, sl)
        if ( QFile(fn).exists() )
            return fn;
    return "";
}

QString localeBasedDirName(const QString &path, const QString &subpath, const QLocale &loc)
{
    if ( path.isEmpty() )
        return "";
    QString lname = loc.name().left(5);
    QDir d(path + "/" + subpath + lname);
    if ( !d.exists() )
        d.setPath( path + "/" + subpath + lname.left(2) );
    if ( !d.exists() )
        d.setPath(path + "/" + subpath + "en");
    if ( !d.exists() )
        d.setPath(path);
    if ( !d.exists() )
        return "";
    return d.path();
}

QByteArray readFile(const QString &fileName, qint64 maxlen, bool *ok)
{
    if (ok)
        *ok = false;
    if ( fileName.isEmpty() )
        return QByteArray();
    QFile f(fileName);
    if ( !f.open(QFile::ReadOnly) )
        return QByteArray();
    QByteArray ba = (maxlen < 0) ? f.readAll() : f.read(maxlen);
    f.close();
    if (ok)
        *ok = true;
    return ba;
}

bool writeFile(const QString &fileName, const QByteArray &data)
{
    if ( fileName.isEmpty() )
        return false;
    QFile f(fileName);
    if ( !touch(fileName) || !f.open(QFile::WriteOnly) )
        return false;
    bool b = ( f.write(data) == data.size() );
    f.close();
    return b;
}

QString readTextFile(const QString &fileName, QTextCodec *codec, bool *ok)
{
    if (ok)
        *ok = false;
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
    if (ok)
        *ok = true;
    return text;
}

QString readTextFile(const QString &fileName, const QString &codecName, bool *ok)
{
    return readTextFile(fileName, QTextCodec::codecForName( codecName.toLatin1() ), ok);
}

bool writeTextFile(const QString &fileName, const QString &text, QTextCodec *codec)
{
    if ( fileName.isEmpty() )
        return false;
    QFile f(fileName);
    if ( !touch(fileName) || !f.open(QFile::WriteOnly) )
        return false;
    QTextStream out(&f);
    if (codec)
        out.setCodec(codec);
    out << text;
    bool b = (out.status() != QTextStream::WriteFailed);
    f.close();
    return b;
}

bool writeTextFile(const QString &fileName, const QString &text, const QString &codecName)
{
    return writeTextFile( fileName, text, QTextCodec::codecForName( codecName.toLatin1() ) );
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
    return mkpath( BCoreApplicationPrivate::instance()->getUserPrefix() + "/" + BCoreApplicationPrivate::subdir(loc) );
}

bool createUserLocation(const QString &subdir)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BDirTools") )
        return false;
    return mkpath(BCoreApplicationPrivate::instance()->getUserPrefix() + "/" + subdir);
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
