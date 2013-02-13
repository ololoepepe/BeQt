#include "bnamespace.h"
#include "bterminaliohandler.h"

#include <QEventLoop>
#include <QTimer>
#include <QUuid>
#include <QString>
#include <QProcess>
#include <QStringList>

namespace BeQt
{

void waitNonBlocking(int msecs)
{
    if (msecs <= 0)
        return;
    QEventLoop el;
    QTimer::singleShot(msecs, &el, SLOT(quit()));
    el.exec();
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

}
