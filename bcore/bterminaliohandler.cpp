#include "bterminaliohandler.h"

#include <QTextStream>
#include <QIODevice>
#include <QThread>
#include <QString>
#include <QMetaObject>
#include <QStringList>
#include <QChar>
#include <QMutex>

#include <cstdio>

#if defined(Q_OS_WIN)
#include "windows.h"
#else
#include "termios.h"
#endif

BTerminalIOHandler *inst = 0;
QMutex mutex;

//

QStringList splitCommand(const QString &command)
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
    if (!inst)
    {
        mutex.lock();
        if (!inst)
        {
            inst = new BTerminalIOHandler;
            inst->start();
        }
        mutex.unlock();
    }
    return inst;
}

void BTerminalIOHandler::write(const QString &text)
{
    static QTextStream out(stdout, QIODevice::WriteOnly);
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
        QStringList args = splitCommand( in.readLine() );
        QString command = args.takeFirst();
        QMetaObject::invokeMethod( inst, "commandEntered", Qt::QueuedConnection,
                                   Q_ARG(QString, command), Q_ARG(QStringList, args) );
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
