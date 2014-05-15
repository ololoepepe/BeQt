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

#ifndef BDIRTOOLS_H
#define BDIRTOOLS_H

class QTextCodec;
class QByteArray;

#include "bglobal.h"
#include "bcoreapplication.h"
#include "bnamespace.h"

#include <QString>
#include <QStringList>
#include <QList>
#include <QDir>

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

B_CORE_EXPORT bool fileExists(const QString &fileName);
B_CORE_EXPORT bool mayBeExecutable(const QString &fileName, bool considerSuffix = true);
B_CORE_EXPORT bool mayBeExecutable(const QByteArray &data, const QString &fileName = QString());
B_CORE_EXPORT BeQt::OSType executablePlatform(const QString &fileName, bool considerSuffix = true);
B_CORE_EXPORT BeQt::OSType executablePlatform(const QByteArray &data, const QString &fileName = QString());
B_CORE_EXPORT bool dirExists(const QString &dirName);
B_CORE_EXPORT bool touch(const QString &filePath);
B_CORE_EXPORT bool mkpath(const QString &dirPath);
B_CORE_EXPORT bool rmdir(const QString &dirName);
B_CORE_EXPORT bool removeFilesInDir(const QString &dirName, const QStringList &nameFilters = QStringList(),
                                    const QStringList &excluding = QStringList());
B_CORE_EXPORT bool removeSubdirs(const QString &dirName, const QStringList &nameFilters = QStringList(),
                                 const QStringList &excluding = QStringList());
B_CORE_EXPORT bool removeEntries(const QString &dirName, const QStringList &nameFilters = QStringList(),
                                 const QStringList &excluding = QStringList());
B_CORE_EXPORT bool copyDir(const QString &dirName, const QString &newDirName, bool recursively = false);
B_CORE_EXPORT bool renameDir(const QString &oldName, const QString &newName);
B_CORE_EXPORT bool moveDir(const QString &oldName, const QString &newName);
B_CORE_EXPORT QStringList entryList(const QString &dirName, const QStringList &nameFilters,
                                    QDir::Filters filters = QDir::NoFilter, QDir::SortFlags sort = QDir::NoSort);
B_CORE_EXPORT QStringList entryList(const QString &dirName, QDir::Filters filters = QDir::NoFilter,
                                    QDir::SortFlags sort = QDir::NoSort);
B_CORE_EXPORT QString localeBasedFileName( const QString &fileName, const QLocale &loc = BCoreApplication::locale() );
B_CORE_EXPORT QString localeBasedDirName( const QString &path, const QString &subpath = QString(),
                                          const QLocale &loc = BCoreApplication::locale() );
B_CORE_EXPORT QByteArray readFile(const QString &fileName, qint64 maxlen = -1, bool *ok = 0);
B_CORE_EXPORT bool writeFile(const QString &fileName, const QByteArray &data);
B_CORE_EXPORT bool appendFile(const QString &fileName, const QByteArray &data);
B_CORE_EXPORT QString readTextFile(const QString &fileName, QTextCodec *codec = 0, bool *ok = 0);
B_CORE_EXPORT QString readTextFile(const QString &fileName, const QString &codecName, bool *ok = 0);
B_CORE_EXPORT bool writeTextFile(const QString &fileName, const QString &text, QTextCodec *codec = 0);
B_CORE_EXPORT bool writeTextFile(const QString &fileName, const QString &text, const QString &codecName);
B_CORE_EXPORT bool appendTextFile(const QString &fileName, const QString &text, QTextCodec *codec = 0);
B_CORE_EXPORT bool appendTextFile(const QString &fileName, const QString &text, const QString &codecName);
B_CORE_EXPORT QString findResource(const QString &subpath, ResourceLookupMode mode = AllResources);
B_CORE_EXPORT QString findResource(const QString &subpath, const QStringList &locations);
B_CORE_EXPORT bool createUserLocation(BCoreApplication::Location loc);
B_CORE_EXPORT bool createUserLocation(const QString &subdir);
B_CORE_EXPORT bool createUserLocations(const QStringList &subdirs);

}

#endif // BDIRTOOLS_H
