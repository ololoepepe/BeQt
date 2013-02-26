#include "bnamespace.h"
#include "bterminaliohandler.h"
#include "bdirtools.h"

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

namespace BeQt
{

void waitNonBlocking(int msecs)
{
    waitNonBlocking(QList<Until>(), msecs);
}

void waitNonBlocking(QObject *sender, const char *signal, int msecs)
{
    waitNonBlocking(QList<Until>() << until(sender, signal), msecs);
}

void waitNonBlocking(QObject *sender1, const char *signal1, QObject *sender2, const char *signal2, int msecs)
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

Until until(QObject *object, const char *signal)
{
    Until p;
    p.first = object;
    p.second = signal;
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

QString wrapped(const QString &text, const QString &wrappingText)
{
    if (text.isEmpty() || wrappingText.isEmpty())
        return "";
    int wl = wrappingText.length();
    QString ntext = text;
    if (text.left(wl) != wrappingText)
        ntext.prepend(wrappingText);
    if (text.right(wl) != wrappingText)
        ntext.append(wrappingText);
    return ntext;
}

QString unwrapped(const QString &text, const QString &wrappingText)
{
    if (text.isEmpty() || wrappingText.isEmpty())
        return "";
    int wl = wrappingText.length();
    QString ntext = text;
    if (ntext.left(wl) == wrappingText)
        ntext.remove(0, wl);
    if (ntext.right(wl) == wrappingText)
        ntext.remove(ntext.length() - wl, wl);
    return ntext;
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
    return execProcess("", command, arguments, startTimeout, finishTimeout, output, codec);
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

QString macVersionString()
{
    return macVersionToString(QSysInfo::macVersion());
}
#endif

#if defined(Q_OS_LINUX)
QString linuxVersionString()
{
    bool ok = false;
    QStringList sl = BDirTools::readTextFile("/etc/lsb-release", "Latin-1", &ok).split('\n', QString::SkipEmptyParts);
    if (!ok || sl.isEmpty())
        return "Unknown";
    sl = sl.last().split('=', QString::SkipEmptyParts);
    return (sl.size() == 2) ? unwrapped(sl.last()) : QString("Unknown");
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

QString windowsVersionString()
{
    return windowsVersionToString(QSysInfo::windowsVersion());
}
#endif

QString osVersionString()
{
#if defined(Q_OS_MAC)
    return macVersionString();
#elif defined(Q_OS_LINUX)
    return linuxVersionString();
#elif defined(Q_OS_WIN)
    return windowsVersionString();
#endif
}

}
