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
#include "btexttools.h"
#include "buuid.h"

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

namespace BeQt
{

int area(const QRect &r)
{
    return r.size().width() * r.size().height();
}

qreal area(const QRectF &r)
{
    return r.size().width() * r.size().height();
}

QString canonicalUuidText(const QString &uuidText)
{
    if (uuidText.isEmpty())
        return "";
    QString t = (uuidText.at(0) != '{' ? "{" : "") + uuidText + (uuidText.at(uuidText.length() - 1) != '}' ? "}" : "");
    return !BUuid(t).isNull() ? t : QString();
}

QTextCodec *codec(const QString &cn)
{
    return codec(cn.toLatin1());
}

QTextCodec *codec(const QByteArray &cn)
{
    QTextCodec *c = !cn.isEmpty() ? QTextCodec::codecForName(cn) : 0;
    return (c && supportedCodecsMap().contains(c)) ? c : 0;
}

QString codecName(QTextCodec *codec)
{
    return supportedCodecsMap().value(codec);
}

QString codecName(const QByteArray &cn)
{
    return codecName(codec(cn));
}

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

QList<QTextCodec *> codecsForGroup(CodecsGroup group)
{
    QList<QTextCodec *> list;
    foreach (const QString &name, codecNamesForGroup(group))
        list << codec(name);
    return list;
}

CodecsGroup codecsGroupFromInt(int cg)
{
    static const QList<int> groups = bRangeD(InvalidGroup, MiddleEastGroup);
    return groups.contains(cg) ? static_cast<CodecsGroup>(cg) : InvalidGroup;
}

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

QList<CodecsGroup> codecsGroups()
{
    static QList<CodecsGroup> list = QList<CodecsGroup>() << UnicodeGroup << EastEuropeanGroup << WestEuropeanGroup
        << EastAsianGroup << SouthEastSouthWestAsianGroup << MiddleEastGroup;
    return list;
}

void deleteLaterUnowned(QObject *object, QObject *possibleOwner)
{
    if (!object)
        return;
    if (!object->parent() || (object->parent() == possibleOwner))
        object->deleteLater();
}

QVariant deserialize(const QByteArray &data, QDataStream::Version version)
{
    QDataStream in(data);
    in.setVersion(version);
    QVariant v;
    in >> v;
    return v;
}

void deleteUnowned(QObject *object, QObject *possibleOwner)
{
    if (!object)
        return;
    if (!object->parent() || (object->parent() == possibleOwner))
        delete object;
}

int execProcess(const QString &command, const QStringList &arguments,
                int startTimeout, int finishTimeout, QString *output, QTextCodec *codec)
{
    QFileInfo fi(command);
    return execProcess(fi.isAbsolute() ? fi.path() : QString(), command, arguments, startTimeout, finishTimeout,
                       output, codec);
}

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

QString fullCodecName(QTextCodec *codec)
{
    if (!codec || !supportedCodecsMap().contains(codec))
        return "";
    QString cn = supportedCodecsMap().value(codec);
    return codecDescriptiveName(cn) + " (" + cn + ")";
}

QString fullCodecName(const QString &codecName)
{
    return fullCodecName(codec(codecName));
}

bool isCodecSupported(QTextCodec *codec)
{
    return codec && supportedCodecsMap().contains(codec);
}

bool isCodecSupported(const QString &codecName)
{
    return isCodecSupported(codec(codecName));
}

#if defined(Q_OS_LINUX)
QString linuxVersion()
{
    bool ok = false;
    QStringList sl = BDirTools::readTextFile("/etc/lsb-release", "Latin-1", &ok).split('\n', QString::SkipEmptyParts);
    if (!ok || sl.isEmpty())
        return "Unknown";
    sl = sl.last().split('=', QString::SkipEmptyParts);
    return (sl.size() == 2) ? ("Linux " + BTextTools::unwrapped(sl.last())) : QString("Unknown");
}
#endif

#if defined(Q_OS_MAC)
QString macVersion()
{
    return macVersionToString(QSysInfo::macVersion());
}

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

void msleep(unsigned long msecs)
{
    ThreadHack::msleepHack(msecs);
}

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
#endif
}

QString pureUuidText(const BUuid &uuid)
{
    QString t = uuid.toString();
    return t.mid(1, t.length() - 2);
}

QString pureUuidText(const QString &uuidText)
{
    return pureUuidText(BUuid(canonicalUuidText(uuidText)));
}

QByteArray serialize(const QVariant &variant, QDataStream::Version version)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(version);
    out << variant;
    return data;
}

void sleep(unsigned long secs)
{
    ThreadHack::sleepHack(secs);
}

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

bool startProcessDetached(const QString &command, const QStringList &arguments)
{
    return startProcessDetached(command, QString(), arguments);
}

bool startProcessDetached(const QString &command, const QString &workingDir, const QStringList &arguments)
{
    if (command.isEmpty())
        return false;
    //Workaround to handle long arguments on Windows
    return QProcess::startDetached(command + " " + BTextTools::mergeArguments(arguments), QStringList(), workingDir);
    //End of the workaround
}

QList<QTextCodec *> supportedCodecs()
{
    return supportedCodecsMap().keys();
}

QStringList supportedCodecsNames()
{
    static QStringList list = QStringList() << supportedUnicodeCodecNames() << supportedEastEuropeanCodecNames()
        << supportedWestEuropeanCodecNames() << supportedEastAsianCodecNames()
        << supportedSouthEastSouthWestAsianCodecNames() << supportedMiddleEastCodecNames();
    return list;
}

TabWidth tabWidthFromInt(int tw)
{
    static const QList<int> values = bRangeD(TabWidth2, TabWidth8);
    return values.contains(tw) ? static_cast<TabWidth>(tw) : TabWidth4;
}

void takeOwnership(QObject *object, QObject *newOwner)
{
    if (!object || !newOwner)
        return;
    if (!object->parent())
        object->setParent(newOwner);
}

Target target(const QObject *object, const char *method)
{
    Target p;
    p.first = object;
    p.second = method;
    return p;
}

QString translate(const char *context, const char *sourceText, const char *disambiguation, int n)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    return QCoreApplication::translate(context, sourceText, disambiguation, QCoreApplication::CodecForTr, n);
#else
    return QCoreApplication::translate(context, sourceText, disambiguation, n);
#endif
}

Until until(const QObject *object, const char *signal)
{
    Until p;
    p.first = object;
    p.second = signal;
    return p;
}

void usleep(unsigned long usecs)
{
    ThreadHack::usleepHack(usecs);
}

BUuid uuidFromText(const QString &uuidText)
{
    return BUuid(canonicalUuidText(uuidText));
}

void waitNonBlocking(int msecs)
{
    waitNonBlocking(QList<Until>(), msecs);
}

void waitNonBlocking(const QObject *sender, const char *signal, int msecs)
{
    waitNonBlocking(QList<Until>() << until(sender, signal), msecs);
}

void waitNonBlocking(const QObject *sender1, const char *signal1, const QObject *sender2, const char *signal2,
                     int msecs)
{
    waitNonBlocking(QList<Until>() << until(sender1, signal1) << until(sender2, signal2), msecs);
}

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
QString windowsVersion()
{
    return windowsVersionToString(QSysInfo::windowsVersion());
}

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
