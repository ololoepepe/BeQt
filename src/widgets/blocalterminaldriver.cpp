#include "blocalterminaldriver.h"

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
#include <QMap>

#include <QDebug>

/*============================================================================
================================ BLocalTerminalDriverPrivate =================
============================================================================*/

class BLocalTerminalDriverPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BLocalTerminalDriver)
public:
    explicit BLocalTerminalDriverPrivate(BLocalTerminalDriver *q);
    ~BLocalTerminalDriverPrivate();
public:
    void init();
public:
    QProcess *process;
    QString workingDirectory;
private:
    Q_DISABLE_COPY(BLocalTerminalDriverPrivate)
};

/*============================================================================
================================ BLocalTerminalDriverPrivate =================
============================================================================*/

/*============================== Public constructors =======================*/

BLocalTerminalDriverPrivate::BLocalTerminalDriverPrivate(BLocalTerminalDriver *q) :
    BBasePrivate(q)
{
    //
}

BLocalTerminalDriverPrivate::~BLocalTerminalDriverPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BLocalTerminalDriverPrivate::init()
{
    B_Q(BLocalTerminalDriver);
    process = new QProcess(this);
    workingDirectory = QDir::homePath();
    process->setProcessChannelMode(QProcess::MergedChannels);
    connect( process, SIGNAL( finished(int) ), q, SLOT( emitFinished(int) ) );
    connect( process, SIGNAL( readyRead() ), q, SLOT( emitReadyRead() ) );
}

/*============================================================================
================================ BLocalTerminalDriver ========================
============================================================================*/

/*============================== Public constructors =======================*/

BLocalTerminalDriver::BLocalTerminalDriver(QObject *parent) :
    BAbstractTerminalDriver(parent), BBase( *new BLocalTerminalDriverPrivate(this) )
{
    d_func()->init();
}

BLocalTerminalDriver::~BLocalTerminalDriver()
{
    //
}

/*============================== Protected constructors ====================*/

BLocalTerminalDriver::BLocalTerminalDriver(BLocalTerminalDriverPrivate &d, QObject *parent) :
    BAbstractTerminalDriver(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

bool BLocalTerminalDriver::processCommand(const QString &command, const QStringList &arguments, QString &error)
{
    if ( !isActive() )
    {
        error = tr("No process is running", "processCommand return");
        return false;
    }
    QTextStream out(d_func()->process);
    out.setCodec("UTF-8");
    QString cmd = command;
    foreach (const QString &arg, arguments)
        cmd += " " + (arg.contains(" ") ? ("\"" + arg + "\"") : arg);
    cmd += "\n";
    out << (cmd);
    return true;
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

bool BLocalTerminalDriver::terminalCommand(const QString &command, const QStringList &arguments, QString &error)
{
    if ( isActive() )
    {
        error = tr("Another process is running", "terminalCommand return");
        return false;
    }
    B_D(BLocalTerminalDriver);
    //TODO (release 3.0.0): Improve standard command handling
    //May not work properly
    if ( !command.compare("cd", Qt::CaseInsensitive) && !arguments.isEmpty() && QDir( arguments.first() ).exists() )
    {
        d->workingDirectory = arguments.first();
        emitFinished(0);
        return true;
    }
    //end test
    d->process->setWorkingDirectory(d->workingDirectory);
    //Workaround to handle long arguments on Windows
#if defined(Q_OS_WIN)
    d->process->setNativeArguments(BTerminalIOHandler::mergeArguments(arguments));
    d->process->start(command);
#else
    d->process->start(command, arguments);
#endif
    //End of the workaround
    if ( !d->process->waitForStarted() )
    {
        d->process->close();
        error = tr("Could not find or start programm", "terminalCommand return");
        return false;
    }
    return true;
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
