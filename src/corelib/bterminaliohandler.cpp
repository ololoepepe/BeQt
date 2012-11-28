#include "bterminaliohandler.h"
#include "bterminaliohandler_p.h"
#include "bglobal.h"
#include "bbase_p.h"

#include <QTextStream>
#include <QIODevice>
#include <QThread>
#include <QString>
#include <QMetaObject>
#include <QStringList>
#include <QChar>
#include <QMutex>
#include <QMutexLocker>
#include <QFile>

#include <QDebug>

#include <cstdio>

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
#include "termios.h"
#elif defined(Q_OS_WIN)
#include "windows.h"
#endif

/*============================================================================
================================ BTerminalIOHandlerThread ====================
============================================================================*/

/*============================== Public constructors =======================*/

BTerminalIOHandlerThread::BTerminalIOHandlerThread(BTerminalIOHandlerPrivate *p) :
    QThread(p), _m_p(p), readStream(stdin, QIODevice::ReadOnly)
{
    //
}

BTerminalIOHandlerThread::~BTerminalIOHandlerThread()
{
    //
}

/*============================== Protected methods =========================*/

void BTerminalIOHandlerThread::run()
{
    QMutexLocker locker(&readMutex);
    forever
    {
        _m_p->lineRead( readStream.readLine() );
    }
}

/*============================== Static public variables ===================*/

QMutex BTerminalIOHandlerThread::readMutex;

/*============================================================================
================================ BTerminalIOHandlerPrivate
============================================================================*/

/*============================== Public constructors =======================*/

BTerminalIOHandlerPrivate::BTerminalIOHandlerPrivate(BTerminalIOHandler *q) :
    BBasePrivate(q), Thread( new BTerminalIOHandlerThread(this) )
{
    //
}

BTerminalIOHandlerPrivate::~BTerminalIOHandlerPrivate()
{
    //
}

/*============================== Static public methods =====================*/

bool BTerminalIOHandlerPrivate::testInit(const char *where)
{
    const char *w = where ? where : "BTerminalIOHandler";
    B_QS(BTerminalIOHandler);
    return bTest(qs, w, "There must be a BTerminalIOHandler instance");
}

bool BTerminalIOHandlerPrivate::testUnique()
{
    return bTest(!qs_func(), "BTerminalIOHandler", "There must be only one instance of BTerminalIOHandler");
}

/*============================== Public methods ============================*/

void BTerminalIOHandlerPrivate::init()
{
    Thread->start();
}

void BTerminalIOHandlerPrivate::lineRead(const QString &text)
{
    QMutexLocker locker(&loopMutex);
    if ( readEventLoop.isRunning() )
    {
        lastLine = text;
        readEventLoop.quit();
    }
    else
    {
        QStringList args = BTerminalIOHandler::splitCommand(text);
        QString cmd = !args.isEmpty() ? args.first() : QString();
        q_func()->handleCommand(cmd, args);
    }
}

/*============================== Static public variables ===================*/

QMutex BTerminalIOHandlerPrivate::echoMutex;
QMutex BTerminalIOHandlerPrivate::readMutex;
QMutex BTerminalIOHandlerPrivate::writeMutex;
QTextStream BTerminalIOHandlerPrivate::writeStream(stdout, QIODevice::WriteOnly);

/*============================================================================
================================ BTerminalIOHandler ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BTerminalIOHandler::BTerminalIOHandler(QObject *parent) :
    QObject(parent), BBase( *new BTerminalIOHandlerPrivate(this) )
{
    d_func()->init();
    BTerminalIOHandlerPrivate::testUnique();
    _m_self = this;
}

BTerminalIOHandler::~BTerminalIOHandler()
{
    _m_self = 0;
}


/*============================== Protected constructors ====================*/

BTerminalIOHandler::BTerminalIOHandler(BTerminalIOHandlerPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

BTerminalIOHandler *BTerminalIOHandler::instance()
{
    return _m_self;
}

QStringList BTerminalIOHandler::splitCommand(const QString &command)
{
    QStringList args;
    QString arg;
    bool quot = false;
    for (int i = 0; i < command.length(); ++i)
    {
        const QChar &c = command.at(i);
        if ( c.isSpace() )
        {
            if (quot)
            {
                arg.append(c);
            }
            else if ( !arg.isEmpty() )
            {
                args << arg;
                arg.clear();
            }
        }
        else
        {
            if ( c == '\"' && (i < 1 || command.at(i - 1) != '\\') )
                quot = !quot;
            else
                arg.append(c);
        }
    }
    if (!arg.isEmpty() && !quot)
        args << arg;
    if ( !args.isEmpty() )
        args.first().toLower();
    return args;
}

QString BTerminalIOHandler::readLine()
{
    QMutexLocker locker(&BTerminalIOHandlerPrivate::readMutex);
    ds_func()->readEventLoop.exec();
    return ds_func()->lastLine;
}

void BTerminalIOHandler::write(const QString &text)
{
    QMutexLocker locker(&BTerminalIOHandlerPrivate::writeMutex);
    BTerminalIOHandlerPrivate::writeStream << text;
    BTerminalIOHandlerPrivate::writeStream.flush();
}

void BTerminalIOHandler::writeLine(const QString &text)
{
    write(text + "\n");
}

void BTerminalIOHandler::setStdinEchoEnabled(bool enabled)
{
    QMutexLocker locker(&BTerminalIOHandlerPrivate::echoMutex);
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if(enabled)
        tty.c_lflag |= ECHO;
    else
        tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#elif defined(Q_OS_WIN)
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    if(enabled)
        mode |= ENABLE_ECHO_INPUT;
    else
        mode &= ~ENABLE_ECHO_INPUT;
    SetConsoleMode(hStdin, mode);
#endif
}

/*============================== Protected methods =========================*/

void BTerminalIOHandler::handleCommand(const QString &command, const QStringList &arguments)
{
    emit commandEntered(command, arguments);
}

/*============================== Static protected variables ================*/

BTerminalIOHandler *BTerminalIOHandler::_m_self = 0;
