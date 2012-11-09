#include "bterminaliohandler.h"
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

#include <cstdio>

#if defined(B_OS_UNIX) || defined(B_OS_MAC)
#include "termios.h"
#elif defined(B_OS_WIN)
#include "windows.h"
#endif

class BTerminalIOHandlerPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTerminalIOHandler)
    B_DECLARE_PUBLIC_S(BTerminalIOHandler)
public:
    static QStringList splitCommand(const QString &command);
    //
    explicit BTerminalIOHandlerPrivate(BTerminalIOHandler *q);
    ~BTerminalIOHandlerPrivate();
    //
    static QMutex instMutex;
    static QMutex stdinMutex;
    static QMutex stdoutMutex;
    static QMutex readLineMutex;
    static QMutex echoMutex;
    static bool prefereReadLine;
    static QString lastLine;
};

//

BTerminalIOHandlerPrivate::BTerminalIOHandlerPrivate(BTerminalIOHandler *q) :
  BBasePrivate(q)
{
    prefereReadLine = false;
}

BTerminalIOHandlerPrivate::~BTerminalIOHandlerPrivate()
{
    //
}

QMutex BTerminalIOHandlerPrivate::instMutex;
QMutex BTerminalIOHandlerPrivate::stdinMutex;
QMutex BTerminalIOHandlerPrivate::stdoutMutex;
QMutex BTerminalIOHandlerPrivate::readLineMutex;
QMutex BTerminalIOHandlerPrivate::echoMutex;
bool BTerminalIOHandlerPrivate::prefereReadLine = false;
QString BTerminalIOHandlerPrivate::lastLine;

//

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

BTerminalIOHandler *BTerminalIOHandler::instance()
{
    if (!_m_self)
    {
        BTerminalIOHandlerPrivate::instMutex.lock();
        if (!_m_self)
        {
            _m_self = new BTerminalIOHandler;
            _m_self->start();
        }
        BTerminalIOHandlerPrivate::instMutex.unlock();
    }
    return _m_self;
}

QString BTerminalIOHandler::readLine()
{
    QString line;
    QMutexLocker locker(&BTerminalIOHandlerPrivate::readLineMutex);
    if ( BTerminalIOHandlerPrivate::stdinMutex.tryLock() )
    {
        static QTextStream in(stdin, QIODevice::ReadOnly);
        line = in.readLine();
        BTerminalIOHandlerPrivate::stdinMutex.unlock();
    }
    else
    {
        BTerminalIOHandlerPrivate::prefereReadLine = true;
        BTerminalIOHandlerPrivate::stdinMutex.lock();
        line = BTerminalIOHandlerPrivate::lastLine;
        BTerminalIOHandlerPrivate::prefereReadLine = false;
        BTerminalIOHandlerPrivate::lastLine.clear();
        BTerminalIOHandlerPrivate::stdinMutex.unlock();
    }
    return line;
}

void BTerminalIOHandler::write(const QString &text)
{
    static QTextStream out(stdout, QIODevice::WriteOnly);
    QMutexLocker locker(&BTerminalIOHandlerPrivate::stdoutMutex);
    out << text;
    out.flush();
}

void BTerminalIOHandler::writeLine(const QString &text)
{
    write(text + "\n");
}

void BTerminalIOHandler::setStdinEchoEnabled(bool enabled)
{
    QMutexLocker locker(&BTerminalIOHandlerPrivate::echoMutex);
#if defined(B_OS_MAC) || defined(B_OS_UNIX)
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if(enabled)
        tty.c_lflag |= ECHO;
    else
        tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#elif defined(B_OS_WIN)
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

//

void BTerminalIOHandler::run()
{
    QTextStream in(stdin, QIODevice::ReadOnly);
    forever
    {
        BTerminalIOHandlerPrivate::stdinMutex.lock();
        QString line = in.readLine();
        if (BTerminalIOHandlerPrivate::prefereReadLine)
        {
            BTerminalIOHandlerPrivate::lastLine = line;
        }
        else
        {
            QStringList args = splitCommand(line);
            QString command = args.takeFirst();
            QMetaObject::invokeMethod( _m_self, "commandEntered", Qt::QueuedConnection,
                                       Q_ARG(QString, command), Q_ARG(QStringList, args) );
        }
        BTerminalIOHandlerPrivate::stdinMutex.unlock();
        msleep(100); //Required for readLine() to be able to lock the mutex. This is unlikely to be noticed by users
    }
}

//

BTerminalIOHandler *BTerminalIOHandler::_m_self = 0;

//

BTerminalIOHandler::BTerminalIOHandler() :
    QThread(0), BBase( *new BTerminalIOHandlerPrivate(this) )
{
    //
}

BTerminalIOHandler::~BTerminalIOHandler()
{
    //
}
