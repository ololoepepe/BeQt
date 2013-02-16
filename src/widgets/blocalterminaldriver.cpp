#include "blocalterminaldriver.h"

#include <BeQtCore/BeQt>
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
#include <QTextCodec>

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
    QTextCodec *codec;
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
    codec = QTextCodec::codecForLocale();
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

bool BLocalTerminalDriver::isActive() const
{
    return d_func()->process->isOpen();
}

QString BLocalTerminalDriver::read()
{
    QTextStream in(d_func()->process);
    if (d_func()->codec)
        in.setCodec(d_func()->codec);
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
    BeQt::startProcess(d->process, command, arguments);
    if ( !d->process->waitForStarted() )
    {
        d->process->close();
        error = tr("Could not find or start programm", "terminalCommand return");
        return false;
    }
    return true;
}

bool BLocalTerminalDriver::processCommand(const QString &command, const QStringList &arguments, QString &error)
{
    if ( !isActive() )
    {
        error = tr("No process is running", "processCommand return");
        return false;
    }
    QTextStream out(d_func()->process);
    if (d_func()->codec)
        out.setCodec(d_func()->codec);
    QString cmd = command;
    foreach (const QString &arg, arguments)
        cmd += " " + (arg.contains(" ") ? ("\"" + arg + "\"") : arg);
    cmd += "\n";
    out << (cmd);
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

void BLocalTerminalDriver::setCodec(QTextCodec *codec)
{
    d_func()->codec = codec;
}

void BLocalTerminalDriver::setCodec(const QString &codecName)
{
    d_func()->codec = QTextCodec::codecForName(codecName.toLatin1());
}

QTextCodec *BLocalTerminalDriver::codec() const
{
    return d_func()->codec;
}

QString BLocalTerminalDriver::codecName() const
{
    return QString::fromLatin1(d_func()->codec->name());
}
