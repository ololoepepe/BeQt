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
#include <QCoreApplication>

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

BTerminalIOHandlerThread::BTerminalIOHandlerThread() :
    QThread(0), readStream(stdin, QIODevice::ReadOnly)
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
    {
        QString l = readStream.readLine();
        bool b = readLoop.isRunning();
        if (b)
        {
            lastLine = l;
            readLoop.quit();
        }
        else
        {
            Q_EMIT lineRead(l);
        }
    }
}

/*============================================================================
================================ BTerminalIOHandlerPrivate
============================================================================*/

/*============================== Public constructors =======================*/

BTerminalIOHandlerPrivate::BTerminalIOHandlerPrivate(BTerminalIOHandler *q) :
    BBasePrivate(q)
{
    //
}

BTerminalIOHandlerPrivate::~BTerminalIOHandlerPrivate()
{
    removeThread();
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

BTerminalIOHandlerThread *BTerminalIOHandlerPrivate::initThread(bool silent)
{
    if (readThread)
        return readThread;
    QMutexLocker locker(&threadMutex);
    if (readThread)
        return readThread;
    if (silent && !QCoreApplication::instance())
        return 0;
    if (!silent && !bTest(QCoreApplication::instance(), "BTerminalIOHandlerPrivate",
                          "There must be a QCoreApplication instance"))
        return 0;
    readThread = new BTerminalIOHandlerThread;
    readThread->start();
    return readThread;
}

void BTerminalIOHandlerPrivate::removeThread()
{
    if (!readThread)
        return;
    threadMutex.lock();
    if (readThread)
    {
        readThread->terminate();
        readThread->wait();
        delete readThread;
        readThread = 0;
    }
    threadMutex.unlock();
}

/*============================== Public methods ============================*/

void BTerminalIOHandlerPrivate::init()
{
    BTerminalIOHandlerThread *t = initThread();
    if (!t)
        return;
    t->disconnect(SIGNAL(lineRead(QString)));
    connect(t, SIGNAL(lineRead(QString)), this, SLOT(lineRead(QString)), Qt::QueuedConnection);
}

/*============================== Public slots ==============================*/

void BTerminalIOHandlerPrivate::lineRead(const QString &text)
{
    QStringList args = BTerminalIOHandler::splitCommand(text);
    QString cmd = !args.isEmpty() ? args.takeFirst() : QString();
    if (cmd.isEmpty())
        return;
    B_Q(BTerminalIOHandler);
    QMetaObject::invokeMethod(q, "commandEntered", Q_ARG(QString, cmd), Q_ARG(QStringList, args));
    if (internalHandlers.contains(cmd))
        return (q->*internalHandlers.value(cmd))(cmd, args);
    else if (externalHandlers.contains(cmd))
        return externalHandlers.value(cmd)(cmd, args);
    else
        q->handleCommand(cmd, args);
}

/*============================== Static public variables ===================*/

QMutex BTerminalIOHandlerPrivate::echoMutex;
QMutex BTerminalIOHandlerPrivate::readMutex;
QMutex BTerminalIOHandlerPrivate::writeMutex;
QMutex BTerminalIOHandlerPrivate::writeErrMutex;
QTextStream BTerminalIOHandlerPrivate::writeStream(stdout, QIODevice::WriteOnly);
QTextStream BTerminalIOHandlerPrivate::writeErrStream(stderr, QIODevice::WriteOnly);
BTerminalIOHandlerThread *BTerminalIOHandlerPrivate::readThread = 0;
QMutex BTerminalIOHandlerPrivate::threadMutex;

/*============================================================================
================================ BTerminalIOHandler ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BTerminalIOHandler::BTerminalIOHandler(QObject *parent) :
    QObject(parent), BBase(*new BTerminalIOHandlerPrivate(this))
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
    BTerminalIOHandlerThread *t = BTerminalIOHandlerPrivate::initThread(true);
    QMutexLocker locker(&BTerminalIOHandlerPrivate::readMutex);
    QString line;
    if (t)
    {
        t->readLoop.exec();
        line = t->lastLine;
    }
    else
    {
        static QTextStream in(stdin, QIODevice::ReadOnly);
        line = in.readLine();
    }
    return line;
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
    if (ds->internalHandlers.contains(command))
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
    if (ds->externalHandlers.contains(command))
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
