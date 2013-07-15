#include "bterminaliohandler.h"
#include "bterminaliohandler_p.h"
#include "bglobal.h"
#include "bbase_p.h"
#include "bnamespace.h"
#include "bsettingsnode.h"

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
    delete root;
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
    root = 0;
    translations = true;
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
        (q->*internalHandlers.value(cmd))(cmd, args);
    else if (externalHandlers.contains(cmd))
        externalHandlers.value(cmd)(q, cmd, args);
    else
        q->handleCommand(cmd, args);
}

/*============================== Static public variables ===================*/

QMutex BTerminalIOHandlerPrivate::echoMutex;
QMutex BTerminalIOHandlerPrivate::titleMutex;
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

QString BTerminalIOHandler::command(StandardCommand cmd)
{
    QStringList sl = commands(cmd);
    return !sl.isEmpty() ? sl.first() : QString();
}

QStringList BTerminalIOHandler::commands(StandardCommand cmd)
{
    QStringList sl;
    switch (cmd)
    {
    case QuitCommand:
        sl << "quit" << "exit";
        break;
    case SetCommand:
        sl << "set";
        break;
    case HelpCommand:
        sl << "help";
        break;
    default:
        break;
    }
    return sl;
}

BTerminalIOHandler::InternalHandler BTerminalIOHandler::handler(StandardCommand cmd)
{
    switch (cmd)
    {
    case QuitCommand:
        return &BTerminalIOHandler::handleQuit;
    case SetCommand:
        return &BTerminalIOHandler::handleSet;
    case HelpCommand:
        return &BTerminalIOHandler::handleHelp;
    default:
        return 0;
    }
}

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

void BTerminalIOHandler::writeHelpLine(const QString &usage, const QString &description)
{
    if (usage.isEmpty())
        return;
    QString s = "  " + usage;
    if (!description.isEmpty())
    {
        if (s.length() > 28)
            s += "\n" + QString().fill(' ', 30);
        else
            s += QString().fill(' ', 30 - s.length());
        s += description;
    }
    writeLine(s);
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

void BTerminalIOHandler::setTerminalTitle(const QString &title)
{
    QMutexLocker locker1(&BTerminalIOHandlerPrivate::titleMutex);
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    QMutexLocker locker2(&BTerminalIOHandlerPrivate::writeMutex);
    BTerminalIOHandlerPrivate::writeStream << QString("%1]0;%3%2").arg("\033", "\007", title);
    BTerminalIOHandlerPrivate::writeStream.flush();
#elif defined(Q_OS_WINDOWS)

    if (sizeof(TCHAR) == > 1)
    {
        LPCTSTR s = new TCHAR[title.length() + 1];
        title.toWCharArray(s);
        s[title.length()] = '\0';
        SetConsoleTitle(s);
        delete [] s;
    }
    else
    {
        SetConsoleTitle(title.toLocal8Bit().constData());
    }
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

void BTerminalIOHandler::installHandler(StandardCommand cmd)
{
    InternalHandler h = handler(cmd);
    foreach (const QString &s, commands(cmd))
        installHandler(s, h);
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

void BTerminalIOHandler::setRootSettingsNode(BSettingsNode *root)
{
    if (!BTerminalIOHandlerPrivate::testInit())
        return;
    B_DS(BTerminalIOHandler);
    if (ds->root)
        delete ds->root;
    ds->root = root;
}

void BTerminalIOHandler::setHelpDescription(const QString &s)
{
    if (!BTerminalIOHandlerPrivate::testInit())
        return;
    ds_func()->help = s;
}

void BTerminalIOHandler::setCommandHelp(const QString &command, const CommandHelp &help)
{
    if (!BTerminalIOHandlerPrivate::testInit())
        return;
    if (command.isEmpty())
        return;
    ds_func()->commandHelp.insert(command, help);
}

BSettingsNode *BTerminalIOHandler::rootSettingsNode()
{
    if (!BTerminalIOHandlerPrivate::testInit())
        return 0;
    return ds_func()->root;
}

/*============================== Protected methods =========================*/

bool BTerminalIOHandler::handleCommand(const QString &, const QStringList &)
{
    writeLine(d_func()->translations ? tr("Unknown command") : QString("Unknown command"));
    return false;
}

bool BTerminalIOHandler::handleQuit(const QString &, const QStringList &)
{
    QCoreApplication::quit();
    return true;
}

bool BTerminalIOHandler::handleSet(const QString &, const QStringList &args)
{
    if (args.size() < 1)
    {
        writeLine(d_func()->translations ? tr("Invalid parameters count") : QString("Invalid parameters count"));
        return false;
    }
    if (!d_func()->root)
    {
        writeLine(d_func()->translations ? tr("Settings structure not set") : QString("Settings structure not set"));
        return false;
    }
    if (args.first() == "--tree")
    {
        d_func()->root->showTree();
    }
    else if (args.first() == "--show")
    {
        if (args.size() != 2)
        {
            writeLine(d_func()->translations ? tr("Invalid parameters count") : QString("Invalid parameters count"));
            return false;
        }
        if (!d_func()->root->show(args.last()))
        {
            writeLine(d_func()->translations ? tr("Failed to show value") : QString("Failed to show value"));
            return false;
        }
    }
    else if (args.first() == "--description")
    {
        if (args.size() != 2)
        {
            writeLine(d_func()->translations ? tr("Invalid parameters count") : QString("Invalid parameters count"));
            return false;
        }
        BSettingsNode *n = d_func()->root->find(args.first());
        if (!n)
        {
            writeLine(d_func()->translations ? tr("No such option") : QString("No such option"));
            return false;
        }
        QString s = n->description();
        if (s.isEmpty())
        {
            writeLine(d_func()->translations ? tr("No description") : QString("No description"));
            return false;
        }
        writeLine(d_func()->translations ? QCoreApplication::translate("BSettingsNode", s.toUtf8().constData()) : s);
    }
    else
    {
        BSettingsNode *n = d_func()->root->find(args.first());
        if (!n)
        {
            writeLine(d_func()->translations ? tr("No such option") : QString("No such option"));
            return false;
        }
        if (args.size() == 2)
        {
            bool b = false;
            QVariant v = BSettingsNode::stringToVariant(args.last(), n->type(), &b);
            if (!b)
            {
                writeLine(d_func()->translations ? tr("Invalid value") : QString("Invalid value"));
                return false;
            }
            if (!d_func()->root->set(args.first(), v))
            {
                writeLine(d_func()->translations ? tr("Failed to set value") : QString("Failed to set value"));
                return false;
            }
            else
            {
                writeLine(d_func()->translations ? tr("OK") : QString("OK"));
            }
        }
        else if (args.size() == 1)
        {
            if (!d_func()->root->set(args.first()))
            {
                writeLine(d_func()->translations ? tr("Failed to set value") : QString("Failed to set value"));
                return false;
            }
            else
            {
                writeLine(d_func()->translations ? tr("OK") : QString("OK"));
            }
        }
        else
        {
            writeLine(d_func()->translations ? tr("Invalid parameters count") : QString("Invalid parameters count"));
            return false;
        }
    }
    return true;
}

bool BTerminalIOHandler::handleHelp(const QString &, const QStringList &args)
{
    QString h = d_func()->translations ?
                QCoreApplication::translate("BTerminalIOHanlder", d_func()->help.toUtf8().constData()) :
                d_func()->help;
    if (args.isEmpty())
    {
        if (!h.isEmpty())
        {
            writeLine(d_func()->translations ? tr("Nothing to display") : QString("Nothing to display"));
            return false;
        }
        writeLine(h);
    }
    else if (args.size() == 1)
    {
        if (args.first() == "--all")
        {
            if (!h.isEmpty())
                writeLine(h);
            foreach (const CommandHelp &ch, d_func()->commandHelp)
                writeHelpLine(ch.usage, ch.description);
        }
        else if (args.first() == "--settings")
        {
            if (!d_func()->root)
            {
                writeLine(d_func()->translations ? tr("Settings structure not set") :
                                                   QString("Settings structure not set"));
                return false;
            }
            d_func()->root->showTree();
        }
        else if (d_func()->commandHelp.contains(args.first()))
        {
            const CommandHelp &ch = d_func()->commandHelp.value(args.first());
            writeHelpLine(ch.usage, ch.description);
        }
        else
        {
            writeLine(d_func()->translations ? tr("Invalid parameters count") : QString("Invalid parameters count"));
            return false;
        }
    }
    else
    {
        writeLine(d_func()->translations ? tr("Invalid parameters count") : QString("Invalid parameters count"));
        return false;
    }
    return false;
}

/*============================== Static protected variables ================*/

BTerminalIOHandler *BTerminalIOHandler::_m_self = 0;
