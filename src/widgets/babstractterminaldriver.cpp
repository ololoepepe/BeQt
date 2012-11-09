#include "babstractterminaldriver.h"

#include <QObject>

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

QString BAbstractTerminalDriver::terminalCommand(const QString &command, const QStringList &arguments)
{
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
