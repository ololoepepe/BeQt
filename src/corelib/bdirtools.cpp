/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "bdirtools.h"

#include "bcoreapplication.h"
#include "bnamespace.h"
#include "bproperties.h"

#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLocale>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QTextStream>

/*============================================================================
================================ BDirTools ===================================
============================================================================*/

/*!
\namespace BDirTools
\inmodule BeQtCore

\brief The BDirTools namespace contains miscellaneous directory- and file-related functions used throughout the BeQt
library.
*/

/*!
\enum BDirTools::ResourceLookupMode

This type is used to specify where to search for a file or a directory.

\value GlobalOnly
\value UserOnly
\value AllResources
*/

namespace BDirTools
{

/*!
\fn bool BDirTools::appendFile(const QString &fileName, const QByteArray &data)
Appends \a data to the end of file \a fileName.

Returns true on success. If any error occurs, returns false.

The file is created automatically if it does not exist.

\sa writeFile()
*/

bool appendFile(const QString &fileName, const QByteArray &data)
{
    if (fileName.isEmpty())
        return false;
    QFile f(fileName);
    if (!touch(fileName) || !f.open(QFile::WriteOnly | QFile::Append))
        return false;
    bool b = (f.write(data) == data.size());
    f.close();
    return b;
}

/*!
\fn bool BDirTools::appendTextFile(const QString &fileName, const QString &text, QTextCodec *codec)
Appends \a text to the end of file \a fileName. \a codec is used to encode the text.

Returns true on success. If any error occurs, returns false.

The file is created automatically if it does not exist.

\sa writeTextFile()
*/

bool appendTextFile(const QString &fileName, const QString &text, QTextCodec *codec)
{
    if (fileName.isEmpty())
        return false;
    QFile f(fileName);
    if (!touch(fileName) || !f.open(QFile::WriteOnly | QFile::Append))
        return false;
    QTextStream out(&f);
    if (codec)
        out.setCodec(codec);
    out << text;
    bool b = (out.status() != QTextStream::WriteFailed);
    f.close();
    return b;
}

/*!
\fn bool BDirTools::appendTextFile(const QString &fileName, const QString &text, const QString &codecName)
\overload appendTextFile()
Appends \a text to the end of file \a fileName. A codec with name \a codecName is used to encode the text.

Returns true on success. If any error occurs, returns false.

The file is created automatically if it does not exist.

\sa writeTextFile()
*/

bool appendTextFile(const QString &fileName, const QString &text, const QString &codecName)
{
    return appendTextFile(fileName, text, QTextCodec::codecForName(codecName.toLatin1()));
}

/*!
\fn bool BDirTools::copyDir(const QString &dirName, const QString &newDirName, bool recursively)
Copies all files in directory \a dirName to new location \a newDirName.

If \a recursively is true, all subdirectories are also copied.

Returns true on success. If any error occurs, returns false.

The target directory is created automatically if it does not exist.
*/

bool copyDir(const QString &dirName, const QString &newDirName, bool recursively)
{
    QDir d(dirName);
    QDir nd(newDirName);
    if (!d.exists() || !mkpath(newDirName))
        return false;
    QStringList files = d.entryList(QDir::Files);
    for (int i = 0; i < files.size(); ++i) {
        const QString &bfn = files.at(i);
        if (!QFile::copy(d.filePath(bfn), nd.filePath(bfn)))
            return false;

    }
    if (recursively) {
        QStringList dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (int i = 0; i < dirs.size(); ++i) {
            const QString &bdn = dirs.at(i);
            if (!copyDir( d.filePath(bdn), nd.filePath(bdn)))
                return false;
        }
    }
    return true;
}

/*!
\fn QString BDirTools::createConfFileName(const QString &name)
Generates absolute .conf file name for the given identifier \a name.

Path to the file will be different on different operating systems. The identifier itself may also be modified. See
\l {Resources system} for details.

\note If there is no valid BApplicationBase instance, an empty QString will be returned.
*/

QString createConfFileName(const QString &name)
{
    if (name.isEmpty())
        return QString();
    QString path = BApplicationBase::location(BApplicationBase::SettingsPath, BApplicationBase::UserResource);
    if (path.isEmpty())
        return QString();
#if defined(Q_OS_UNIX)
    QString nname = name.toLower().replace(QRegExp("\\s"), "-");
#else
    QString nname = name;
#endif
    return path + "/" + nname + ".conf";
}

/*!
\fn bool BDirTools::dirExists(const QString &dirName)
Checks if \a dirName is a valid absolute path to an existing directory.

Returns true on success. Otherwise returns false.

\sa fileExists()
*/

bool dirExists(const QString &dirName)
{
    if (dirName.isEmpty())
        return false;
    QFileInfo fi(dirName);
    return fi.isAbsolute() && fi.isDir();
}

/*!
\fn QStringList BDirTools::entryList(const QString &dirName, const QStringList &nameFilters, QDir::Filters filters,
                                     QDir::SortFlags sort)
Returns a list of files or directories in directory \a dirName, matching \a nameFilters and \a filters. \a sort
determines if the list must be sorted.

The same as QDir::entryList(), but the paths returned are absolute.

\sa entryListRecursive()
*/

QStringList entryList(const QString &dirName, const QStringList &nameFilters, QDir::Filters filters,
                      QDir::SortFlags sort)
{
    QStringList sl;
    QDir d(dirName);
    foreach (const QString &fn, d.entryList(nameFilters, filters, sort))
        sl << d.absoluteFilePath(fn);
    return sl;
}

/*!
\fn QStringList BDirTools::entryList(const QString &dirName, QDir::Filters filters, QDir::SortFlags sort)
\overload entryList()
Returns a list of files or directories in directory \a dirName, matching \a filters. \a sort determines if the list
must be sorted.

The same as QDir::entryList(), but the paths returned are absolute.

\sa entryListRecursive()
*/

QStringList entryList(const QString &dirName, QDir::Filters filters, QDir::SortFlags sort)
{
    return entryList(dirName, QStringList(), filters, sort);
}

/*!
\fn QStringList BDirTools::entryListRecursive(const QString &dirName, const QStringList &nameFilters,
                                              QDir::Filters filters, QDir::SortFlags sort)
Returns a list of files or directories in directory \a dirName and all it's subdirectories, matching \a nameFilters and
\a filters. \a sort determines if the list must be sorted.

The same as calling QDir::entryList() recursively, but the paths returned are absolute.

\sa entryList()
*/

QStringList entryListRecursive(const QString &dirName, const QStringList &nameFilters, QDir::Filters filters,
                               QDir::SortFlags sort)
{
    QStringList sl;
    QDir d(dirName);
    foreach (const QString &fn, d.entryList(nameFilters, filters, sort))
        sl << d.absoluteFilePath(fn);
    foreach (const QString &dn, d.entryList(QDir::Dirs | QDir::NoDotAndDotDot, sort))
        sl << entryListRecursive(dirName + "/" + dn, nameFilters, filters, sort);
    return sl;
}

/*!
\fn QStringList BDirTools::entryListRecursive(const QString &dirName, QDir::Filters filters, QDir::SortFlags sort)
\overload
Returns a list of files or directories in directory \a dirName and all it's subdirectories, matching \a filters.
\a sort determines if the list must be sorted.

The same as calling QDir::entryList() recursively, but the paths returned are absolute.

\sa entryList()
*/

QStringList entryListRecursive(const QString &dirName, QDir::Filters filters, QDir::SortFlags sort)
{
    return entryListRecursive(dirName, QStringList(), filters, sort);
}

/*!
\fn BeQt::OSType BDirTools::executablePlatform(const QString &fileName, bool considerSuffix)
Returns the supposed target operating system of file \a fileName.

Only first 4 bytes of the file are read, so this function is fast.

If \a considerSuffix is true and it is impossible to determine the target OS (for example, for script files), the
suffix of the file is taken into account.

\sa mayBeExecutable()
*/

BeQt::OSType executablePlatform(const QString &fileName, bool considerSuffix)
{
    return executablePlatform(readFile(fileName, 4), considerSuffix ? fileName : QString());
}

/*!
\fn BeQt::OSType BDirTools::executablePlatform(const QByteArray &data, const QString &fileName)
\overload
Returns the supposed target operating system of file based on it's \a data.

If \a fileName is specified, and it is impossible to determine the target OS (for example, for script files), the
suffix of the file is taken into account.

\sa mayBeExecutable()
*/

BeQt::OSType executablePlatform(const QByteArray &data, const QString &fileName)
{
    if (data.mid(0, 2) == "MZ")
        return BeQt::WindowsOS;
    else if (data.mid(0, 4) == "\xFE""\xED""\xFA""\xCE" || data.mid(0, 4) == "\xFE""\xED""\xFA""\xCF")
        return BeQt::MacOS;
    else if (data.mid(0, 4) == "\x7F""ELF")
        return BeQt::LinuxOS;
    else if (data.mid(0, 2) == "#!")
        return BeQt::UnixOS;
    if (fileName.isEmpty())
        return BeQt::UnknownOS;
    QString suffix = QFileInfo(fileName).suffix();
    if (!suffix.compare("dylib", Qt::CaseInsensitive))
        return BeQt::MacOS;
    else if (!suffix.compare("dll", Qt::CaseInsensitive) || !suffix.compare("bat", Qt::CaseInsensitive))
        return BeQt::WindowsOS;
    else if (!fileName.indexOf(QRegExp("so(\\.\\d+)?(\\.\\d+)?(\\.\\d+)?$")))
        return BeQt::UnixOS;
    return BeQt::UnknownOS;
}

/*!
\fn bool BDirTools::fileExists(const QString &fileName)
Checks if \a fileName is a valid absolute path to an existing file.

Returns true on success. Otherwise returns false.

\sa dirExists()
*/

bool fileExists(const QString &fileName)
{
    if (fileName.isEmpty())
        return false;
    QFileInfo fi(fileName);
    return fi.isAbsolute() && fi.isFile();
}

/*!
\fn QString BDirTools::findResource(const QString &subpath, ResourceLookupMode mode)
Searches for an application resource with identifier \a subpath. \a mode determines in which locations to perform the
search.

If \a mode is GlobalOnly, the resource first is searched for in shared resources location, and then in builtin
resources location.

If \a mode is UserOnly, the resource is searched for in user resources location only.

If \a mode is GlobalOnly, the resource first is searched for in user resources location, then in shared resources
location, and finally in builtin resources location.

When the resource is found, the corresponding path is returned. If the resource is not found, an empty QString is
returned.

\sa {Resources system}
*/

QString findResource(const QString &subpath, ResourceLookupMode mode)
{
    QStringList sl;
    switch (mode) {
    case GlobalOnly:
        sl << BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::SharedResource);
        sl << BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::BuiltinResource);
        break;
    case UserOnly:
        sl << BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::UserResource);
        break;
    case AllResources:
    default:
        sl << BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::SharedResource);
        sl << BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::BuiltinResource);
        sl << BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::UserResource);
        break;
    }
    return findResource(subpath, sl);
}

/*!
\fn QString BDirTools::findResource(const QString &subpath, const QStringList &locations)
\overload
Searches for a resource with identifier \a subpath subsequently in every of \a locations.

When the resource is found, the corresponding path is returned. If the resource is not found, an empty QString is
returned.
*/

QString findResource(const QString &subpath, const QStringList &locations)
{
    if (subpath.isEmpty() || locations.isEmpty())
        return QString();
    foreach (const QString &loc, locations) {
        if (!loc.isEmpty() && QFileInfo(loc + "/" + subpath).exists())
            return loc + "/" + subpath;
    }
    return QString();
}

/*!
\fn QString BDirTools::localeBasedFileName(const QString &fileName, const QLocale &loc)
Appends locale \a loc name to \a fileName (before the suffix) and then checks if the resulting file name is valid (i.e.
the corresponding file exists). If the file does not exist, repeats previous actions, but uses only language name this
time. If the file name is not valid again, repeats the actions, but appends "en" to file name. Finally, if the file
name is not valid, checks the existance of file \a fileName.

Returns as far as the resulting file name becomes valid. If no constructed file name is valid, returns an empty
QString.

\note "_" if inserted between the file path and the locale-based suffixes.

\sa localeBasedDirName()
*/

QString localeBasedFileName(const QString &fileName, const QLocale &loc)
{
    if (fileName.isEmpty())
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
    foreach (const QString &fn, sl) {
        if (QFile(fn).exists())
            return fn;
    }
    return "";
}

/*!
\fn QString BDirTools::localeBasedDirName(const QString &path, const QString &subpath, const QLocale &loc)
Searches for a subdirectory of \a path, which name is constructed of \a subpath and locale \a loc name. If the
resulting directory does not exists, the previous actions are repeated, but obly locale language name is used. If the
resulting directory is not valid again (i.e. does not exist), "en" is appended instead of locale name. Finally, if the
directory name is not valid again, existance of \a path is checked.

Returns as far as the resulting directory name becomes valid. If no constructed directory name is valid, returns an
empty QString.

\sa localeBasedFileName()
*/

QString localeBasedDirName(const QString &path, const QString &subpath, const QLocale &loc)
{
    if (path.isEmpty())
        return "";
    QString lname = loc.name().left(5);
    QDir d(path + "/" + subpath + lname);
    if (!d.exists())
        d.setPath(path + "/" + subpath + lname.left(2));
    if (!d.exists())
        d.setPath(path + "/" + subpath + "en");
    if (!d.exists())
        d.setPath(path);
    if (!d.exists())
        return "";
    return d.path();
}

/*!
\fn bool BDirTools::mayBeExecutable(const QString &fileName, bool considerSuffix)
Checks if a file \a fileName is supposed to be executable.

Only first 4 bytes of the file are read, so this function is fast.

If \a considerSuffix is true and it is impossible to determine if the file is executable (for example, for script
files), the suffix of the file is taken into account.

Returns true if the file is supposed to be executable. Otherwise returns false. If any error occurs, returns false.

\sa executablePlatform()
*/

bool mayBeExecutable(const QString &fileName, bool considerSuffix)
{
    return mayBeExecutable(readFile(fileName, 4), considerSuffix ? fileName : QString());
}

/*!
\fn bool BDirTools::mayBeExecutable(const QByteArray &data, const QString &fileName)
\overload
Checks if a file is supposed to be executable based on it's \a data.

If \a fileName is specified, and it is impossible to determine if the file is executable (for example, for script
files), the suffix of the file is taken into account.

Returns true if the file is supposed to be executable. Otherwise returns false. If any error occurs, returns false.

\sa executablePlatform()
*/

bool mayBeExecutable(const QByteArray &data, const QString &fileName)
{
    if (data.mid(0, 4) == "\x7F""ELF" || data.mid(0, 2) == "MZ" || data.mid(0, 2) == "#!")
        return true;
    if (fileName.isEmpty())
        return false;
    QString suffix = QFileInfo(fileName).suffix();
    if (!suffix.compare("dylib", Qt::CaseInsensitive) || !suffix.compare("dll", Qt::CaseInsensitive)
            || !suffix.compare("bat", Qt::CaseInsensitive))
        return true;
    if (!fileName.indexOf(QRegExp("so(\\.\\d+)?(\\.\\d+)?(\\.\\d+)?$")))
        return true;
    return false;
}

/*!
\fn bool BDirTools::mkpath(const QString &dirPath)
Creates directory \a dirPath and all directories in it's path, if they does not exist.

Returns true on success. If any error occurs, returns false.

The same as QDir::mkpath().
*/

bool mkpath(const QString &dirPath)
{
    return QDir(dirPath).mkpath(dirPath);
}

/*!
\fn bool BDirTools::moveDir(const QString &oldName, const QString &newName)
Copies directory \a oldName to new location \a newName, and then removes the old directory.

Returns true on success. If any error occurs, returns false.

The same as calling copyDir() and then rmdir().
*/

bool moveDir(const QString &oldName, const QString &newName)
{
    return renameDir(oldName, newName) || (copyDir(oldName, newName, true) && rmdir(oldName));
}

/*!
\fn QByteArray BDirTools::readFile(const QString &fileName, qint64 maxlen, bool *ok)
Reads the contents of file \a fileName. If \a maxlen is greater than 0, reads no more than \a maxlen bytes.

Returns the contents of the file.

If \a ok if non-zero pointer, it's value is set to true on success and to false on any error.

\sa readPropertiesFile(), readTextFile(), writeFile()
*/

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

/*!
\fn BProperties BDirTools::readPropertiesFile(const QString &fileName, QTextCodec *codec, bool *ok)
Reads the contents of file \a fileName and parses it with BeQt::propertiesFromString() function. \a codec is passed to
the parsing function.

Returns the resulting BProperties object.

If \a ok if non-zero pointer, it's value is set to true on success and to false on any error.

\sa readFile(), readTextFile(), writePropertiesFile(), BeQt::propertiesFromString()
*/

BProperties readPropertiesFile(const QString &fileName, QTextCodec *codec, bool *ok)
{
    bool b = false;
    QString s = readTextFile(fileName, codec, &b);
    if (!b)
        return bRet(ok, false, BProperties());
    return BeQt::propertiesFromString(s, ok);
}

/*!
\fn BProperties BDirTools::readPropertiesFile(const QString &fileName, const QString &codecName, bool *ok)
\overload
Reads the contents of file \a fileName and parses it with BeQt::propertiesFromString() function. \a codecName is passed
to the parsing function.

Returns the resulting BProperties object.

If \a ok if non-zero pointer, it's value is set to true on success and to false on any error.

\sa readFile(), readTextFile(), writePropertiesFile(), BeQt::propertiesFromString()
*/

BProperties readPropertiesFile(const QString &fileName, const QString &codecName, bool *ok)
{
    return readPropertiesFile(fileName, QTextCodec::codecForName(codecName.toLatin1()), ok);
}

/*!
\fn QString BDirTools::readTextFile(const QString &fileName, QTextCodec *codec, bool *ok)
Reads the contents of file \a fileName as a text file. \a codec is used to decode the text.

Returns the contents of the file.

If \a ok if non-zero pointer, it's value is set to true on success and to false on any error.

\sa readFile(), readPropertiesFile(), writeTextFile()
*/

QString readTextFile(const QString &fileName, QTextCodec *codec, bool *ok)
{
    if (ok)
        *ok = false;
    if (fileName.isEmpty())
        return "";
    QFile f(fileName);
    if (!f.open(QFile::ReadOnly))
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

/*!
\fn QString BDirTools::readTextFile(const QString &fileName, const QString &codecName, bool *ok)
\overload
Reads the contents of file \a fileName as a text file. A codec with name \a codecName is used to decode the text.

Returns the contents of the file.

If \a ok if non-zero pointer, it's value is set to true on success and to false on any error.

\sa readFile(), readPropertiesFile(), writeTextFile()
*/

QString readTextFile(const QString &fileName, const QString &codecName, bool *ok)
{
    return readTextFile(fileName, QTextCodec::codecForName(codecName.toLatin1()), ok);
}

/*!
\fn bool BDirTools::removeEntries(const QString &dirName, const QStringList &nameFilters, const QStringList &excluding)
Removes all files and directories in directory \a dirName. The directory itself is not removed.

If \a nameFilters is specified, only entries that match these filters are removed.

If \a excluding is specified, entries which match any of that list are not removed.

Returns true on success. If any error occurs, returns false.

\sa removeFilesInDir(), removeSubdirs()
*/

bool removeEntries(const QString &dirName, const QStringList &nameFilters, const QStringList &excluding)
{
    QDir d(dirName);
    if (!QFileInfo(dirName).isDir())
        return false;
    foreach (const QString &fn, d.entryList(nameFilters, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (excluding.contains(fn))
            continue;
        QFileInfo fi(d.absoluteFilePath(fn));
        if ((fi.isFile() && !d.remove(fn)) || (fi.isDir() && !rmdir(d.absoluteFilePath(fn))))
            return false;
    }
    return true;
}

/*!
\fn bool BDirTools::removeFilesInDir(const QString &dirName, const QStringList &nameFilters,
                                     const QStringList &excluding)
Removes all files in directory \a dirName. The directory itself and directories in it are not removed.

If \a nameFilters is specified, only files that match these filters are removed.

If \a excluding is specified, files which match any of that list are not removed.

Returns true on success. If any error occurs, returns false.

\sa removeEntries(), removeSubdirs()
*/

bool removeFilesInDir(const QString &dirName, const QStringList &nameFilters, const QStringList &excluding)
{
    QDir d(dirName);
    if (!QFileInfo(dirName).isDir())
        return false;
    foreach (const QString &fn, d.entryList(nameFilters, QDir::Files)) {
        if (excluding.contains(fn))
            continue;
        if (!d.remove(fn))
            return false;
    }
    return true;
}

/*!
\fn bool BDirTools::removeSubdirs(const QString &dirName, const QStringList &nameFilters, const QStringList &excluding)
Removes all directories in directory \a dirName recursively. The directors itself and files in it are not removed.

If \a nameFilters is specified, only directories that match these filters are removed.

If \a excluding is specified, directories which match any of that list are not removed.

Returns true on success. If any error occurs, returns false.

\sa removeEntries(), removeFilesInDir()
*/

bool removeSubdirs(const QString &dirName, const QStringList &nameFilters, const QStringList &excluding)
{
    QDir d(dirName);
    if (!QFileInfo(dirName).isDir())
        return false;
    foreach (const QString &dn, d.entryList(nameFilters, QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (excluding.contains(dn))
            continue;
        if (!rmdir(d.absoluteFilePath(dn)))
            return false;
    }
    return true;
}

/*!
\fn bool BDirTools::renameDir(const QString &oldName, const QString &newName)
Renames directory \a oldName into \a newName.

Returns true on success. If any error occurs, returns false.

The same as QDir::rename().

\sa copyDir()
*/

bool renameDir(const QString &oldName, const QString &newName)
{
    if (oldName.isEmpty() || newName.isEmpty())
        return false;
    return QDir(oldName).rename(oldName, newName);
}

/*!
\fn bool BDirTools::rmdir(const QString &dirName)
Removes all files and directories in directory \a dirName recursively, and then removes the directory itself.

Returns true on success. If any error occurs, returns false.
*/

bool rmdir(const QString &dirName)
{
    QDir d(dirName);
    QStringList dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < dirs.size(); ++i) {
        if (!rmdir(d.filePath(dirs.at(i))))
            return false;
    }
    return removeFilesInDir(dirName) && d.rmdir(dirName);
}

/*!
\fn bool BDirTools::touch(const QString &filePath)
Creates file with name \a filePath. Creates all directories in the file's path if needed.

Returns true on success or if the file already exists. If any error occurs, returns false.

If the file exists, it is not modified.
*/

bool touch(const QString &filePath)
{
    if (filePath.isEmpty())
        return false;
    QFileInfo fi(filePath);
    if (fi.exists() && fi.isFile())
        return true;
    if (!mkpath(fi.path()))
        return false;
    QFile f(filePath);
    if (!f.open(QFile::WriteOnly))
        return false;
    f.close();
    return true;
}

/*!
\fn bool BDirTools::writeFile(const QString &fileName, const QByteArray &data)
Writes \a data to file \a fileName. The file is truncated.

Returns true on success. If any error occurs, returns false.

The file is created automatically if it does not exist.

\sa appendFile(), readFile()
*/

bool writeFile(const QString &fileName, const QByteArray &data)
{
    if (fileName.isEmpty())
        return false;
    QFile f(fileName);
    if (!touch(fileName) || !f.open(QFile::WriteOnly))
        return false;
    bool b = (f.write(data) == data.size());
    f.close();
    return b;
}

/*!
\fn bool BDirTools::writePropertiesFile(const QString &fileName, const BProperties &p, QTextCodec *codec)
Writes BProperties object \a p to file \a fileName. BeQt::propertiesToString() function is used to convert \a p to a
string. \a codec is passed to the converting function.

Returns true on success. If any error occurs, returns false.

The file is created automatically if it does not exist.

\sa writeFile(), writeTextFile(), readPropertiesFile(), BeQt::propertiesToString()
*/

bool writePropertiesFile(const QString &fileName, const BProperties &p, QTextCodec *codec)
{
    return writeTextFile(fileName, BeQt::propertiesToString(p), codec);
}

/*!
\fn bool BDirTools::writePropertiesFile(const QString &fileName, const BProperties &p, const QString &codecName)
\overload
Writes BProperties object \a p to file \a fileName. BeQt::propertiesToString() function is used to convert \a p to a
string. \a codecName is passed to the converting function.

Returns true on success. If any error occurs, returns false.

The file is created automatically if it does not exist.

\sa writeFile(), writeTextFile(), readPropertiesFile(), BeQt::propertiesToString()
*/

bool writePropertiesFile(const QString &fileName, const BProperties &p, const QString &codecName)
{
    return writePropertiesFile(fileName, p, QTextCodec::codecForName(codecName.toLatin1()));
}

/*!
\fn bool BDirTools::writeTextFile(const QString &fileName, const QString &text, QTextCodec *codec)
Writes \a text to file \a fileName. The file is truncated. \a codec is used to encode the text.

Returns true on success. If any error occurs, returns false.

The file is created automatically if it does not exist.

\sa appendTextFile(), readTextFile(), writeFile()
*/

bool writeTextFile(const QString &fileName, const QString &text, QTextCodec *codec)
{
    if (fileName.isEmpty())
        return false;
    QFile f(fileName);
    if (!touch(fileName) || !f.open(QFile::WriteOnly))
        return false;
    QTextStream out(&f);
    if (codec)
        out.setCodec(codec);
    out << text;
    bool b = (out.status() != QTextStream::WriteFailed);
    f.close();
    return b;
}

/*!
\fn bool BDirTools::writeTextFile(const QString &fileName, const QString &text, const QString &codecName)
Writes \a text to file \a fileName. The file is truncated. A codec with name \a codecName is used to encode the text.

Returns true on success. If any error occurs, returns false.

The file is created automatically if it does not exist.

\sa appendTextFile(), readTextFile(), writeFile()
*/

bool writeTextFile(const QString &fileName, const QString &text, const QString &codecName)
{
    return writeTextFile(fileName, text, QTextCodec::codecForName(codecName.toLatin1()));
}

}
