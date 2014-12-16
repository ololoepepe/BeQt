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

#include "bnamespace.h"

#include "bdirtools.h"
#include "bproperties.h"
#include "btexttools.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <QList>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QProcess>
#include <QRect>
#include <QRectF>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QSysInfo>
#include <QTextCodec>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <QVariant>

/*============================================================================
================================ ThreadHack ==================================
============================================================================*/

class ThreadHack : public QThread
{
private:
    explicit ThreadHack();
public:
    static void msleepHack(unsigned long msecs);
    static void sleepHack(unsigned long secs);
    static void usleepHack(unsigned long usecs);
private:
    Q_DISABLE_COPY(ThreadHack)
};

/*============================================================================
================================ Global static functions =====================
============================================================================*/

static QString fileSizeToStringInternal(qint64 sz, BeQt::FileSizeFormat f, quint8 p)
{
    if (sz < 0)
        sz = 0;
    int d = 1;
    switch (f) {
    case BeQt::GigabytesFormat:
        d = BeQt::Gigabyte;
        break;
    case BeQt::MegabytesFormat:
        d = BeQt::Megabyte;
        break;
    case BeQt::KilobytesFormat:
        d = BeQt::Kilobyte;
        break;
    case BeQt::BytesFormat:
    default:
        break;
    }
    QString s = QString::number(sz / d);
    if (1 == d || !p)
        return s;
    if (p > 3)
        p = 3;
    QString ss = QString::number(double(sz % d) / double(d)).mid(2, p);
    while (!ss.isEmpty() && ss.at(ss.length() - 1) == '0')
        ss.remove(ss.length() - 1, 1);
    if (ss.isEmpty() && (sz % d))
        ss = "1";
    return s + (!ss.isEmpty() ? ("." + ss) : QString());
}

static void removeUnsupportedCodecNames(QStringList &list)
{
    foreach (int i, bRangeR(list.size() - 1, 0)) {
        if (!QTextCodec::codecForName(list.at(i).toLatin1()))
            list.removeAt(i);
    }
}

static QStringList supportedUnicodeCodecNames()
{
    init_once(QStringList, list, QStringList()) {
        list << "UTF-8" << "UTF-16";
        removeUnsupportedCodecNames(list);
    }
    return list;
}

static QStringList supportedEastEuropeanCodecNames()
{
    init_once(QStringList, list, QStringList()) {
        list << "ISO 8859-13" << "ISO 8859-4" << "Windows-1257" << "ISO 8859-5" << "KOI8-R" << "Windows-1251"
             << "IBM 866" << "KOI8-U" << "ISO 8859-16" << "ISO 8859-2" << "Windows-1250";
        removeUnsupportedCodecNames(list);
    }
    return list;
}

static QStringList supportedWestEuropeanCodecNames()
{
    init_once(QStringList, list, QStringList()) {
        list << "ISO 8859-7" << "Windows-1253" << "IBM 850" << "ISO 8859-1" << "ISO 8859-15" << "Apple Roman"
             << "Windows-1252" << "ISO 8859-14" << "ISO 8859-10" << "ISO 8859-3";
        removeUnsupportedCodecNames(list);
    }
    return list;
}

static QStringList supportedEastAsianCodecNames()
{
    init_once(QStringList, list, QStringList()) {
        list << "Windows-1258" << "Big5" << "Big5-HKSCS" << "GB18030-0" << "EUC-KR" << "JOHAB" << "EUC-JP"
             << "ISO 2022-JP" << "Shift-JIS";
        removeUnsupportedCodecNames(list);
    }
    return list;
}

static QStringList supportedSouthEastSouthWestAsianCodecNames()
{
    init_once(QStringList, list, QStringList()) {
        list << "TIS-620" << "ISO 8859-9" << "Windows-1254";
        removeUnsupportedCodecNames(list);
    }
    return list;
}

static QStringList supportedMiddleEastCodecNames()
{
    init_once(QStringList, list, QStringList()) {
        list << "ISO 8859-6" << "Windows-1256" << "Windows-1255" << "ISO 8859-8";
        removeUnsupportedCodecNames(list);
    }
    return list;
}

static QMap<QTextCodec *, QString> supportedCodecsMap()
{
    typedef QMap<QTextCodec *, QString> CodecMap;
    init_once(CodecMap, m, CodecMap()) {
        foreach (const QString &cn, BeQt::supportedCodecsNames())
            m.insert(QTextCodec::codecForName(cn.toLatin1()), cn);
    }
    return m;
}

static QString codecDescriptiveName(const QString &codecName)
{
    if (supportedUnicodeCodecNames().contains(codecName)) {
        return BeQt::translate("BeQt", "Unicode", "codec descriptiveName");
    } else if (supportedEastEuropeanCodecNames().contains(codecName)) {
        if ((QStringList() << "ISO 8859-13" << "ISO 8859-4" << "Windows-1257").contains(codecName,
                                                                                        Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Baltic", "codec descriptiveName");
        else if ((QStringList() << "ISO 8859-5" << "KOI8-R"
                  << "Windows-1251" << "IBM 866").contains(codecName, Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Cyrillic", "codec descriptiveName");
        else if (!codecName.compare("KOI8-U", Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Cyrillic (Ukrainian)", "codec descriptiveName");
        else if (!codecName.compare("ISO 8859-16", Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Romanic", "codec descriptiveName");
        else if ((QStringList() << "ISO 8859-2" << "Windows-1250").contains(codecName, Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Central European", "codec descriptiveName");
    } else if (supportedWestEuropeanCodecNames().contains(codecName)) {
        if ((QStringList() << "ISO 8859-7" << "Windows-1253").contains(codecName, Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Greek", "codec descriptiveName");
        else if ((QStringList() << "IBM 850" << "ISO 8859-1" << "ISO 8859-15"
                  << "Apple Roman" << "Windows-1252").contains(codecName, Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Western", "codec descriptiveName");
        else if (!codecName.compare("ISO 8859-14", Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Celtic", "codec descriptiveName");
        else if (!codecName.compare("ISO 8859-10", Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Nordic", "codec descriptiveName");
        else if (!codecName.compare("ISO 8859-3", Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "South European", "codec descriptiveName");
    } else if (supportedEastAsianCodecNames().contains(codecName)) {
        if (!codecName.compare("Windows-1258", Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Vietnamese", "codec descriptiveName");
        else if ((QStringList() << "Big5" << "Big5-HKSCS").contains(codecName, Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Traditional Chinese", "codec descriptiveName");
        else if (!codecName.compare("GB18030-0", Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Simplified Chinese", "codec descriptiveName");
        else if ((QStringList() << "EUC-KR" << "JOHAB").contains(codecName, Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Korean", "codec descriptiveName");
        else if ((QStringList() << "EUC-JP" << "ISO 2022-JP" << "Shift-JIS").contains(codecName, Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Japanese", "codec descriptiveName");
    } else if (supportedSouthEastSouthWestAsianCodecNames().contains(codecName)) {
        if (!codecName.compare("TIS-620", Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Thai", "codec descriptiveName");
        else if ((QStringList() << "ISO 8859-9" << "Windows-1254").contains(codecName, Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Turkish", "codec descriptiveName");
    } else if (supportedMiddleEastCodecNames().contains(codecName)) {
        if ((QStringList() << "ISO 8859-6" << "Windows-1256").contains(codecName, Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Arabic", "codec descriptiveName");
        else if (!codecName.compare("Windows-1255", Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Hebrew", "codec descriptiveName");
        else if (!codecName.compare("ISO 8859-8", Qt::CaseInsensitive))
            return BeQt::translate("BeQt", "Hebrew (visual)", "codec descriptiveName");
    }
    return "";
}

/*============================================================================
================================ ThreadHack ==================================
============================================================================*/

/*============================== Static public methods =====================*/

void ThreadHack::msleepHack(unsigned long msecs)
{
    msleep(msecs);
}

void ThreadHack::sleepHack(unsigned long secs)
{
    sleep(secs);
}

void ThreadHack::usleepHack(unsigned long usecs)
{
    usleep(usecs);
}

/*============================================================================
================================ BeQt ========================================
============================================================================*/

/*!
\namespace BeQt
\inmodule BeQtCore

\brief The BeQt namespace contains miscellaneous identifiers and functions used throughout the BeQt library.
*/

/*!
\enum BeQt::CodecsGroup

This type is used to specify groups of codecs used to encode/decode text.

\value InvalidGroup
\value UnicodeGroup
\value EastEuropeanGroup
\value WestEuropeanGroup
\value EastAsianGroup
\value SouthEastSouthWestAsianGroup
\value MiddleEastGroup
*/

/*!
\enum BeQt::FileSizeFormat

This type is used to specify user-readable file size representation format.

\value BytesFormat
\value KilobytesFormat
\value MegabytesFormat
\value GigabytesFormat
*/

/*!
\enum BeQt::LineFeed

This type is used to specify line feed.

\value DefaultLineFeed
\value ClassicMacLineFeed
\value UnixLineFeed
\value WindowsLineFeed
*/

/*!
\enum BeQt::OSType

This type is used to specify an operating system.

\value UnknownOS
\value UnixOS
\value WindowsOS
\value LinuxOS
\value MacOS
*/

/*!
\enum BeQt::ProcessorArchitecture

This type is used to specify a processor architecture.

\value UnknownArchitecture
\value AlphaArchitecture
\value Amd64Architecture
\value ArmArchitecture
\value Arm64Architecture
\value BlackfinArchitecture
\value ConvexArchitecture
\value EpiphanyArchitecture
\value HpPaRiscArchitecture
\value IntelX86Architecture
\value IntelItaniumArchitecture
\value Motorola68kAArchitecture
\value MipsArchitecture
\value PowerPcArchitecture
\value Pyramid9810Architecture
\value Rs6000Architecture
\value SparcArchitecture
\value SuperHArchitecture
\value SystemZArchitecture
\value Tms320Architecture
\value Tms470Architecture
*/

/*!
\enum BeQt::TabWidth

This type is used to specify tabulation equivalent in spaces.

\value TabWidth2
\value TabWidth4
\value TabWidth8
*/

/*!
\typedef BeQt::Target

The BeQt::Target typedef provides a pair of a const QObject pointer and a const char pointer for BeQt.
*/

/*!
\typedef BeQt::Until

The BeQt::Target typedef provides a pair of a const QObject pointer and a const char pointer for BeQt.

\sa BeQt::Target
*/

/*!
\variable BeQt::DataStreamVersion
\brief The data stream version that is used to encode/decode QVariant variables to/from QByteArray data.
*/

/*!
\variable BeQt::Second
\brief Equivalent of 1000 (milliseconds).
*/

/*!
\variable BeQt::Minute
\brief Equivalent of 60000 (milliseconds).
*/

/*!
\variable BeQt::Hour
\brief Equivalent of 3600000 (milliseconds).
*/

/*!
\variable BeQt::Kilobyte
\brief Equivalent of 1024 (bytes).
*/

/*!
\variable BeQt::Megabyte
\brief Equivalent of 1048576 (bytes).
*/

/*!
\variable BeQt::Gigabyte
\brief Equivalent of 1073741824 (bytes).
*/

/*!
\variable BeQt::ContorlShiftModifier
\brief A Control and a Shift keys on the keyboard are pressed.
*/

/*!
\variable BeQt::KeypadAltModifier
\brief A keypad button and an Alt key on the keyboard are pressed.
*/

/*!
\variable BeQt::KeypadControlModifier
\brief A keypad button and a Control key on the keyboard are pressed.
*/

/*!
\variable BeQt::KeypadShiftModifier
\brief A keypad button and a Shift key on the keyboard are pressed.
*/

/*!
\variable BeQt::KeypadControlAltModifier
\brief A keypad button, a Control and an Alt keys on the keyboard are pressed.
*/

/*!
\variable BeQt::KeypadControlShiftModifier
\brief A keypad button, a Control and a Shift keys on the keyboard are pressed.
*/

namespace BeQt
{

/*!
\fn QList<OSType> BeQt::allOSTypes(bool includeUnknown, bool includeMeta)
Returns a list of all possible OS types.

If \a includeUnknown is true, UnknownOS is included, otherwise it is not included.

If \a includeMeta is true, UnixOS is included, otherwise it is not included.

\sa osType()
*/

QList<OSType> allOSTypes(bool includeUnknown, bool includeMeta)
{
    QList<OSType> list = QList<OSType>() << WindowsOS << LinuxOS << MacOS;
    if (includeMeta)
        list.prepend(UnixOS);
    if (includeUnknown)
        list.prepend(UnknownOS);
    return list;
}

/*!
\fn QList<ProcessorArchitecture> BeQt::allProcessorArchitectures(bool includeUnknown)
Returns a list of all possible processor architectures.

If \a includeUnknown is true, UnknownArchitecture is included, otherwise it is not included.

\sa processorArchitecture()
*/

QList<ProcessorArchitecture> allProcessorArchitectures(bool includeUnknown)
{
    QList<ProcessorArchitecture> list = QList<ProcessorArchitecture>() << AlphaArchitecture << Amd64Architecture
        << ArmArchitecture << Arm64Architecture << BlackfinArchitecture << ConvexArchitecture << EpiphanyArchitecture
        << HpPaRiscArchitecture << IntelX86Architecture << IntelItaniumArchitecture << Motorola68kAArchitecture
        << MipsArchitecture << PowerPcArchitecture << Pyramid9810Architecture << Rs6000Architecture
        << SparcArchitecture << SuperHArchitecture << SystemZArchitecture << Tms320Architecture << Tms470Architecture;
    if (includeUnknown)
        list.prepend(UnknownArchitecture);
    return list;
}

/*!
\fn QList<BeQt::TabWidth> BeQt::allTabWidths()
Returns a list of all possible tab widths.
*/

QList<BeQt::TabWidth> allTabWidths()
{
    return QList<BeQt::TabWidth>() << TabWidth2 << TabWidth4 << TabWidth8;
}

/*!
\fn int BeQt::area(const QRect &r)
Calculates the area of a given QRect \a r. Returns the area.
*/

int area(const QRect &r)
{
    return r.size().width() * r.size().height();
}

/*!
\fn qreal BeQt::area(const QRectF &r)
\overload
Calculates the area of a given QRect \a r. Returns the area.

\sa area()
*/

qreal area(const QRectF &r)
{
    return r.size().width() * r.size().height();
}

/*!
\fn QTextCodec *BeQt::codec(const QString &cn)
Raturns a pointer to a QTextCodec with the name \a cn. If there is no appropriate codec, 0 is returned.

\sa codecName()
*/

QTextCodec *codec(const QString &cn)
{
    return codec(cn.toLatin1());
}

/*!
\fn QTextCodec *BeQt::codec(const QByteArray &cn)
\overload
Raturns a pointer to a QTextCodec with the name \a cn. If there is no appropriate codec, 0 is returned.

\sa codec(), codecName()
*/

QTextCodec *codec(const QByteArray &cn)
{
    QTextCodec *c = !cn.isEmpty() ? QTextCodec::codecForName(cn) : 0;
    return (c && supportedCodecsMap().contains(c)) ? c : 0;
}

/*!
\fn QString BeQt::codecName(QTextCodec *codec)
Raturns the name of a given \a codec. If \a codec is 0, an empty QString is returned.

\sa codec()
*/

QString codecName(QTextCodec *codec)
{
    return supportedCodecsMap().value(codec);
}

/*!
\fn QString BeQt::codecName(const QByteArray &cn)
\overload
Returns the name of a codec \a cn as a QString. If \a cn is not a valid codec name, an empty QString is returned.

\sa codec(), codecName()
*/

QString codecName(const QByteArray &cn)
{
    return codecName(codec(cn));
}

/*!
\fn QStringList BeQt::codecNamesForGroup(CodecsGroup group)
Returns a list of all supported codec names in a given \a group.

\sa codecsForGroup()
*/

QStringList codecNamesForGroup(CodecsGroup group)
{
    switch (group) {
    case UnicodeGroup:
        return supportedUnicodeCodecNames();
    case EastEuropeanGroup:
        return supportedEastEuropeanCodecNames();
    case WestEuropeanGroup:
        return supportedWestEuropeanCodecNames();
    case EastAsianGroup:
        return supportedEastAsianCodecNames();
    case SouthEastSouthWestAsianGroup:
        return supportedSouthEastSouthWestAsianCodecNames();
    case MiddleEastGroup:
        return supportedMiddleEastCodecNames();
    default:
        return QStringList();
    }
}

/*!
\fn QList<QTextCodec *> BeQt::codecsForGroup(CodecsGroup group)
Returns a list of pointers to all supported codecs in a given \a group.

\sa codecNamesForGroup()
*/

QList<QTextCodec *> codecsForGroup(CodecsGroup group)
{
    QList<QTextCodec *> list;
    foreach (const QString &name, codecNamesForGroup(group))
        list << codec(name);
    return list;
}

/*!
\fn CodecsGroup BeQt::codecsGroupFromInt(int cg)
Casts \a cg to a CodecsGroup. Returns the resulting CodecsGroup. If the value of \a cg does not match any CodecsGroup
member, InvalidGroup is returned.
*/

CodecsGroup codecsGroupFromInt(int cg)
{
    static const QList<int> groups = bRangeD(InvalidGroup, MiddleEastGroup);
    return groups.contains(cg) ? static_cast<CodecsGroup>(cg) : InvalidGroup;
}

/*!
\fn QString BeQt::codecsGroupName(CodecsGroup group)
Returns a localised (translated) name of a given \a group as a QString. If an invalid group is specified, an empty
string is returned.
*/

QString codecsGroupName(CodecsGroup group)
{
    switch (group) {
    case UnicodeGroup:
        return translate("BeQt", "Unicode", "codecs group name");
    case EastEuropeanGroup:
        return translate("BeQt", "Eastern Europe", "codecs group name");
    case WestEuropeanGroup:
        return translate("BeQt", "Western Europe", "codecs group name");
    case EastAsianGroup:
        return translate("BeQt", "East Asia", "codecs group name");
    case SouthEastSouthWestAsianGroup:
        return translate("BeQt", "South-East and South-West Asia", "codecs group name");
    case MiddleEastGroup:
        return translate("BeQt", "Middle East", "codecs group name");
    default:
        return "";
    }
}

/*!
\fn QList<CodecsGroup> BeQt::codecsGroups()
Returns a list of all codecs groups.
*/

QList<CodecsGroup> codecsGroups()
{
    static QList<CodecsGroup> list = QList<CodecsGroup>() << UnicodeGroup << EastEuropeanGroup << WestEuropeanGroup
        << EastAsianGroup << SouthEastSouthWestAsianGroup << MiddleEastGroup;
    return list;
}

/*!
\fn void BeQt::deleteLaterUnowned(QObject *object, QObject *possibleOwner)
Calls deleteLater method of an \a object if it has no parent, or if it's parent equals \a possibleOwner. Otherwise does
nothing.

If \a object is 0, nothing is done.
*/

void deleteLaterUnowned(QObject *object, QObject *possibleOwner)
{
    if (!object)
        return;
    if (!object->parent() || (object->parent() == possibleOwner))
        object->deleteLater();
}

/*!
\fn QVariant BeQt::deserialize(const QByteArray &data, QDataStream::Version version)
Deserializes \a data to a QVariant using QDataStream. \a version is used as a QDataStream version.

Returns the resulting QVariant.
*/

QVariant deserialize(const QByteArray &data, QDataStream::Version version)
{
    QDataStream in(data);
    in.setVersion(version);
    QVariant v;
    in >> v;
    return v;
}

/*!
\fn void BeQt::deleteUnowned(QObject *object, QObject *possibleOwner)
Deletes an \a object if it has no parent, or if it's parent equals \a possibleOwner. Otherwise does nothing.

If \a object is 0, nothing is done.

\note Deleting is performed using the delete operator.
*/

void deleteUnowned(QObject *object, QObject *possibleOwner)
{
    if (!object)
        return;
    if (!object->parent() || (object->parent() == possibleOwner))
        delete object;
}

/*!
\fn int BeQt::execProcess(const QString &command, const QStringList &arguments, int startTimeout, int finishTimeout,
                          QString *output, QTextCodec *codec)
Executes \a command with \a arguments. If \a command is an absolute path to an executable, the working directory is set
to the directory where the file is located.

This function waits \a startTimeout milliseconds for the process to start, and \a finishTimeout milliseconds for it to
finish. The operation is synchronous, so it may block the GUI.

If \a output is a non-null pointer, the processes' output is written to it. \a codec is used to read the output.

This function uses QProcess and QTextStream internally.

\sa startProcess()
*/

int execProcess(const QString &command, const QStringList &arguments,
                int startTimeout, int finishTimeout, QString *output, QTextCodec *codec)
{
    QFileInfo fi(command);
    return execProcess(fi.isAbsolute() ? fi.path() : QString(), command, arguments, startTimeout, finishTimeout,
                       output, codec);
}

/*!
\fn int BeQt::execProcess(const QString &workingDir, const QString &command, const QStringList &arguments,
                          int startTimeout, int finishTimeout, QString *output, QTextCodec *codec)
\overload
Executes \a command with \a arguments. The working directory is set to \a workingDir if it is not empty.

This function waits \a startTimeout milliseconds for the process to start, and \a finishTimeout milliseconds for it to
finish. The operation is synchronous, so it may block the GUI.

If \a output is a non-null pointer, the processes' output is written to it. \a codec is used to read the output.

This function uses QProcess and QTextStream internally.

\sa startProcess()
*/

int execProcess(const QString &workingDir, const QString &command, const QStringList &arguments,
                int startTimeout, int finishTimeout, QString *output, QTextCodec *codec)
{
    if (command.isEmpty())
        return 0;
    QProcess proc;
    if (!workingDir.isEmpty())
        proc.setWorkingDirectory(workingDir);
    proc.setProcessChannelMode(QProcess::MergedChannels);
    startProcess(&proc, command, arguments);
    if (!proc.waitForStarted(startTimeout) || !proc.waitForFinished(finishTimeout)) {
        proc.kill();
        return -2;
    }
    QTextStream in(&proc);
    if (codec)
        in.setCodec(codec);
    if (proc.exitStatus() != QProcess::NormalExit)
        return -1;
    bSet(output, in.readAll());
    BeQt::waitNonBlocking(100);
    return proc.exitCode();
}

/*!
\fn QString BeQt::fileSizeToString(qint64 size, FileSizeFormat format, quint8 precision)
Converts file size \a size to a human-readable localized string using \a format with \a precision.
*/

QString fileSizeToString(qint64 size, FileSizeFormat format, quint8 precision)
{
    QString s;
    switch (format) {
    case MegabytesFormat:
        s = translate("BeQt", "Megabyte(s)");
        break;
    case KilobytesFormat:
        s = translate("BeQt", "Kilobyte(s)");
        break;
    case BytesFormat:
    default:
        s = translate("BeQt", "Byte(s)");
        break;
    }
    return fileSizeToStringInternal(size, format, precision) + " " + s;
}

/*!
\fn QString BeQt::fileSizeToStringNoTr(qint64 size, FileSizeFormat format, quint8 precision)
\overload
Converts file size \a size to a human-readable bot not localized string using \a format with \a precision.
*/

QString fileSizeToStringNoTr(qint64 size, FileSizeFormat format, quint8 precision)
{
    QString s;
    switch (format) {
    case MegabytesFormat:
        s = "Megabyte(s)";
        break;
    case KilobytesFormat:
        s = "Kilobyte(s)";
        break;
    case BytesFormat:
    default:
        s = "Byte(s)";
        break;
    }
    return fileSizeToStringInternal(size, format, precision) + " " + s;
}

/*!
\fn QString BeQt::fullCodecName(QTextCodec *codec)
Returns the full codec name of a \a codec. The full name contains of the codec description (localized string) and the
identifier.

Example: Cyrillic (UTF-8).

If \a codec is 0, an empty QString is returned.
*/

QString fullCodecName(QTextCodec *codec)
{
    if (!codec || !supportedCodecsMap().contains(codec))
        return "";
    QString cn = supportedCodecsMap().value(codec);
    return codecDescriptiveName(cn) + " (" + cn + ")";
}

/*!
\fn QString BeQt::fullCodecName(const QString &codecName)
\overload
Returns the full codec name of a codec with identifier \a codecName. The full name contains of the codec description
(localized string) and the identifier.

Example: Cyrillic (UTF-8).

If \a codecName is not a valid codec name (identifier), an empty QString is returned.
*/

QString fullCodecName(const QString &codecName)
{
    return fullCodecName(codec(codecName));
}

/*!
\fn bool BeQt::isCodecSupported(QTextCodec *codec)
Returns true if \a codec is supported by Qt's codec system (QTextCodec) and by BeQt. Otherwise returns false.

If \a codec is 0, false is returned.

\sa supportedCodecs()
*/

bool isCodecSupported(QTextCodec *codec)
{
    return codec && supportedCodecsMap().contains(codec);
}

/*!
\fn bool BeQt::isCodecSupported(const QString &codecName)
\overload
Returns true if a codec with name \a codecName is supported by Qt's codec system (QTextCodec) and by BeQt.
Otherwise returns false.

\sa supportedCodecs()
*/

bool isCodecSupported(const QString &codecName)
{
    return isCodecSupported(codec(codecName));
}

#if defined(Q_OS_LINUX)

/*!
\fn QString BeQt::linuxVersion()
Returns a string containing the version of the Linux distribution (OS), on which the application is running.

Information is retrieved from the /etc/lsb-release file. If no such file is found or it can not be read, "Unknown" is
returned.

\note This function is only avaliable on Linux (more exactly, if Q_OS_LINUX is defined).
*/

QString linuxVersion()
{
    bool ok = false;
    QStringList sl = BDirTools::readTextFile("/etc/lsb-release", "Latin-1", &ok).split('\n', QString::SkipEmptyParts);
    if (!ok || sl.isEmpty())
        return "Unknown";
    sl = sl.last().split('=', QString::SkipEmptyParts);
    return (sl.size() == 2) ? ("Linux " + BTextTools::unwrapped(sl.last(), "\"")) : QString("Unknown");
}

#endif

#if defined(Q_OS_MAC)

/*!
\fn QString BeQt::macVersion()
Returns a string containing the version of the Mac OS X, on which the application is running.

Information is retrieved from the QSysInfo class.

\note This function is only avaliable on Mac OS X (more exactly, if Q_OS_MAC is defined).
*/

QString macVersion()
{
    return macVersionToString(QSysInfo::macVersion());
}

/*!
\fn QString BeQt::macVersionToString(QSysInfo::MacVersion version)
Returns human-readable string representation of \a version of QSysInfo::MacVersion member.

\note This function is only avaliable on Mac OS X (more exactly, if Q_OS_MAC is defined).
*/

QString macVersionToString(QSysInfo::MacVersion version)
{
    switch (version) {
    case QSysInfo::MV_9:
        return "Mac OS 9";
    case QSysInfo::MV_10_0:
        return "Mac OS X 10.0 Cheetah";
    case QSysInfo::MV_10_1:
        return "Mac OS X 10.1 Puma";
    case QSysInfo::MV_10_2:
        return "Mac OS X 10.2 Jaguar";
    case QSysInfo::MV_10_3:
        return "Mac OS X 10.3 Panther";
    case QSysInfo::MV_10_4:
        return "Mac OS X 10.4 Tiger";
    case QSysInfo::MV_10_5:
        return "Mac OS X 10.5 Leopard";
    case QSysInfo::MV_10_6:
        return "Mac OS X 10.6 Snow Leopard";
    case QSysInfo::MV_10_7:
        return "Mac OS X 10.7 Lion";
    case QSysInfo::MV_10_8:
        return "Mac OS X 10.8 Mountain Lion";
    default:
        return "Unknown";
    }
}

#endif

/*!
\fn void BeQt::msleep(unsigned long msecs)
Causes the current thread to sleep for \a msecs milliseconds.
*/

void msleep(unsigned long msecs)
{
    ThreadHack::msleepHack(msecs);
}

/*!
\fn OSType BeQt::osType()
Returns the type of operation system on which the application is running. If neither Q_OS_MAC, Q_OS_LINUX, nor Q_OS_WIN
macro is defined, UnknownOS is returned.

\sa OSType
*/

OSType osType()
{
#if defined(Q_OS_MAC)
    return MacOS;
#elif defined(Q_OS_LINUX)
    return LinuxOS;
#elif defined(Q_OS_WIN)
    return WindowsOS;
#else
    return UnknownOS;
#endif
}

/*!
\fn QString BeQt::osTypeToString(OSType t)
Returns human-readable string representation of an operating system type \a t.
*/

QString osTypeToString(OSType t)
{
    switch (t) {
    case LinuxOS:
        return "Linux";
    case MacOS:
        return "MacOS X";
    case WindowsOS:
        return "Windows";
    case UnknownOS:
    default:
        return "";
    }
}

/*!
\fn QString BeQt::osVersion()
Returns a string containing the version of the operating system, on which the application is running.

Information is retrieved using either macVersion(), linuxVersion(), or windowsVersion(), depending on the platform.

On an unsupported platform, an empty QString is returned.
*/

QString osVersion()
{
#if defined(Q_OS_MAC)
    return macVersion();
#elif defined(Q_OS_LINUX)
    return linuxVersion();
#elif defined(Q_OS_WIN)
    return windowsVersion();
#else
    return "";
#endif
}

/*!
\fn LineFeed BeQt::platformLineFeed()
Returns s line feed, which is used by default on the platform where the application is running. If the platform is not
supported, DefaultLineFeed is returned.
*/

LineFeed platformLineFeed()
{
#if defined(Q_OS_MAC)
    return ClassicMacLineFeed);
#elif defined(Q_OS_UNIX)
    return UnixLineFeed;
#elif defined(Q_OS_WIN)
    return WindowsLineFeed;
#else
    return DefaultLineFeed;
#endif
}

/*!
\fn ProcessorArchitecture BeQt::processorArchitecture()
Returns a processor architecture type for which the application was compiled.

{BEQT_ARCH_*} macros are used to determine the architecture.

On an unsupported platform, UnknownArchitecture is returned.
*/

ProcessorArchitecture processorArchitecture()
{
#if defined(BEQT_ARCH_ALPHA)
    return AlphaArchitecture;
#elif defined(BEQT_ARCH_AMD64)
    return Amd64Architecture;
#elif defined(BEQT_ARCH_ARM)
    return ArmArchitecture;
#elif defined(BEQT_ARCH_ARM64)
    return Arm64Architecture;
#elif defined(BEQT_ARCH_BLACKFIN)
    return BlackfinArchitecture;
#elif defined(BEQT_ARCH_CONVEX)
    return ConvexArchitecture;
#elif defined(BEQT_ARCH_EPIPHANY)
    return EpiphanyArchitecture;
#elif defined(BEQT_ARCH_HP_PA_RISC)
    return HpPaRiscArchitecture;
#elif defined(BEQT_ARCH_INTEL_X86)
    return IntelX86Architecture;
#elif defined(BEQT_ARCH_INTEL_ITANIUM)
    return IntelItaniumArchitecture;
#elif defined(BEQT_ARCH_MOTOROLA_68K)
    return Motorola68kAArchitecture;
#elif defined(BEQT_ARCH_MIPS)
    return MipsArchitecture;
#elif defined(BEQT_ARCH_POWERPC)
    return PowerPcArchitecture;
#elif defined(BEQT_ARCH_PYRAMID_9810)
    return Pyramid9810Architecture;
#elif defined(BEQT_ARCH_RS_6000)
    return Rs6000Architecture;
#elif defined(BEQT_ARCH_SPARC)
    return SparcArchitecture;
#elif defined(BEQT_ARCH_SUPERH)
    return SuperHArchitecture;
#elif defined(BEQT_ARCH_SYSTEMZ)
    return SystemZArchitecture;
#elif defined(BEQT_ARCH_TMS320)
    return Tms320Architecture;
#elif defined(BEQT_ARCH_TMS470)
    return Tms470Architecture;
#else
    return UnknownArchitecture;
#endif
}

/*!
\fn QString BeQt::processorArchitectureToString(ProcessorArchitecture arch)
Returns human-readable string representation of a processro architecture \a arch.
*/

QString processorArchitectureToString(ProcessorArchitecture arch)
{
    switch (arch) {
    case AlphaArchitecture:
        return "Alpha";
    case Amd64Architecture:
        return "AMD64";
    case ArmArchitecture:
        return "ARM";
    case Arm64Architecture:
        return "ARM64";
    case BlackfinArchitecture:
        return "Blackfin";
    case ConvexArchitecture:
        return "Convex";
    case EpiphanyArchitecture:
        return "Epiphany";
    case HpPaRiscArchitecture:
        return "HP/PA RISC";
    case IntelX86Architecture:
        return "Intel x86";
    case IntelItaniumArchitecture:
        return "Intel Itanium (IA-64)";
    case Motorola68kAArchitecture:
        return "Motorola 68k";
    case MipsArchitecture:
        return "MIPS";
    case PowerPcArchitecture:
        return "PowerPC";
    case Pyramid9810Architecture:
        return "Pyramid 9810";
    case Rs6000Architecture:
        return "RS/6000";
    case SparcArchitecture:
        return "SPARC";
    case SuperHArchitecture:
        return "SuperH";
    case SystemZArchitecture:
        return "SystemZ";
    case Tms320Architecture:
        return "TMS320";
    case Tms470Architecture:
        return "TMS470";
    case UnknownArchitecture:
    default:
        return "Unknown";
    }
}

/*!
\fn BProperties BeQt::propertiesFromString(const QString &s, bool resolveVariables, bool *ok)
Parses a string \a s and creates a BProperties file from it. See \l https://en.wikipedia.org/wiki/.properties for the
description of the .properties format.

If \a resolveVariables is true, the values of the variables enclosed in braces are resolved when parsing; otherwise
they are treated as raw values.

If \a ok is a non-zero pointer, it's value is set to true on success, and to false on any error.

\sa propertiesToString()
*/

BProperties propertiesFromString(const QString &s, bool resolveVariables, bool *ok)
{
    if (s.isEmpty())
        return bRet(ok, true, BProperties());
    QString key;
    QString value;
    bool multiline = false;
    BProperties p;
    foreach (QString line, s.split(QRegExp("(\n|\r)+"), QString::SkipEmptyParts)) {
        if (line.startsWith('#') || line.startsWith('!'))
            continue;
        line.remove(QRegExp("^\\s+"));
        if (line.isEmpty())
            continue;
        QString stack;
        for (int i = 0; i < line.length(); ++ i) {
            const QChar &c = line.at(i);
            if (('=' == c || ':' == c) && (0 == i || line.at(i - 1) != '\\')) {
                if (multiline) {
                    BTextTools::removeTrailingSpaces(&key);
                    key.replace("\\ ", " ");
                    value.replace("\\\\", "\\");
                    QRegExp rx("\\\\u\\d{4}");
                    int ind = value.indexOf(rx);
                    while (ind >= 0) {
                        value.replace(ind, 6, QChar(value.mid(ind + 2, 4).toInt()));
                        ind = value.indexOf(rx, ind + 1);
                    }
                    if (resolveVariables && (value.startsWith('{') || value.startsWith("${")) && value.endsWith('}')) {
                        QString valueUnwrapped = BTextTools::unwrapped(value, "${", "}");
                        BTextTools::unwrap(valueUnwrapped, "{", "}");
                        value = p.value(valueUnwrapped);
                    }
                    p.insert(key, value);
                    key.clear();
                    value.clear();
                    multiline = false;
                }
                if (!key.isEmpty())
                    return bRet(ok, false, BProperties());
                key = stack;
                stack.clear();
                QRegExp rx("\\s+");
                int ind = line.indexOf(rx, i + 1);
                if (ind >= 0)
                    i += rx.matchedLength();
            } else if ('\\' == c && line.length() - 1 == i) {
                if (key.isEmpty())
                    return bRet(ok, false, BProperties());
                multiline = true;
                value += stack;
            } else if (('!' == c || '#' == c || '=' == c || ':' == c) && (0 == i || line.at(i - 1) == '\\')) {
                stack.remove(stack.length() - 1, 1);
                stack += c;
            } else {
                stack += c;
            }
        }
        if (!multiline) {
            value = stack;
            BTextTools::removeTrailingSpaces(&key);
            key.replace("\\ ", " ");
            value.replace("\\\\", "\\");
            QRegExp rx("\\\\u\\d{4}");
            int ind = value.indexOf(rx);
            while (ind >= 0) {
                value.replace(ind, 6, QChar(value.mid(ind + 2, 4).toInt()));
                ind = value.indexOf(rx, ind + 1);
            }
            if (resolveVariables && (value.startsWith('{') || value.startsWith("${")) && value.endsWith('}')) {
                QString valueUnwrapped = BTextTools::unwrapped(value, "${", "}");
                BTextTools::unwrap(valueUnwrapped, "{", "}");
                value = p.value(valueUnwrapped);
            }
            p.insert(key, value);
            key.clear();
            value.clear();
        } else {
            value += stack;
        }
    }
    return bRet(ok, true, p);
}

/*!
\fn BProperties BeQt::propertiesFromString(const QString &s, bool *ok)
\overload
Parses a string \a s and creates a BProperties file from it. See \l https://en.wikipedia.org/wiki/.properties for the
description of the .properties format.

The values of the variables enclosed in braces are resolved when parsing.

If \a ok is a non-zero pointer, it's value is set to true on success, and to false on any error.

\sa propertiesFromString(), propertiesToString()
*/

BProperties propertiesFromString(const QString &s, bool *ok)
{
    return propertiesFromString(s, true, ok);
}

/*!
\fn QString BeQt::propertiesToString(const BProperties &p)
Converts \a p to a string in the .properties file format. See \l https://en.wikipedia.org/wiki/.properties for the
description of the .properties format.

\sa propertiesFromString()
*/

QString propertiesToString(const BProperties &p)
{
    QString s;
    foreach (QString key, p.keys()) {
        QString value = p.value(key);
        key.replace(' ', "\\ ").replace('=', "\\=").replace(':', "\\:").replace('#', "\\#").replace('!', "\\!");
        value.replace("\\", "\\\\").replace('=', "\\=").replace(':', "\\:").replace('#', "\\#").replace('!', "\\!");
        value.replace('\n', "\\n").replace('\r', "\\r").replace('\t', "\\t");
        s += key + " = " + value + "\n";
    }
    if (!s.isEmpty())
        s.remove(s.length() - 1, 1);
    return s;
}

/*!
\fn QByteArray BeQt::serialize(const QVariant &variant, QDataStream::Version version)
Serializes \a variant to a QByteArray data using QDataStream. \a version is used as a QDataStream version.

Returns the resulting QByteArray.
*/

QByteArray serialize(const QVariant &variant, QDataStream::Version version)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(version);
    out << variant;
    return data;
}

/*!
\fn void BeQt::sleep(unsigned long secs)
Causes the current thread to sleep for \a secs seconds.
*/

void sleep(unsigned long secs)
{
    ThreadHack::sleepHack(secs);
}

/*!
\fn void BeQt::startProcess(QProcess *proc, const QString &command, const QStringList &arguments)
Starts a process \a proc. The \a command argument is used as a command, and \a arguments are the arguments.

On Windows, this function merges the command and the arguments to avoid a bug with long arguments.

\sa execProcess()
*/

void startProcess(QProcess *proc, const QString &command, const QStringList &arguments)
{
    if (!proc)
        return;
    //Workaround to handle long arguments on Windows
#if defined(Q_OS_WIN)
    proc->setNativeArguments(BTextTools::mergeArguments(arguments));
    proc->start(command);
#else
    proc->start(command, arguments);
#endif
    //End of the workaround
}

/*!
\fn bool BeQt::startProcessDetached(const QString &command, const QStringList &arguments)
Starts a process detached. The \a command argument is used as a command, and \a arguments are the arguments.

Returns true on success; otherwise returns false.

\sa startProcess()
*/

bool startProcessDetached(const QString &command, const QStringList &arguments)
{
    return startProcessDetached(command, QString(), arguments);
}

/*!
\fn bool BeQt::startProcessDetached(const QString &command, const QString &workingDir, const QStringList &arguments)
Starts a process detached. The \a command argument is used as a command, and \a arguments are the arguments. The
working directory is set to \a workingDir.

Returns true on success; otherwise returns false.

\sa startProcess()
*/

bool startProcessDetached(const QString &command, const QString &workingDir, const QStringList &arguments)
{
    if (command.isEmpty())
        return false;
    return QProcess::startDetached(command, arguments, workingDir);
}

/*!
\fn QList<QTextCodec *> BeQt::supportedCodecs()
Returns a list of pointers to all codecs supported by Qt's codec system (QTextCodec) and by BeQt.

\sa isCodecSupported()
*/

QList<QTextCodec *> supportedCodecs()
{
    return supportedCodecsMap().keys();
}

/*!
\fn QStringList BeQt::supportedCodecsNames()
Returns a list of names (identifiers) of all codecs supported by Qt's codec system (QTextCodec) and by BeQt.

\sa supportedCodecs()
*/

QStringList supportedCodecsNames()
{
    static QStringList list = QStringList() << supportedUnicodeCodecNames() << supportedEastEuropeanCodecNames()
        << supportedWestEuropeanCodecNames() << supportedEastAsianCodecNames()
        << supportedSouthEastSouthWestAsianCodecNames() << supportedMiddleEastCodecNames();
    return list;
}

/*!
\fn TabWidth BeQt::tabWidthFromInt(int tw)
Casts \a tw to a TabWidth. Returns the resulting TabWidth. If the value of \a tw does not match any TabWidth member,
TabWidth4 is returned.
*/

TabWidth tabWidthFromInt(int tw)
{
    static const QList<int> values = QList<int>() << TabWidth2 << TabWidth4 << TabWidth8;
    return values.contains(tw) ? static_cast<TabWidth>(tw) : TabWidth4;
}

/*!
\fn void BeQt::takeOwnership(QObject *object, QObject *newOwner)
Makes a \a newOwner become a parent of an \a object, if \a object does not have any parent. Otherwise does nothing.

If \a object is 0 or \a newOwner is 0, nothing is done.
*/

void takeOwnership(QObject *object, QObject *newOwner)
{
    if (!object || !newOwner)
        return;
    if (!object->parent())
        object->setParent(newOwner);
}

/*!
\fn Target BeQt::target(const QObject *object, const char *method)

Returns a Target pair, constructed from the given \a object and \a method.
*/

Target target(const QObject *object, const char *method)
{
    return qMakePair(object, method);
}

/*!
\fn QString BeQt::translate(const char *context, const char *sourceText, const char *disambiguation, int n)

A wrapper function for QCoreApplication::translate(). Provides the same interface for Qt4 and Qt5.

Returns the translation text for \a sourceText, by querying the installed translation files. The translation files are
searched from the most recently installed file back to the first installed file.

\a context is typically a class name (e.g., "MyDialog") and sourceText is either English text or a short identifying
text.

\a disambiguation is an identifying string, for when the same sourceText is used in different roles within the same
context. By default, it is null.

\a n is used in conjunction with %n to support plural forms.
*/

QString translate(const char *context, const char *sourceText, const char *disambiguation, int n)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    return QCoreApplication::translate(context, sourceText, disambiguation, QCoreApplication::CodecForTr, n);
#else
    return QCoreApplication::translate(context, sourceText, disambiguation, n);
#endif
}

/*!
\fn Until BeQt::until(const QObject *object, const char *signal)

Returns an Until pair, constructed from the given \a object and \a signal.
*/

Until until(const QObject *object, const char *signal)
{
    return qMakePair(object, signal);
}

/*!
\fn void BeQt::usleep(unsigned long usecs)
Causes the current thread to sleep for \a usecs microseconds.
*/

void usleep(unsigned long usecs)
{
    ThreadHack::usleepHack(usecs);
}

/*!
\fn void BeQt::waitNonBlocking(int msecs)
Starts a local QEventLoop which is finished after \a msecs milliseconds pass.

If \a msecs is less or equal to zero, the loop will be executed forever.

This function blocks execution of the current function, but does not block the Qt's signal and slot system. The GUI is
not block either.
*/

void waitNonBlocking(int msecs)
{
    waitNonBlocking(QList<Until>(), msecs);
}

/*!
\fn void BeQt::waitNonBlocking(const QObject *sender, const char *signal, int msecs)
\overload
Starts a local QEventLoop which is finished after \a msecs milliseconds pass or when \a sender emits \a signal.

If \a msecs is less or equal to zero, the loop can be only terminated by emitting the \a {sender}'s \a signal.

This function blocks execution of the current function, but does not block the Qt's signal and slot system. The GUI is
not block either.
*/

void waitNonBlocking(const QObject *sender, const char *signal, int msecs)
{
    waitNonBlocking(QList<Until>() << until(sender, signal), msecs);
}

/*!
\fn void BeQt::waitNonBlocking(const QObject *sender1, const char *signal1, const QObject *sender2,
                               const char *signal2, int msecs)
\overload
Starts a local QEventLoop which is finished after \a msecs milliseconds pass or when \a sender1 emits \a signal1, or
when \a sender2 emits \a signal2.

If \a msecs is less or equal to zero, the loop can be only terminated by emitting the corresponding signal of one of
the senders.

This function blocks execution of the current function, but does not block the Qt's signal and slot system. The GUI is
not block either.
*/

void waitNonBlocking(const QObject *sender1, const char *signal1, const QObject *sender2, const char *signal2,
                     int msecs)
{
    waitNonBlocking(QList<Until>() << until(sender1, signal1) << until(sender2, signal2), msecs);
}

/*!
\fn void BeQt::waitNonBlocking(const QList<Until> &list, int msecs)
\overload
Starts a local QEventLoop which is finished after \a msecs milliseconds pass or when one of the senders form the
\a list emits the corresponding signal.

If \a msecs is less or equal to zero, the loop can be only terminated by emitting the corresponding signal of one of
the senders.

This function blocks execution of the current function, but does not block the Qt's signal and slot system. The GUI is
not block either.
*/

void waitNonBlocking(const QList<Until> &list, int msecs)
{
    QEventLoop el;
    bool b = false;
    foreach (const Until &u, list) {
        if (!u.first || !u.second)
            continue;
        QObject::connect(u.first, u.second, &el, SLOT(quit()));
        b = true;
    }
    if (msecs > 0)
        QTimer::singleShot(msecs, &el, SLOT(quit()));
    else if (!b)
        return;
    el.exec();
}

#if defined(Q_OS_WIN)

/*!
\fn QString BeQt::windowsVersion()
Returns a string containing the version of the Windows operationg system, on which the application is running.

Information is retrieved from the QSysInfo class.

\note This function is only avaliable on Windows (more exactly, if Q_OS_WIN is defined).
*/

QString windowsVersion()
{
    return windowsVersionToString(QSysInfo::windowsVersion());
}

/*!
\fn QString BeQt::windowsVersionToString(QSysInfo::WinVersion version)
Returns human-readable string representation of \a version of QSysInfo::WinVersion member.

\note This function is only avaliable on Windows (more exactly, if Q_OS_WIN is defined).
*/

QString windowsVersionToString(QSysInfo::WinVersion version)
{
    switch (version) {
    case QSysInfo::WV_32s:
        return "Windows 3.1";
    case QSysInfo::WV_95:
        return "Windows 95";
    case QSysInfo::WV_98:
        return "Windows 98";
    case QSysInfo::WV_Me:
        return "Windows Me";
    case QSysInfo::WV_NT:
        return "Windows NT (4.0)";
    case QSysInfo::WV_2000:
        return "Windows 2000 (5.0)";
    case QSysInfo::WV_XP:
        return "Windows XP (5.1)";
    case QSysInfo::WV_2003:
        return "Windows Server 2003 / Windows Server 2003 R2 / "
                "Windows Home Server / Windows XP Professional x64 (5.2)";
    case QSysInfo::WV_VISTA:
        return "Windows Vista / Windows Server 2008 (6.0)";
    case QSysInfo::WV_WINDOWS7:
        return "Windows 7 / Windows Server 2008 R2 (6.1)";
    case QSysInfo::WV_WINDOWS8:
        return "Windows 8 (6.2)";
    default:
        return "Unknown";
    }
}

#endif

}
