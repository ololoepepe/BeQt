#include "bterminaliohandler.h"
#include "bglobal.h"

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

BTerminalIOHandler *BTerminalIOHandler::_m_inst = 0;
QMutex BTerminalIOHandler::_m_instMutex;
QMutex BTerminalIOHandler::_m_stdinMutex;
QMutex BTerminalIOHandler::_m_stdoutMutex;
QMutex BTerminalIOHandler::_m_readLineMutex;
bool BTerminalIOHandler::_m_prefereReadLine = false;
QString BTerminalIOHandler::_m_lastLine;

//

QStringList BTerminalIOHandler::_m_splitCommand(const QString &command)
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

//

BTerminalIOHandler *BTerminalIOHandler::instance()
{
    if (!_m_inst)
    {
        _m_instMutex.lock();
        if (!_m_inst)
        {
            _m_inst = new BTerminalIOHandler;
            _m_inst->start();
        }
        _m_instMutex.unlock();
    }
    return _m_inst;
}

QString BTerminalIOHandler::readLine()
{
    QString line;
    QMutexLocker locker(&_m_readLineMutex);
    if ( _m_stdinMutex.tryLock() )
    {
        static QTextStream in(stdin, QIODevice::ReadOnly);
        line = in.readLine();
        _m_stdinMutex.unlock();
    }
    else
    {
        _m_prefereReadLine = true;
        _m_stdinMutex.lock();
        line = _m_lastLine;
        _m_prefereReadLine = false;
        _m_lastLine.clear();
        _m_stdinMutex.unlock();
    }
    return line;
}

void BTerminalIOHandler::write(const QString &text)
{
    static QTextStream out(stdout, QIODevice::WriteOnly);
    QMutexLocker locker(&_m_stdoutMutex);
    out << text;
    out.flush();
}

void BTerminalIOHandler::writeLine(const QString &text)
{
    write(text + "\n");
}

void BTerminalIOHandler::setStdinEchoEnabled(bool enabled)
{
#if defined(Q_OS_WIN)
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    if(enabled)
        mode |= ENABLE_ECHO_INPUT;
    else
        mode &= ~ENABLE_ECHO_INPUT;
    SetConsoleMode(hStdin, mode);
#elif defined(Q_OS_UNIX)
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if(enabled)
        tty.c_lflag |= ECHO;
    else
        tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

//

void BTerminalIOHandler::run()
{
    QTextStream in(stdin, QIODevice::ReadOnly);
    forever
    {
        _m_stdinMutex.lock();
        QString line = in.readLine();
        if (_m_prefereReadLine)
        {
            _m_lastLine = line;
        }
        else
        {
            QStringList args = _m_splitCommand(line);
            QString command = args.takeFirst();
            QMetaObject::invokeMethod( _m_inst, "commandEntered", Qt::QueuedConnection,
                                       Q_ARG(QString, command), Q_ARG(QStringList, args) );
        }
        _m_stdinMutex.unlock();
        msleep(100); //Required for readLine() to be able to lock the mutex. This is unlikely to be noticed by users
    }
}

//

BTerminalIOHandler::BTerminalIOHandler() :
    QThread(0)
{
    //
}

BTerminalIOHandler::~BTerminalIOHandler()
{
    //
}
