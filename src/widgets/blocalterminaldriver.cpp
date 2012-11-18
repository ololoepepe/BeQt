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
#include <QDir>

#include <QDebug>

/*============================================================================
================================ Local Terminal Driver Private Object
============================================================================*/

BLocalTerminalDriverPrivateObject::BLocalTerminalDriverPrivateObject(BLocalTerminalDriverPrivate *p) :
    BBasePrivateObject(p)
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
    p_func()->finished(exitCode);
}

void BLocalTerminalDriverPrivateObject::readyRead()
{
    p_func()->readyRead();
}

/*============================================================================
================================ Local Terminal Driver Private
============================================================================*/

BLocalTerminalDriverPrivate::BLocalTerminalDriverPrivate(BLocalTerminalDriver *q) :
    BBasePrivate( *q, *new BLocalTerminalDriverPrivateObject(this) )
{
    process = new QProcess(q);
    workingDirectory = QDir::homePath();
    process->setProcessChannelMode(QProcess::MergedChannels);
    QObject::connect( process, SIGNAL( finished(int) ), o_func(), SLOT( finished(int) ) );
    QObject::connect( process, SIGNAL( readyRead() ), o_func(), SLOT( readyRead() ) );
}

BLocalTerminalDriverPrivate::~BLocalTerminalDriverPrivate()
{
    //
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

BLocalTerminalDriverPrivate::BLocalTerminalDriverPrivate(BLocalTerminalDriver &q,
                                                         BLocalTerminalDriverPrivateObject &o) :
    BBasePrivate(q, o)
{
    //
}

/*============================================================================
================================ Local Terminal Driver
============================================================================*/

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

QString BLocalTerminalDriver::processCommand(const QString &command, const QStringList &arguments)
{
    if ( !isActive() )
        return tr("No process is running", "processCommand return");
    QTextStream out(d_func()->process);
    out.setCodec("UTF-8");
    QString cmd = command;
    foreach (const QString &arg, arguments)
        cmd += " " + (arg.contains(" ") ? ("\"" + arg + "\"") : arg);
    cmd += "\n";
    out << (cmd);
    return "";
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

void BLocalTerminalDriver::close()
{
    d_func()->process->close();
}

void BLocalTerminalDriver::terminate()
{
    d_func()->process->terminate();
}

void BLocalTerminalDriver::kill()
{
    d_func()->process->kill();
}

QString BLocalTerminalDriver::prompt() const
{
    return d_func()->workingDirectory + "$ ";
}

QString BLocalTerminalDriver::terminalCommand(const QString &command, const QStringList &arguments,
                                              bool *finished, int *exitCode)
{
    if (finished)
        *finished = true;
    if (exitCode)
        *exitCode = 0;
    if ( isActive() )
        return tr("A process is running", "terminalCommand return");
    B_D(BLocalTerminalDriver);
    //test
    if ( !command.compare("cd", Qt::CaseInsensitive) && !arguments.isEmpty() && QDir( arguments.first() ).exists() )
    {
        d->workingDirectory = arguments.first();
        return "";
    }
    //end test
    d->process->setWorkingDirectory(d->workingDirectory);
    d->process->start(command, arguments);
    if ( d->process->waitForStarted() )
    {
        if (finished)
            *finished = false;
        return "";
    }
    else
    {
        d->process->close();
        return tr("Could not find or start programm", "terminalCommand return");
    }
}

void BLocalTerminalDriver::setWorkingDirectory(const QString &path)
{
    if ( isActive() )
        return;
    d_func()->workingDirectory = !path.isEmpty() ? path : QDir::homePath();
}

QString BLocalTerminalDriver::workingDirectory() const
{
    return d_func()->workingDirectory;
}

//

BLocalTerminalDriver::BLocalTerminalDriver(BLocalTerminalDriverPrivate &d, QObject *parent) :
    BAbstractTerminalDriver(parent), BBase(d)
{
    //
}
