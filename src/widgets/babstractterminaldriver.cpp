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

void BAbstractTerminalDriver::terminate()
{
    close();
}

void BAbstractTerminalDriver::kill()
{
    terminate();
}

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

bool BAbstractTerminalDriver::processCommand(const QString &, const QStringList &, QString &)
{
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
    Q_EMIT readyRead();
}

void BAbstractTerminalDriver::emitFinished(int exitCode)
{
    Q_EMIT finished(exitCode);
}

void BAbstractTerminalDriver::emitBlockTerminal()
{
    Q_EMIT blockTerminal();
}

void BAbstractTerminalDriver::emitUnblockTerminal()
{
    Q_EMIT unblockTerminal();
}
