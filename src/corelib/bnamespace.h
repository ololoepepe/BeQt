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

#ifndef BNAMESPACE_H
#define BNAMESPACE_H

class QObject;
class QProcess;
class QRect;
class QRectF;
class QString;
class QTextCodec;

#include "bglobal.h"

#include <QDataStream>
#include <QList>
#include <QPair>
#include <QStringList>
#include <QSysInfo>

/*============================================================================
================================ BeQt ========================================
============================================================================*/

namespace BeQt
{

enum CodecsGroup
{
    //Keep the order
    InvalidGroup = 0,
    UnicodeGroup,
    EastEuropeanGroup,
    WestEuropeanGroup,
    EastAsianGroup,
    SouthEastSouthWestAsianGroup,
    MiddleEastGroup
};

enum FileSizeFormat
{
    //Keep the order
    BytesFormat = 1,
    KilobytesFormat,
    MegabytesFormat,
    GigabytesFormat
};

enum LineFeed
{
    DefaultLineFeed = 0,
    ClassicMacLineFeed,
    UnixLineFeed,
    WindowsLineFeed
};

enum OSType
{
    //Keep the order
    UnknownOS = 0x00,       //0 0 0 0 0 0 0 0
    UnixOS = 0x01,          //0 0 0 0 0 0 0 1
    WindowsOS = 0x02,       //0 0 0 0 0 0 1 0
    LinuxOS = UnixOS | 0x04,//0 0 0 0 0 1 0 1
    MacOS = UnixOS | 0x08   //0 0 0 0 1 0 0 1
};

enum ProcessorArchitecture
{
    UnknownArchitecture = 0,
    AlphaArchitecture,
    Amd64Architecture,
    ArmArchitecture,
    Arm64Architecture,
    BlackfinArchitecture,
    ConvexArchitecture,
    EpiphanyArchitecture,
    HpPaRiscArchitecture,
    IntelX86Architecture,
    IntelItaniumArchitecture,
    Motorola68kAArchitecture,
    MipsArchitecture,
    PowerPcArchitecture,
    Pyramid9810Architecture,
    Rs6000Architecture,
    SparcArchitecture,
    SuperHArchitecture,
    SystemZArchitecture,
    Tms320Architecture,
    Tms470Architecture
};

enum TabWidth
{
    TabWidth2 = 2,
    TabWidth4 = 4,
    TabWidth8 = 8
};

typedef QPair<const QObject *, const char *> Until;
typedef QPair<const QObject *, const char *> Target;

const QDataStream::Version DataStreamVersion = QDataStream::Qt_4_8;

//Keep the order
const int Second = 1000;
const int Minute = 60 * Second;
const int Hour = 60 * Minute;

//Keep the order
const int Kilobyte = 1024;
const int Megabyte = 1024 * Kilobyte;
const int Gigabyte = 1024 * Megabyte;

//Kep the order
const int ContorlShiftModifier = ((int) Qt::ControlModifier | (int) Qt::ShiftModifier);
const int KeypadAltModifier = ((int) Qt::KeypadModifier | (int) Qt::AltModifier);
const int KeypadControlModifier = ((int) Qt::KeypadModifier | (int) Qt::ControlModifier);
const int KeypadShiftModifier = ((int) Qt::KeypadModifier | (int) Qt::ShiftModifier);
const int KeypadControlAltModifier = (KeypadAltModifier | (int) Qt::ControlModifier);
const int KeypadControlShiftModifier = ((int) Qt::KeypadModifier | ContorlShiftModifier);

B_CORE_EXPORT QList<OSType> allOSTypes(bool includeUnknown = true, bool includeMeta = false);
B_CORE_EXPORT QList<ProcessorArchitecture> allProcessorArchitectures(bool includeUnknown = true);
B_CORE_EXPORT QList<TabWidth> allTabWidths();
B_CORE_EXPORT int area(const QRect &r);
B_CORE_EXPORT qreal area(const QRectF &r);
B_CORE_EXPORT QTextCodec *codec(const QString &cn);
B_CORE_EXPORT QTextCodec *codec(const QByteArray &cn);
B_CORE_EXPORT QString codecName(QTextCodec *codec);
B_CORE_EXPORT QString codecName(const QByteArray &cn);
B_CORE_EXPORT QStringList codecNamesForGroup(CodecsGroup group);
B_CORE_EXPORT QList<QTextCodec *> codecsForGroup(CodecsGroup group);
B_CORE_EXPORT CodecsGroup codecsGroupFromInt(int cg);
B_CORE_EXPORT QString codecsGroupName(CodecsGroup group);
B_CORE_EXPORT QList<CodecsGroup> codecsGroups();
B_CORE_EXPORT void deleteLaterUnowned(QObject *object, QObject *possibleOwner = 0);
B_CORE_EXPORT QVariant deserialize(const QByteArray &data, QDataStream::Version version = DataStreamVersion);
B_CORE_EXPORT void deleteUnowned(QObject *object, QObject *possibleOwner = 0);
B_CORE_EXPORT int execProcess(const QString &command, const QStringList &arguments,
                              int startTimeout, int finishTimeout, QString *output = 0, QTextCodec *codec = 0);
B_CORE_EXPORT int execProcess(const QString &workingDir, const QString &command, const QStringList &arguments,
                              int startTimeout, int finishTimeout, QString *output = 0, QTextCodec *codec = 0);
B_CORE_EXPORT QString fileSizeToString(qint64 size, FileSizeFormat format = BytesFormat, quint8 precision = 1);
B_CORE_EXPORT QString fileSizeToStringNoTr(qint64 size, FileSizeFormat format = BytesFormat, quint8 precision = 1);
B_CORE_EXPORT QString fullCodecName(QTextCodec *codec);
B_CORE_EXPORT QString fullCodecName(const QString &codecName);
B_CORE_EXPORT bool isCodecSupported(QTextCodec *codec);
B_CORE_EXPORT bool isCodecSupported(const QString &codecName);
#if defined(Q_OS_LINUX)
B_CORE_EXPORT QString linuxVersion();
#endif
#if defined(Q_OS_MAC)
B_CORE_EXPORT QString macVersionToString(QSysInfo::MacVersion version);
B_CORE_EXPORT QString macVersion();
#endif
B_CORE_EXPORT void msleep(unsigned long msecs);
B_CORE_EXPORT OSType osType();
B_CORE_EXPORT QString osTypeToString(OSType t);
B_CORE_EXPORT QString osVersion();
B_CORE_EXPORT LineFeed platformLineFeed();
B_CORE_EXPORT ProcessorArchitecture processorArchitecture();
B_CORE_EXPORT QString processorArchitectureToString(ProcessorArchitecture arch);
B_CORE_EXPORT QByteArray serialize(const QVariant &variant, QDataStream::Version version = DataStreamVersion);
B_CORE_EXPORT void sleep(unsigned long secs);
B_CORE_EXPORT void startProcess(QProcess *proc, const QString &command, const QStringList &arguments = QStringList());
B_CORE_EXPORT bool startProcessDetached(const QString &command, const QStringList &arguments = QStringList());
B_CORE_EXPORT bool startProcessDetached(const QString &command, const QString &workingDir,
                                        const QStringList &arguments = QStringList());
B_CORE_EXPORT QList<QTextCodec *> supportedCodecs();
B_CORE_EXPORT QStringList supportedCodecsNames();
B_CORE_EXPORT TabWidth tabWidthFromInt(int tw);
B_CORE_EXPORT void takeOwnership(QObject *object, QObject *newOwner);
B_CORE_EXPORT Target target(const QObject *object, const char *method);
B_CORE_EXPORT QString translate(const char *context, const char *sourceText, const char *disambiguation = 0,
                                int n = -1);
B_CORE_EXPORT Until until(const QObject *object, const char *signal);
B_CORE_EXPORT void usleep(unsigned long usecs);
B_CORE_EXPORT void waitNonBlocking(int msecs);
B_CORE_EXPORT void waitNonBlocking(const QObject *sender, const char *signal, int msecs = -1);
B_CORE_EXPORT void waitNonBlocking(const QObject *sender1, const char *signal1, const QObject *sender2,
                                   const char *signal2, int msecs = -1);
B_CORE_EXPORT void waitNonBlocking(const QList<Until> &list, int msecs = -1);
#if defined(Q_OS_WIN)
B_CORE_EXPORT QString windowsVersionToString(QSysInfo::WinVersion version);
B_CORE_EXPORT QString windowsVersion();
#endif

}

#endif // BNAMESPACE_H
