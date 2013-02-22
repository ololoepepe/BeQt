#include "bnamespace.h"
#include "bterminaliohandler.h"

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

}
