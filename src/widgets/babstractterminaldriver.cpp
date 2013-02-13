#include "babstractterminaldriver.h"

#include <QObject>
#include <QMetaObject>

/*============================================================================
================================ BAbstractTerminalDriver =====================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractTerminalDriver::BAbstractTerminalDriver(QObject *parent) :
    QObject(parent)
{
    //
}

BAbstractTerminalDriver::~BAbstractTerminalDriver()
{
    //
}

/*============================== Public methods ============================*/

QString BAbstractTerminalDriver::prompt() const
{
    return "$";
}

bool BAbstractTerminalDriver::terminalCommand(const QString &, const QStringList &, QString &)
{
    emitFinished(0);
    return true;
}

bool BAbstractTerminalDriver::terminalCommand(const QVariant &, QString &)
{
    emitFinished(0);
    return true;
}

void BAbstractTerminalDriver::setWorkingDirectory(const QString &)
{
    //
}

QString BAbstractTerminalDriver::workingDirectory() const
{
    return "";
}

/*============================== Protected slots ===========================*/

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
