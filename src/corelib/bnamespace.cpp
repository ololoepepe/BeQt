#include "bnamespace.h"
#include "bterminaliohandler.h"
#include "bdirtools.h"
#include "btexttools.h"

#include <QEventLoop>
#include <QTimer>
#include <QUuid>
#include <QString>
#include <QProcess>
#include <QStringList>
#include <QByteArray>
#include <QTextCodec>
#include <QTextStream>
#include <QObject>
#include <QList>
#include <QPair>
#include <QSysInfo>
#include <QRect>
#include <QRectF>
#include <QSize>
#include <QSizeF>
#include <QVariant>
#include <QCoreApplication>
#include <QRegExp>
#include <QMap>

#include <QDebug>

namespace BeQt
{

//Internal

void removeUnsupportedCodecNames(QStringList &list)
{
    foreach (int i, bRangeR(list.size() - 1, 0))
        if (!QTextCodec::codecForName(list.at(i).toLatin1()))
            list.removeAt(i);
}

QStringList supportedUnicodeCodecNames()
{
    init_once(QStringList, list, QStringList() << "UTF-8" << "UTF-16")
        removeUnsupportedCodecNames(list);
    return list;
}

QStringList supportedEastEuropeanCodecNames()
{
    init_once(QStringList, list, QStringList() << "ISO 8859-13" << "ISO 8859-4" << "Windows-1257" << "ISO 8859-5")
    {
        list << "KOI8-R" << "Windows-1251" << "IBM 866" << "KOI8-U" << "ISO 8859-16" << "ISO 8859-2" << "Windows-1250";
        removeUnsupportedCodecNames(list);
    }
    return list;
}

QStringList supportedWestEuropeanCodecNames()
{
    init_once(QStringList, list, QStringList() << "ISO 8859-7" << "Windows-1253" << "IBM 850" << "ISO 8859-1")
    {
        list << "ISO 8859-15" << "Apple Roman" << "Windows-1252" << "ISO 8859-14" << "ISO 8859-10" << "ISO 8859-3";
        removeUnsupportedCodecNames(list);
    }
    return list;
}

QStringList supportedEastAsianCodecNames()
{
    init_once(QStringList, list, QStringList() << "Windows-1258" << "Big5" << "Big5-HKSCS" << "GB18030-0" << "EUC-KR")
    {
        list << "JOHAB" << "EUC-JP" << "ISO 2022-JP" << "Shift-JIS";
        removeUnsupportedCodecNames(list);
    }
    return list;
}

QStringList supportedSouthEastSouthWestAsianCodecNames()
{
    init_once(QStringList, list, QStringList() << "TIS-620" << "ISO 8859-9" << "Windows-1254")
        removeUnsupportedCodecNames(list);
    return list;
}

QStringList supportedMiddleEastCodecNames()
{
    init_once(QStringList, list, QStringList()  << "ISO 8859-6" << "Windows-1256" << "Windows-1255" << "ISO 8859-8")
        removeUnsupportedCodecNames(list);
    return list;
}

QMap<QTextCodec *, QString> supportedCodecsMap()
{
    typedef QMap<QTextCodec *, QString> CodecMap;
    init_once(CodecMap, m, CodecMap())
        foreach (const QString &cn, supportedCodecsNames())
            m.insert(QTextCodec::codecForName(cn.toLatin1()), cn);
    return m;
}

QString codecDescriptiveName(const QString &codecName)
{
    if (supportedUnicodeCodecNames().contains(codecName))
    {
        return translate("BeQt", "Unicode", "codec descriptiveName");
    }
    else if (supportedEastEuropeanCodecNames().contains(codecName))
    {
        if ((QStringList() << "ISO 8859-13" << "ISO 8859-4" << "Windows-1257").contains(codecName, Qt::CaseInsensitive))
            return translate("BeQt", "Baltic", "codec descriptiveName");
        else if ((QStringList() << "ISO 8859-5" << "KOI8-R"
                  << "Windows-1251" << "IBM 866").contains(codecName, Qt::CaseInsensitive))
            return translate("BeQt", "Cyrillic", "codec descriptiveName");
        else if (!codecName.compare("KOI8-U", Qt::CaseInsensitive))
            return translate("BeQt", "Cyrillic (Ukrainian)", "codec descriptiveName");
        else if (!codecName.compare("ISO 8859-16", Qt::CaseInsensitive))
            return translate("BeQt", "Romanic", "codec descriptiveName");
        else if ((QStringList() << "ISO 8859-2" << "Windows-1250").contains(codecName, Qt::CaseInsensitive))
            return translate("BeQt", "Central European", "codec descriptiveName");
    }
    else if (supportedWestEuropeanCodecNames().contains(codecName))
    {
        if ((QStringList() << "ISO 8859-7" << "Windows-1253").contains(codecName, Qt::CaseInsensitive))
            return translate("BeQt", "Greek", "codec descriptiveName");
        else if ((QStringList() << "IBM 850" << "ISO 8859-1" << "ISO 8859-15"
                  << "Apple Roman" << "Windows-1252").contains(codecName, Qt::CaseInsensitive))
            return translate("BeQt", "Western", "codec descriptiveName");
        else if (!codecName.compare("ISO 8859-14", Qt::CaseInsensitive))
            return translate("BeQt", "Celtic", "codec descriptiveName");
        else if (!codecName.compare("ISO 8859-10", Qt::CaseInsensitive))
            return translate("BeQt", "Nordic", "codec descriptiveName");
        else if (!codecName.compare("ISO 8859-3", Qt::CaseInsensitive))
            return translate("BeQt", "South European", "codec descriptiveName");
    }
    else if (supportedEastAsianCodecNames().contains(codecName))
    {
        if (!codecName.compare("Windows-1258", Qt::CaseInsensitive))
            return translate("BeQt", "Vietnamese", "codec descriptiveName");
        else if ((QStringList() << "Big5" << "Big5-HKSCS").contains(codecName, Qt::CaseInsensitive))
            return translate("BeQt", "Traditional Chinese", "codec descriptiveName");
        else if (!codecName.compare("GB18030-0", Qt::CaseInsensitive))
            return translate("BeQt", "Simplified Chinese", "codec descriptiveName");
        else if ((QStringList() << "EUC-KR" << "JOHAB").contains(codecName, Qt::CaseInsensitive))
            return translate("BeQt", "Korean", "codec descriptiveName");
        else if ((QStringList() << "EUC-JP" << "ISO 2022-JP" << "Shift-JIS").contains(codecName, Qt::CaseInsensitive))
            return translate("BeQt", "Japanese", "codec descriptiveName");
    }
    else if (supportedSouthEastSouthWestAsianCodecNames().contains(codecName))
    {
        if (!codecName.compare("TIS-620", Qt::CaseInsensitive))
            return translate("BeQt", "Thai", "codec descriptiveName");
        else if ((QStringList() << "ISO 8859-9" << "Windows-1254").contains(codecName, Qt::CaseInsensitive))
            return translate("BeQt", "Turkish", "codec descriptiveName");
    }
    else if (supportedMiddleEastCodecNames().contains(codecName))
    {
        if ((QStringList() << "ISO 8859-6" << "Windows-1256").contains(codecName, Qt::CaseInsensitive))
            return translate("BeQt", "Arabic", "codec descriptiveName");
        else if (!codecName.compare("Windows-1255", Qt::CaseInsensitive))
            return translate("BeQt", "Hebrew", "codec descriptiveName");
        else if (!codecName.compare("ISO 8859-8", Qt::CaseInsensitive))
            return translate("BeQt", "Hebrew (visual)", "codec descriptiveName");
    }
    return "";
}

QString fileSizeToStringInternal(qint64 sz, FileSizeFormat f, quint8 p)
{
    if (sz < 0)
        sz = 0;
    int d = 1;
    switch (f)
    {
    case GigabytesFormat:
        d = Gigabyte;
        break;
    case MegabytesFormat:
        d = Megabyte;
        break;
    case KilobytesFormat:
        d = Kilobyte;
        break;
    case BytesFormat:
    default:
        break;
    }
    QString s = QString::number(sz / d);
    if (1 == d || !p)
        return s;
    if (p > 3)
        p = 3;
    QString ss = QString::number((double) (sz % d) / (double) d).mid(2, p);
    while (!ss.isEmpty() && ss.at(ss.length() - 1) == '0')
        ss.remove(ss.length() - 1, 1);
    if (ss.isEmpty() && (sz % d))
        ss = "1";
    return s + (!ss.isEmpty() ? ("." + ss) : QString());
}

//External

int area(const QRect &r)
{
    return r.size().width() * r.size().height();
}

qreal area(const QRectF &r)
{
    return r.size().width() * r.size().height();
}

QByteArray variantToData(const QVariant &variant)
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(DataStreamVersion);
    out << variant;
    return ba;
}

QVariant dataToVariant(const QByteArray &data)
{
    QDataStream in(data);
    in.setVersion(DataStreamVersion);
    QVariant v;
    in >> v;
    return v;
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
    foreach (const Until &u, list)
    {
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

Until until(const QObject *object, const char *signal)
{
    Until p;
    p.first = object;
    p.second = signal;
    return p;
}

Target target(const QObject *object, const char *method)
{
    Target p;
    p.first = object;
    p.second = method;
    return p;
}

QString pureUuidText(const QUuid &uuid)
{
    QString t = uuid.toString();
    return t.mid(1, t.length() - 2);
}

QString pureUuidText(const QString &uuidText)
{
    return pureUuidText(QUuid(canonicalUuidText(uuidText)));
}

QString canonicalUuidText(const QString &uuidText)
{
    if (uuidText.isEmpty())
        return "";
    QString t = (uuidText.at(0) != '{' ? "{" : "") + uuidText + (uuidText.at(uuidText.length() - 1) != '}' ? "}" : "");
    return !QUuid(t).isNull() ? t : QString();
}

QUuid uuidFromText(const QString &uuidText)
{
    return QUuid(canonicalUuidText(uuidText));
}

QByteArray serialize(const QVariant &variant, QDataStream::Version version)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(version);
    out << variant;
    return data;
}

QVariant deserialize(const QByteArray &data, QDataStream::Version version)
{
    QDataStream in(data);
    in.setVersion(version);
    QVariant v;
    in >> v;
    return v;
}

void startProcess(QProcess *proc, const QString &command, const QStringList &arguments)
{
    if (!proc)
        return;
    //Workaround to handle long arguments on Windows
#if defined(Q_OS_WIN)
    proc->setNativeArguments(BTerminalIOHandler::mergeArguments(arguments));
    proc->start(command);
#else
    proc->start(command, arguments);
#endif
    //End of the workaround
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
    if (!proc.waitForStarted(startTimeout) || !proc.waitForFinished(finishTimeout))
    {
        proc.kill();
        return -2;
    }
    QTextStream in(&proc);
    if (codec)
        in.setCodec(codec);
    return (proc.exitStatus() == QProcess::NormalExit) ? bRet(output, in.readAll(), proc.exitCode()) : -1;
}

QString translate(const char *context, const char *sourceText, const char *disambiguation, int n)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    return QCoreApplication::translate(context, sourceText, disambiguation, QCoreApplication::CodecForTr, n);
#else
    return QCoreApplication::translate(context, sourceText, disambiguation, n);
#endif
}

bool isCodecSupported(QTextCodec *codec)
{
    return codec && supportedCodecsMap().contains(codec);
}

bool isCodecSupported(const QString &codecName)
{
    return isCodecSupported(codec(codecName));
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

QString codecName(QTextCodec *codec)
{
    return supportedCodecsMap().value(codec);
}

QString codecName(const QByteArray &cn)
{
    return codecName(codec(cn));
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

QTextCodec *codec(const QString &cn)
{
    return codec(cn.toLatin1());
}

QTextCodec *codec(const QByteArray &cn)
{
    QTextCodec *c = !cn.isEmpty() ? QTextCodec::codecForName(cn) : 0;
    return (c && supportedCodecsMap().contains(c)) ? c : 0;
}

QList<CodecsGroup> codecsGroups()
{
    static QList<CodecsGroup> list = QList<CodecsGroup>() << UnicodeGroup << EastEuropeanGroup << WestEuropeanGroup
        << EastAsianGroup << SouthEastSouthWestAsianGroup << MiddleEastGroup;
    return list;
}

QString codecsGroupName(CodecsGroup group)
{
    switch (group)
    {
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

QList<QTextCodec *> codecsForGroup(CodecsGroup group)
{
    QList<QTextCodec *> list;
    foreach (const QString &name, codecNamesForGroup(group))
        list << codec(name);
    return list;
}

QStringList codecNamesForGroup(CodecsGroup group)
{
    switch (group)
    {
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

CodecsGroup codecsGroupFromInt(int cg)
{
    static const QList<int> groups = bRangeD(InvalidGroup, MiddleEastGroup);
    return groups.contains(cg) ? static_cast<CodecsGroup>(cg) : InvalidGroup;
}

TabWidth tabWidthFromInt(int tw)
{
    static const QList<int> values = bRangeD(TabWidth2, TabWidth8);
    return values.contains(tw) ? static_cast<TabWidth>(tw) : TabWidth4;
}

QString fileSizeToString(qint64 size, FileSizeFormat format, quint8 precision)
{
    QString s;
    switch (format)
    {
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
    switch (format)
    {
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

void takeOwnership(QObject *object, QObject *newOwner)
{
    if (!object || !newOwner)
        return;
    if (!object->parent())
        object->setParent(newOwner);
}

void deleteUnowned(QObject *object, QObject *possibleOwner)
{
    if (!object)
        return;
    if (!object->parent() || object->parent() == possibleOwner)
        delete object;
}

void deleteLaterUnowned(QObject *object, QObject *possibleOwner)
{
    if (!object)
        return;
    if (!object->parent() || object->parent() == possibleOwner)
        object->deleteLater();
}

#if defined(Q_OS_MAC)
QString macVersionToString(QSysInfo::MacVersion version)
{
    switch (version)
    {
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

QString macVersion()
{
    return macVersionToString(QSysInfo::macVersion());
}
#endif

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

#if defined(Q_OS_WIN)
QString windowsVersionToString(QSysInfo::WinVersion version)
{
    switch (version)
    {
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

QString windowsVersion()
{
    return windowsVersionToString(QSysInfo::windowsVersion());
}
#endif

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
    switch (t)
    {
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

}
