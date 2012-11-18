#include "babstractterminaldriver.h"

#include <QObject>
#include <QMetaObject>

/*============================================================================
================================ Abstract Terminal Driver
============================================================================*/

BAbstractTerminalDriver::BAbstractTerminalDriver(QObject *parent) :
    QObject(parent)
{
    //
}

BAbstractTerminalDriver::~BAbstractTerminalDriver()
{
    //
}

//

QString BAbstractTerminalDriver::prompt() const
{
    return "$";
}

QString BAbstractTerminalDriver::terminalCommand(const QString &command, const QStringList &arguments,
                                                 bool *finished, int *exitCode)
{
    if (finished)
        *finished = true;
    if (exitCode)
        *exitCode = 0;
    return "";
}

void BAbstractTerminalDriver::setWorkingDirectory(const QString &path)
{
    //
}

QString BAbstractTerminalDriver::workingDirectory() const
{
    return "";
}

//

void BAbstractTerminalDriver::emitReadyRead()
{
    emit readyRead();
}

void BAbstractTerminalDriver::emitFinished(int exitCode)
{
    emit finished(exitCode);
}

void BAbstractTerminalDriver::emitBlockTerminal()
{
    emit blockTerminal();
}

void BAbstractTerminalDriver::emitUnblockTerminal()
{
    emit unblockTerminal();
}
