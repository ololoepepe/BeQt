#include "bterminaliohandler.h"
#include "bterminaliohandler_p.h"
#include "bglobal.h"
#include "bbase_p.h"
#include "bnamespace.h"

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
    forever
        _m_p->lineRead( readStream.readLine() );
}

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
    Thread->terminate();
    Thread->wait();
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
    if (readEventLoop.isRunning())
    {
        lastLine = text;
        readEventLoop.quit();
    }
    else
    {
        QStringList args = BTerminalIOHandler::splitCommand(text);
        QString cmd = !args.isEmpty() ? args.takeFirst() : QString();
        if (cmd.isEmpty())
            return;
        B_Q(BTerminalIOHandler);
        QMetaObject::invokeMethod(q, "commandEntered", Q_ARG(QString, cmd), Q_ARG(QStringList, args));
        if (internalHandlers.contains(cmd))
        {
            QMetaObject::invokeMethod(this, "executeInternalHandler", Qt::QueuedConnection, Q_ARG(QString, cmd),
                                      Q_ARG(QStringList, args));
            return;
        }
        if (externalHandlers.contains(cmd))
        {
            QMetaObject::invokeMethod(this, "executeExternalHandler", Qt::QueuedConnection, Q_ARG(QString, cmd),
                                      Q_ARG(QStringList, args));
            return;
        }
        QMetaObject::invokeMethod(this, "executeHandleCommand", Qt::QueuedConnection, Q_ARG(QString, cmd),
                                  Q_ARG(QStringList, args));
    }
}

/*============================== Public slots ==============================*/

void BTerminalIOHandlerPrivate::executeInternalHandler(const QString &cmd, const QStringList &args)
{
    if (!internalHandlers.contains(cmd))
        return;
    (q_func()->*internalHandlers.value(cmd))(cmd, args);
}

void BTerminalIOHandlerPrivate::executeExternalHandler(const QString &cmd, const QStringList &args)
{
    if (!externalHandlers.contains(cmd))
        return;
    externalHandlers.value(cmd)(cmd, args);
}

void BTerminalIOHandlerPrivate::executeHandleCommand(const QString &cmd, const QStringList &args)
{
    q_func()->handleCommand(cmd, args);
}

/*============================== Static public variables ===================*/

QMutex BTerminalIOHandlerPrivate::echoMutex;
QMutex BTerminalIOHandlerPrivate::readMutex;
QMutex BTerminalIOHandlerPrivate::writeMutex;
QMutex BTerminalIOHandlerPrivate::writeErrMutex;
QTextStream BTerminalIOHandlerPrivate::writeStream(stdout, QIODevice::WriteOnly);
QTextStream BTerminalIOHandlerPrivate::writeErrStream(stderr, QIODevice::WriteOnly);

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
    BTerminalIOHandlerPrivate::testUnique();
    _m_self = this;
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

QString BTerminalIOHandler::mergeArguments(const QStringList &arguments)
{
    QString args;
    foreach (const QString &a, arguments)
        args += (a.contains(' ') ? BeQt::wrapped(a) : a) + " ";
    if ( !args.isEmpty() )
        args.remove(args.length() - 1, 1);
    return args;
}

QString BTerminalIOHandler::readLine(const QString &text)
{
    if (!text.isEmpty())
        write(text);
    QMutexLocker locker(&BTerminalIOHandlerPrivate::readMutex);
    ds_func()->readEventLoop.exec();
    return ds_func()->lastLine;
}

void BTerminalIOHandler::write(const QString &text)
{
    if (text.isEmpty())
        return;
    QMutexLocker locker(&BTerminalIOHandlerPrivate::writeMutex);
    BTerminalIOHandlerPrivate::writeStream << text;
    BTerminalIOHandlerPrivate::writeStream.flush();
}

void BTerminalIOHandler::writeLine(const QString &text)
{
    write(text + "\n");
}

void BTerminalIOHandler::writeErr(const QString &text)
{
    QMutexLocker locker(&BTerminalIOHandlerPrivate::writeErrMutex);
    BTerminalIOHandlerPrivate::writeErrStream << text;
    BTerminalIOHandlerPrivate::writeErrStream.flush();
}

void BTerminalIOHandler::writeLineErr(const QString &text)
{
    writeErr(text + "\n");
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

void BTerminalIOHandler::installHandler(const QString &command, InternalHandler handler)
{
    if ( !BTerminalIOHandlerPrivate::testInit() )
        return;
    if (command.isEmpty() || !handler)
        return;
    B_DS(BTerminalIOHandler);
    if ( ds->internalHandlers.contains(command) )
        return;
    ds->internalHandlers.insert(command, handler);
}

void BTerminalIOHandler::installHandler(const QString &command, ExternalHandler handler)
{
    if ( !BTerminalIOHandlerPrivate::testInit() )
        return;
    if (command.isEmpty() || !handler)
        return;
    B_DS(BTerminalIOHandler);
    if ( ds->externalHandlers.contains(command) )
        return;
    ds->externalHandlers.insert(command, handler);
}

/*============================== Protected methods =========================*/

void BTerminalIOHandler::handleCommand(const QString &, const QStringList &)
{
    //
}

/*============================== Static protected variables ================*/

BTerminalIOHandler *BTerminalIOHandler::_m_self = 0;
