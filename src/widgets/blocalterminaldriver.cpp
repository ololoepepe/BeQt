#include "blocalterminaldriver.h"
#include "blocalterminaldriver_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BTerminalIOHandler>

#include <QObject>
#include <QString>
#include <QProcess>
#include <QTextStream>
#include <QStringList>
#include <QObject>

BLocalTerminalDriverPrivateObject::BLocalTerminalDriverPrivateObject(BLocalTerminalDriverPrivate *p) :
    QObject(0), _m_p(p)
{
    //
}

BLocalTerminalDriverPrivateObject::~BLocalTerminalDriverPrivateObject()
{
    //
}

//

void BLocalTerminalDriverPrivateObject::finished(int exitCode)
{
    _m_p->finished(exitCode);
}

void BLocalTerminalDriverPrivateObject::readyRead()
{
    _m_p->readyRead();
}

//

BLocalTerminalDriverPrivate::BLocalTerminalDriverPrivate(BLocalTerminalDriver *q) :
    BBasePrivate(q), _m_o( new BLocalTerminalDriverPrivateObject(this) )
{
    process = new QProcess(q);
    process->setProcessChannelMode(QProcess::MergedChannels);
    QObject::connect( process, SIGNAL( finished(int) ), _m_o, SLOT( finished(int) ) );
    QObject::connect( process, SIGNAL( readyRead() ), _m_o, SLOT( readyRead() ) );
}

BLocalTerminalDriverPrivate::~BLocalTerminalDriverPrivate()
{
    _m_o->deleteLater();
}

//

void BLocalTerminalDriverPrivate::finished(int exitCode)
{
    q_func()->emitFinished(exitCode);
}

void BLocalTerminalDriverPrivate::readyRead()
{
    q_func()->emitReadyRead();
}

//

BLocalTerminalDriver::BLocalTerminalDriver(QObject *parent) :
    BAbstractTerminalDriver(parent), BBase( *new BLocalTerminalDriverPrivate(this) )
{
    //
}

BLocalTerminalDriver::~BLocalTerminalDriver()
{
    //
}

//

QString BLocalTerminalDriver::prompt() const
{
    //test
    return "$ ";
    //end test
}

bool BLocalTerminalDriver::isActive() const
{
    return d_func()->process->isOpen();
}

QString BLocalTerminalDriver::read()
{
    QTextStream in(d_func()->process);
    in.setCodec("UTF-8");
    return in.readAll();
}

bool BLocalTerminalDriver::applyCommand(const QString &command)
{
    B_D(BLocalTerminalDriver);
    if ( isActive() )
    {
        QTextStream out(d->process);
        out.setCodec("UTF-8");
        out << (command + "\n");
    }
    else
    {
        QStringList args = BTerminalIOHandler::splitCommand(command);
        if ( args.isEmpty() )
            return false;
        QString program = args.takeFirst();
        if ( program.isEmpty() )
            return false;
        d->process->start(program, args);
        if ( !d->process->waitForStarted() )
            return false;
    }
    return true;
}

void BLocalTerminalDriver::close()
{
    d_func()->process->closeWriteChannel();
}

void BLocalTerminalDriver::terminate()
{
    d_func()->process->terminate();
}

void BLocalTerminalDriver::kill()
{
    d_func()->process->kill();
}

//

BLocalTerminalDriver::BLocalTerminalDriver(BLocalTerminalDriverPrivate &d, QObject *parent) :
    BAbstractTerminalDriver(parent), BBase(d)
{
    //
}
