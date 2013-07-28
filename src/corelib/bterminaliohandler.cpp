#include "bterminaliohandler.h"
#include "bterminaliohandler_p.h"
#include "bglobal.h"
#include "bbase_p.h"
#include "bnamespace.h"
#include "bsettingsnode.h"
#include "bcoreapplication.h"
#include "btranslation.h"
#include "btexttools.h"

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
#include <QScopedArrayPointer>

#include <QDebug>

#include <cstdio>

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
#include "termios.h"
#elif defined(Q_OS_WIN)
#include "windows.h"
#endif

/*============================================================================
================================ Static global functions =====================
============================================================================*/

static bool areEqual(const BTerminalIOHandler::CommandHelpList &l1, const BTerminalIOHandler::CommandHelpList &l2)
{
    if (l1.size() != l2.size())
        return false;
    foreach (int i, bRangeD(0, l1.size() - 1))
    {
        const BTerminalIOHandler::CommandHelp &ch1 = l1.at(i);
        const BTerminalIOHandler::CommandHelp &ch2 = l2.at(i);
        if (ch1.usage != ch2.usage || ch1.description != ch2.description)
            return false;
    }
    return true;
}

static bool setLocale(const BSettingsNode *, const QVariant &v)
{

    QString s;
    if (!v.isNull())
    {
        bool ok = false;
        s = BSettingsNode::variantToString(v, &ok);
        if (!ok)
            return false;
    }
    else
    {
        s = bReadLine(BeQt::translate("BTerminalIOHandler", "Enter locale:") + " ");
        if (s.isEmpty())
            return false;
    }
    bool ok = true;
    QVariant vv = BSettingsNode::stringToVariant(s, QVariant::Locale, &ok);
    if (!ok)
        return false;
    BCoreApplication::setLocale(vv.toLocale());
    return true;
}

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

BTerminalIOHandler::CommandHelp BTerminalIOHandler::commandHelp(StandardCommand cmd)
{
    CommandHelpList l = commandHelpList(cmd);
    return !l.isEmpty() ? l.first() : CommandHelp();
}

BTerminalIOHandler::CommandHelpList BTerminalIOHandler::commandHelpList(StandardCommand cmd)
{
    CommandHelpList l;
    CommandHelp h;
    switch (cmd)
    {
    case QuitCommand:
        h.usage = "quit|exit";
        h.description = BTranslation::translate("BTerminalIOHandler", "Quit the application");
        l << h;
        break;
    case SetCommand:
        h.usage = "set --tree";
        h.description = BTranslation::translate("BTerminalIOHandler", "Show list of all available settings");
        l << h;
        h.usage = "set --show|--description <key>";
        h.description = BTranslation::translate("BTerminalIOHandler", "Show the value for <key> or it's description");
        l << h;
        h.usage = "set <key> [value]";
        h.description = BTranslation::translate("BTerminalIOHandler", "Set the value for <key> to [value] "
                                                "(if specified) or request value input");
        l << h;
        break;
    case HelpCommand:
        h.usage = "help <command>";
        h.description = BTranslation::translate("BTerminalIOHandler", "Show description of <command>");
        l << h;
        h.usage = "help [--commands|--settings|--all]";
        h.description = BTranslation::translate("BTerminalIOHandler", "Show basic help, or:\n"
                                                "  --commands - list of all available commands\n"
                                                "  --settings - list of all available settings\n"
                                                "  --all - all of the above (including basic help)");
        l << h;
        h.usage = "help --about [description|changelog|license|authors|translations|thanksto]";
        h.description = BTranslation::translate("BTerminalIOHandler", "Show information about this application");
        l << h;
        h.usage = "help --about-beqt [description|changelog|license|authors|translations|thanksto]";
        h.description = BTranslation::translate("BTerminalIOHandler", "Show information about BeQt libraries");
        l << h;
        break;
    default:
        break;
    }
    return l;
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
        args += (a.contains(' ') ? BTextTools::wrapped(a) : a) + " ";
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

QString BTerminalIOHandler::readLineSecure(const QString &text)
{
    setStdinEchoEnabled(false);
    QString line = readLine(text);
    setStdinEchoEnabled(true);
    writeLine();
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
    QString s;
    foreach (const QString &u, usage.split('\n'))
        s += "  " + u + "\n";
    s.remove(s.length() - 1, 1);
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

void BTerminalIOHandler::writeHelpLine(const QString &usage, const BTranslation &description, bool translate)
{
    writeHelpLine(usage, translate ? description.translate() : description.sourceText());
}

void BTerminalIOHandler::writeHelpLines(const CommandHelpList &list, bool translate)
{
    foreach (const CommandHelp &h, list)
        writeHelpLine(h.usage, h.description, translate);
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
#elif defined(Q_OS_WIN)
#ifdef _UNICODE
    QScopedPointer<wchar_t> chars(new wchar_t[title.length() + 1]);
    title.toWCharArray(chars);
    chars[title.length()] = '\0';
    LPCTSTR s = chars;
#else
    LPCTSTR s = title.toLocal8Bit().constData();
#endif
    SetConsoleTitle(s);
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

void BTerminalIOHandler::installHandler(StandardCommand cmd)
{
    InternalHandler h = handler(cmd);
    CommandHelpList l = commandHelpList(cmd);
    foreach (const QString &s, commands(cmd))
    {
        installHandler(s, h);
        setCommandHelp(s, l);
    }
}

void BTerminalIOHandler::installHandler(StandardCommand cmd, InternalHandler handler)
{
    foreach (const QString &s, commands(cmd))
        installHandler(s, handler);
}

void BTerminalIOHandler::installHandler(StandardCommand cmd, ExternalHandler handler)
{
    foreach (const QString &s, commands(cmd))
        installHandler(s, handler);
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

BSettingsNode *BTerminalIOHandler::createBeQtSettingsNode(BSettingsNode *parent)
{
    BSettingsNode *n = new BSettingsNode("BeQt", parent);
      BSettingsNode *nn = new BSettingsNode("Core", n);
        BSettingsNode *nnn = new BSettingsNode(QVariant::Locale, "locale", nn);
          nnn->setUserSetFunction(&setLocale);
          nnn->setDescription(BTranslation::translate("BSettingsNode", "Locale for the whole application. "
                                                      "Format: ??_**, where ?? stands for mandatory language name, "
                                                      "and ** stands for optional country name. "
                                                      "Examples: en, en_GB, ru, ru_RU"));
    return n;
}

void BTerminalIOHandler::setHelpDescription(const BTranslation &t)
{
    if (!BTerminalIOHandlerPrivate::testInit())
        return;
    ds_func()->help = t;
}

void BTerminalIOHandler::setCommandHelp(const QString &command, const CommandHelp &help)
{
    setCommandHelp(command, CommandHelpList() << help);
}

void BTerminalIOHandler::setCommandHelp(const QString &command, const CommandHelpList &list)
{
    if (!BTerminalIOHandlerPrivate::testInit())
        return;
    if (command.isEmpty())
        return;
    ds_func()->commandHelp.insert(command, list);
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
        writeLine(d_func()->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
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
            writeLine(d_func()->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
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
            writeLine(d_func()->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
            return false;
        }
        BSettingsNode *n = d_func()->root->find(args.last());
        if (!n)
        {
            writeLine(d_func()->translations ? tr("No such option") : QString("No such option"));
            return false;
        }
        BTranslation t = n->description();
        if (!t.isValid())
        {
            writeLine(d_func()->translations ? tr("No description") : QString("No description"));
            return false;
        }
        writeLine(d_func()->translations ? t.translate() : t.sourceText());
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
            writeLine(d_func()->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
            return false;
        }
    }
    return true;
}

bool BTerminalIOHandler::handleHelp(const QString &, const QStringList &args)
{
    QString h = d_func()->translations ? d_func()->help.translate() : d_func()->help.sourceText();
    if (args.isEmpty())
    {
        if (h.isEmpty())
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
            if (h.isEmpty() && d_func()->commandHelp.isEmpty())
            {
                writeLine(d_func()->translations ? tr("Nothing to display") : QString("Nothing to display"));
                return false;
            }
            if (!h.isEmpty())
            {
                writeLine(h);
                if (!d_func()->commandHelp.isEmpty())
                    writeLine();
            }
            if (!d_func()->commandHelp.isEmpty())
            {
                writeLine(d_func()->translations ? tr("The following commands are available:") :
                                                   QString("The following commands are available:"));
                writeLine();
                foreach (const CommandHelpList &ch, bWithoutDuplicates(d_func()->commandHelp.values(), &areEqual))
                    writeHelpLines(ch, d_func()->translations);
            }
        }
        else if (args.first() == "--commands")
        {
            if (d_func()->commandHelp.isEmpty())
            {
                writeLine(d_func()->translations ? tr("Nothing to display") : QString("Nothing to display"));
                return false;
            }
            writeLine(d_func()->translations ? tr("The following commands are available:") :
                                               QString("The following commands are available:"));
            writeLine();
            foreach (const CommandHelpList &ch, bWithoutDuplicates(d_func()->commandHelp.values(), &areEqual))
                writeHelpLines(ch, d_func()->translations);
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
        else if (args.first() == "--about" || args.first() == "--about-beqt")
        {
            if (args.first() == "--about-beqt")
                writeLine(BCoreApplication::beqtInfo(BCoreApplication::Copyringt));
            else
                writeLine(BCoreApplication::applicationInfo(BCoreApplication::Copyringt));
        }
        else if (d_func()->commandHelp.contains(args.first()))
        {
            writeHelpLines(d_func()->commandHelp.value(args.first()), d_func()->translations);
        }
        else
        {
            writeLine(d_func()->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
            return false;
        }
    }
    else if (args.size() == 2)
    {
        if (args.first() != "--about" && args.first() != "--about-beqt")
        {
            writeLine(d_func()->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
            return false;
        }
        BCoreApplication::About type;
        if (args.last() == "description")
        {
            type = BCoreApplication::Description;
        }
        else if (args.last() == "changelog")
        {
            type = BCoreApplication::ChangeLog;
        }
        else if (args.last() == "license")
        {
            type = BCoreApplication::License;
        }
        else if (args.last() == "authors")
        {
            type = BCoreApplication::Authors;
        }
        else if (args.last() == "translators")
        {
            type = BCoreApplication::Translators;
        }
        else if (args.last() == "thanksto")
        {
            type = BCoreApplication::ThanksTo;
        }
        else
        {
            writeLine(d_func()->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
            return false;
        }
        QString s = (args.first() == "--about-beqt") ? BCoreApplication::beqtInfo(type) :
                                                       BCoreApplication::applicationInfo(type);
        if (s.isEmpty())
        {
            writeLine(d_func()->translations ? tr("Nothing to display") : QString("Nothing to display"));
            return false;
        }
        writeLine(s);
        return true;
    }
    else
    {
        writeLine(d_func()->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
        return false;
    }
    return true;
}

/*============================== Static protected variables ================*/

BTerminalIOHandler *BTerminalIOHandler::_m_self = 0;
