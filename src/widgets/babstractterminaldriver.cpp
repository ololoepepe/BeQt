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

void BAbstractTerminalDriver::emitReadyRead()
{
    emit readyRead();
}

void BAbstractTerminalDriver::emitFinished(int exitCode)
{
    emit finished(exitCode);
}
