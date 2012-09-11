#include "bstdio.h"

#include <QTextStream>
#include <QIODevice>
#include <QThread>
#include <QString>
#include <QMetaObject>
#include <QStringList>
#include <QChar>

#include <cstdio>

#if defined(Q_OS_WIN)
#include "windows.h"
#else
#include "termios.h"
#endif

BStdIO *BStdIO::_m_instance = 0;

//

BStdIO *BStdIO::instance()
{
    if (!_m_instance)
    {
        _m_instance = new BStdIO;
        _m_instance->start();
    }
    return _m_instance;
}

QString BStdIO::readLine()
{
    static QTextStream in(stdin, QIODevice::ReadOnly);
    return in.readLine();
}

void BStdIO::write(const QString &text)
{
    static QTextStream out(stdout, QIODevice::WriteOnly);
    out << text;
    out.flush();
}

void BStdIO::writeLine(const QString &text)
{
    write(text + "\n");
}

QStringList BStdIO::splitCommand(const QString &command)
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

void BStdIO::setStdinEchoEnabled(bool enabled)
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
#else
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

void BStdIO::writeSlot(const QString &text)
{
    write(text);
}

//

BStdIO::BStdIO() :
    QThread(0)
{
}

//

void BStdIO::run()
{
    QTextStream in(stdin, QIODevice::ReadOnly);
    forever
        QMetaObject::invokeMethod( instance(), "read", Qt::QueuedConnection, Q_ARG( QString, in.readLine() ) );
}
