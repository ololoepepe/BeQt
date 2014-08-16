/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "bterminal.h"
#include "bterminal_p.h"

#include "bapplicationbase.h"
#include "bbaseobject.h"
#include "bbaseobject_p.h"
#include "bnamespace.h"
#include "bsettingsnode.h"
#include "btexttools.h"
#include "btranslation.h"

#include <QChar>
#include <QCoreApplication>
#include <QDebug>
#include <QIODevice>
#include <QMetaObject>
#include <QMutex>
#include <QMutexLocker>
#include <QScopedArrayPointer>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QThread>

#include <cstdio>
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#elif defined(Q_OS_WIN)
#include "windows.h"
#endif

/*============================================================================
================================ Global static functions =====================
============================================================================*/

static bool areEqual(const BTerminal::CommandHelpList &l1, const BTerminal::CommandHelpList &l2)
{
    if (l1.size() != l2.size())
        return false;
    foreach (int i, bRangeD(0, l1.size() - 1)) {
        const BTerminal::CommandHelp &ch1 = l1.at(i);
        const BTerminal::CommandHelp &ch2 = l2.at(i);
        if (ch1.usage != ch2.usage || ch1.description != ch2.description)
            return false;
    }
    return true;
}

static bool setLocale(const BSettingsNode *, const QVariant &v)
{

    QString s;
    if (!v.isNull()) {
        bool ok = false;
        s = BSettingsNode::variantToString(v, &ok);
        if (!ok)
            return false;
    } else {
        s = bReadLine(BeQt::translate("BTerminal", "Enter locale:") + " ");
        if (s.isEmpty())
            return false;
    }
    bool ok = true;
    QVariant vv = BSettingsNode::stringToVariant(s, QVariant::Locale, &ok);
    if (!ok)
        return false;
    BApplicationBase::setLocale(vv.toLocale());
    return true;
}

/*============================================================================
================================ BTerminalThread =============================
============================================================================*/

/*============================== Public constructors =======================*/

BTerminalThread::BTerminalThread(BTerminalPrivate *tp) :
    QThread(0), TerminalPrivate(tp)
{
    loop = 0;
}

BTerminalThread::~BTerminalThread()
{
    //
}

/*============================== Protected methods =========================*/

void BTerminalThread::run()
{
    forever {
        QString l = BTerminalPrivate::readStream.readLine();
        if (loop) {
            lastLine = l;
            QMetaObject::invokeMethod(loop, "quit", Qt::QueuedConnection);
        }
        else {
            QMetaObject::invokeMethod(TerminalPrivate, "lineRead", Qt::QueuedConnection, Q_ARG(QString, l));
        }
    }
}

/*============================================================================
================================ BTerminalPrivate ============================
============================================================================*/

/*============================== Static public variables ===================*/

BTerminal::Color BTerminalPrivate::backgroundColor = BTerminal::DefaultColor;
BTerminal::Mode BTerminalPrivate::mode = BTerminal::NoMode;
QMutex BTerminalPrivate::mutex(QMutex::Recursive);
QTextStream BTerminalPrivate::readStream(stdin, QIODevice::ReadOnly);
BTerminal::Color BTerminalPrivate::textColor = BTerminal::DefaultColor;
QTextStream BTerminalPrivate::writeErrStream(stderr, QIODevice::WriteOnly);
QTextStream BTerminalPrivate::writeStream(stdout, QIODevice::WriteOnly);

/*============================== Public constructors =======================*/

BTerminalPrivate::BTerminalPrivate(BTerminal *q, BTerminal::Mode m) :
    BBaseObjectPrivate(q), Mode(m)
{
    //
}

BTerminalPrivate::~BTerminalPrivate()
{
    QMutexLocker locker(&BTerminalPrivate::mutex); //Note: Not sure that needed
    switch (Mode) {
    case BTerminal::StandardMode:
        delete root;
        readThread->terminate();
        readThread->wait();
        delete readThread;
        break;
    default:
        break;
    }
}

/*============================== Static public methods =====================*/

void BTerminalPrivate::resetColor()
{
    QMutexLocker lockae(&mutex);
#if defined(Q_OS_UNIX)
    QString s = "\e[";
    if (BTerminal::DefaultColor != textColor) {
        s += "0;3" + QString::number(textColor);
        if (BTerminal::DefaultColor != backgroundColor)
            s += ";4" + QString::number(backgroundColor);
    } else if (BTerminal::DefaultColor != backgroundColor) {
        s += "7;3" + QString::number(backgroundColor);
    } else {
        s += "0";
    }
    s += "m";
    BTerminal::write(s);
#elif defined (Q_OS_WIN)
    init_once(WORD, DefCol, WORD()) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        DefCol = csbi.wAttributes;
    }
    WORD c = DefCol;
    if (BTerminal::DefaultColor != textColor) {
        c = c & ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        switch (textColor) {
        case BTerminal::Red:
            c = c | FOREGROUND_RED;
            break;
        case BTerminal::Green:
            c = c | FOREGROUND_GREEN;
            break;
        case BTerminal::Brown:
            c = c | FOREGROUND_GREEN | FOREGROUND_RED;
            break;
        case BTerminal::Blue:
            c = c | FOREGROUND_BLUE;
            break;
        case BTerminal::Magenta:
            c = c | FOREGROUND_RED | FOREGROUND_BLUE;
            break;
        case BTerminal::Cyan:
            c = c | FOREGROUND_BLUE | FOREGROUND_GREEN;
            break;
        case BTerminal::Lightgray:
            c = c | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
            break;
        case BTerminal::Black:
        default:
            break;
        }
    }
    if (BTerminal::DefaultColor != backgroundColor) {
        c = c & ~(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
        switch (backgroundColor) {
        case BTerminal::Red:
            c = c | BACKGROUND_RED;
            break;
        case BTerminal::Green:
            c = c | BACKGROUND_GREEN;
            break;
        case BTerminal::Brown:
            c = c | BACKGROUND_GREEN | BACKGROUND_RED;
            break;
        case BTerminal::Blue:
            c = c | BACKGROUND_BLUE;
            break;
        case BTerminal::Magenta:
            c = c | BACKGROUND_RED | BACKGROUND_BLUE;
            break;
        case BTerminal::Cyan:
            c = c | BACKGROUND_BLUE | BACKGROUND_GREEN;
            break;
        case BTerminal::Lightgray:
            c = c | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
            break;
        case BTerminal::Black:
        default:
            break;
        }
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
#endif
}

bool BTerminalPrivate::testInit(const char *where)
{
    const char *w = where ? where : "BTerminal";
    return bTest(qs_func(), w, "There must be a BTerminal instance");
}

/*============================== Public methods ============================*/

void BTerminalPrivate::init()
{
    defaultHandler = 0;
    qAddPostRoutine(&BTerminal::destroy);
    root = 0;
    translations = true;
    switch (Mode) {
    case BTerminal::StandardMode:
        readThread = new BTerminalThread(this);
        readThread->start();
        break;
    default:
        break;
    }
}

/*============================== Public slots ==============================*/

void BTerminalPrivate::commandEntered(const QString &cmd, const QStringList &args)
{
    if (cmd.isEmpty())
        return;
    lastCommand = cmd;
    lastArgs = args;
    B_Q(BTerminal);
    QMetaObject::invokeMethod(q, "commandEntered", Q_ARG(QString, lastCommand), Q_ARG(QStringList, lastArgs));
    if (handlers.contains(lastCommand))
        handlers.value(lastCommand)(lastCommand, lastArgs);
    else if (defaultHandler)
        defaultHandler(lastCommand, lastArgs);
    else
        q->writeLine(translations ? tr("Unknown command", "message") : QString("Unknown command"));
    if (lastCommand != "last" && !lastCommand.startsWith("last "))
        commandHistory.prepend(lastCommand);
}

void BTerminalPrivate::lineRead(const QString &text)
{
    QStringList args;
    bool ok = false;
    QString cmd = BTextTools::splitCommand(text, args, &ok);
    if (!ok || cmd.isEmpty())
        return;
    commandEntered(cmd, args);
}

/*============================================================================
================================ BTerminal ===================================
============================================================================*/

/*============================== Static protected variables ================*/

BTerminal *BTerminal::_m_self = 0;

/*============================== Protected constructors ====================*/

BTerminal::BTerminal(Mode mode) :
    QObject(0), BBaseObject(*new BTerminalPrivate(this, mode))
{
    d_func()->init();
}

BTerminal::BTerminal(BTerminalPrivate &d) :
    QObject(0), BBaseObject(d)
{
    d_func()->init();
}

BTerminal::~BTerminal()
{
    _m_self = 0;
}

/*============================== Static public methods =====================*/

BTerminal::Color BTerminal::backgroundColor()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    return BTerminalPrivate::backgroundColor;
}

int BTerminal::columnCount()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return w.ws_col;
#elif defined(Q_OS_WIN)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#endif
}

QString BTerminal::command(StandardCommand cmd)
{
    QStringList sl = commands(cmd);
    return !sl.isEmpty() ? sl.first() : QString();
}

BTerminal::CommandHelp BTerminal::commandHelp(StandardCommand cmd)
{
    CommandHelpList l = commandHelpList(cmd);
    return !l.isEmpty() ? l.first() : CommandHelp();
}

BTerminal::CommandHelpList BTerminal::commandHelpList(StandardCommand cmd)
{
    CommandHelpList l;
    CommandHelp h;
    switch (cmd) {
    case QuitCommand:
        h.usage = "quit|exit";
        h.description = BTranslation::translate("BTerminal", "Quit the application");
        l << h;
        break;
    case SetCommand:
        h.usage = "set --tree";
        h.description = BTranslation::translate("BTerminal", "Show list of all available settings");
        l << h;
        h.usage = "set --show|--description <key>";
        h.description = BTranslation::translate("BTerminal", "Show the value for <key> or it's description");
        l << h;
        h.usage = "set <key> [value]";
        h.description = BTranslation::translate("BTerminal", "Set the value for <key> to [value] "
                                                "(if specified) or request value input");
        l << h;
        break;
    case HelpCommand:
        h.usage = "help <command>";
        h.description = BTranslation::translate("BTerminal", "Show description of <command>");
        l << h;
        h.usage = "help [--commands|--settings|--all]";
        h.description = BTranslation::translate("BTerminal", "Show basic help, or:\n"
                                                "  --commands - list of all available commands\n"
                                                "  --settings - list of all available settings\n"
                                                "  --all - all of the above (including basic help)");
        l << h;
        h.usage = "help --about [description|changelog|license|authors|translations|thanksto]";
        h.description = BTranslation::translate("BTerminal", "Show information about this application");
        l << h;
        h.usage = "help --about-beqt [description|changelog|license|authors|translations|thanksto]";
        h.description = BTranslation::translate("BTerminal", "Show information about BeQt libraries");
        l << h;
        break;
    case LastCommand:
        h.usage = "last [n]";
        h.description = BTranslation::translate("BTerminal", "Reenter last n-th command (the very last by default)");
        l << h;
        h.usage = "last --show [n]";
        h.description = BTranslation::translate("BTerminal", "Show n last commands (10 by default)");
        l << h;
        break;
    default:
        break;
    }
    return l;
}

QStringList BTerminal::commandHistory()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return QStringList();
    if (StandardMode != ds_func()->Mode)
        return QStringList();
    return ds_func()->commandHistory;
}

QStringList BTerminal::commands(StandardCommand cmd)
{
    QStringList sl;
    switch (cmd) {
    case QuitCommand:
        sl << "quit" << "exit";
        break;
    case SetCommand:
        sl << "set";
        break;
    case HelpCommand:
        sl << "help";
        break;
    case LastCommand:
        sl << "last";
        break;
    default:
        break;
    }
    return sl;
}

void BTerminal::connectToCommandEntered(QObject *receiver, const char *method)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    if (!receiver || !method)
        return;
    connect(_m_self, SIGNAL(commandEntered(QString,QStringList)), receiver, method);
}

BSettingsNode *BTerminal::createBeQtSettingsNode(BSettingsNode *parent)
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

void BTerminal::destroy()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    delete _m_self;
    _m_self = 0;
}

void BTerminal::disconnectFromCommandEntered(QObject *receiver, const char *method)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    if (!receiver || !method)
        return;
    disconnect(_m_self, SIGNAL(commandEntered(QString,QStringList)), receiver, method);
}

void BTerminal::emulateCommand(const QString &command, const QStringList &arguments)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    QMetaObject::invokeMethod(ds_func(), "commandEntered", Qt::QueuedConnection, Q_ARG(QString, command),
                              Q_ARG(QStringList, arguments));
}

BTerminal::HandlerFunction BTerminal::handler(StandardCommand cmd)
{
    switch (cmd) {
    case QuitCommand:
        return &BTerminal::handleQuit;
    case SetCommand:
        return &BTerminal::handleSet;
    case HelpCommand:
        return &BTerminal::handleHelp;
    case LastCommand:
        return &BTerminal::handleLast;
    default:
        return 0;
    }
}

void BTerminal::installDefaultHandler(HandlerFunction handler)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    ds_func()->defaultHandler = handler;
}

BTerminal::HandlerFunction BTerminal::installedDefaultHandler()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return 0;
    if (StandardMode != ds_func()->Mode)
        return 0;
    return ds_func()->defaultHandler;
}

BTerminal::HandlerFunction BTerminal::installedHandler(const QString &command)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return 0;
    if (StandardMode != ds_func()->Mode)
        return 0;
    return !command.isEmpty() ? ds_func()->handlers.value(command) : 0;
}

void BTerminal::installHandler(const QString &command, HandlerFunction handler)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    if (command.isEmpty() || !handler)
        return;
    B_DS(BTerminal);
    if (ds->handlers.contains(command))
        return;
    ds->handlers.insert(command, handler);
}

void BTerminal::installHandler(StandardCommand cmd)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    HandlerFunction h = handler(cmd);
    CommandHelpList l = commandHelpList(cmd);
    foreach (const QString &s, commands(cmd)) {
        installHandler(s, h);
        setCommandHelp(s, l);
    }
}

void BTerminal::installHandler(StandardCommand cmd, HandlerFunction handler)
{
    foreach (const QString &s, commands(cmd))
        installHandler(s, handler);
}

QString BTerminal::lastCommand(QStringList *args)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return QString();
    if (StandardMode != ds_func()->Mode)
        return QString();
    return bRet(args, ds_func()->lastArgs, ds_func()->lastCommand);
}

BTerminal::Mode BTerminal::mode()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    return BTerminalPrivate::mode;
}

QString BTerminal::readLine(const QString &text)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    Mode m = mode();
    if (NoMode != m && StandardMode != m)
        return QString();
    if (!text.isEmpty())
        write(text);
    if (NoMode == m)
        return BTerminalPrivate::readStream.readLine();
    QEventLoop loop;
    ds_func()->readThread->loop = &loop;
    loop.exec();
    QString line = ds_func()->readThread->lastLine;
    ds_func()->readThread->loop = 0;
    return line;
}

QString BTerminal::readLineSecure(const QString &text)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    Mode m = mode();
    if (NoMode != m && StandardMode != m)
        return QString();
    setStdinEchoEnabled(false);
    QString line = readLine(text);
    setStdinEchoEnabled(true);
    writeLine();
    return line;
}

BSettingsNode *BTerminal::rootSettingsNode()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return 0;
    if (StandardMode != ds_func()->Mode)
        return 0;
    return ds_func()->root;
}

int BTerminal::rowCount()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return w.ws_row;
#elif defined(Q_OS_WIN)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#endif
}

void BTerminal::setBackgroundColor(Color color)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    BTerminalPrivate::backgroundColor = color;
    BTerminalPrivate::resetColor();
}

void BTerminal::setCommandHelp(const QString &command, const CommandHelp &help)
{
    setCommandHelp(command, CommandHelpList() << help);
}

void BTerminal::setCommandHelp(const QString &command, const CommandHelpList &list)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    if (command.isEmpty())
        return;
    ds_func()->commandHelp.insert(command, list);
}

void BTerminal::setCommandHistory(const QStringList &list)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    ds_func()->commandHistory = list;
}

void BTerminal::setHelpDescription(const BTranslation &t)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    ds_func()->help = t;
}

void BTerminal::setMode(Mode mode)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (mode == BTerminalPrivate::mode)
        return;
    if (_m_self)
        destroy();
    BTerminalPrivate::mode = mode;
    if (NoMode == mode)
        return;
    _m_self = new BTerminal(mode);
}

void BTerminal::setRootSettingsNode(BSettingsNode *root)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    B_DS(BTerminal);
    if (ds->root)
        delete ds->root;
    ds->root = root;
}

void BTerminal::setStdinEchoEnabled(bool enabled)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
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

void BTerminal::setTextColor(Color color)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    BTerminalPrivate::textColor = color;
    BTerminalPrivate::resetColor();
}

void BTerminal::setTitle(const QString &title)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    BTerminalPrivate::writeStream << QString("%1]0;%3%2").arg("\033", "\007", title);
    BTerminalPrivate::writeStream.flush();
#elif defined(Q_OS_WIN)
#ifdef UNICODE
    QScopedPointer<wchar_t> chars(new wchar_t[title.length() + 1]);
    title.toWCharArray(chars.data());
    chars.data()[title.length()] = '\0';
    LPCTSTR s = chars.data();
#else
    LPCTSTR s = title.toLocal8Bit().constData();
#endif
    SetConsoleTitle(s);
#endif
}

void BTerminal::setTranslationsEnabled(bool enabled)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    ds_func()->translations = enabled;
}

QSize BTerminal::size()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return QSize(w.ws_col, w.ws_row);
#elif defined(Q_OS_WIN)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return QSize(csbi.srWindow.Right - csbi.srWindow.Left + 1, csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
#endif
}

BTerminal::Color BTerminal::textColor()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    return BTerminalPrivate::textColor;
}

bool BTerminal::translationsEnabled()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return false;
    return ds_func()->translations;
}

void BTerminal::write(const QString &text)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    Mode m = mode();
    if (NoMode != m && StandardMode != m)
        return;
    if (text.isEmpty())
        return;
    BTerminalPrivate::writeStream << text;
    BTerminalPrivate::writeStream.flush();
}

void BTerminal::writeErr(const QString &text)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    Mode m = mode();
    if (NoMode != m && StandardMode != m)
        return;
    BTerminalPrivate::writeErrStream << text;
    BTerminalPrivate::writeErrStream.flush();
}

void BTerminal::writeHelpLine(const QString &usage, const QString &description)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    Mode m = mode();
    if (NoMode != m && StandardMode != m)
        return;
    if (usage.isEmpty())
        return;
    QString s;
    foreach (const QString &u, usage.split('\n'))
        s += "  " + u + "\n";
    s.remove(s.length() - 1, 1);
    if (!description.isEmpty()) {
        if (s.length() > 28)
            s += "\n" + QString().fill(' ', 30);
        else
            s += QString().fill(' ', 30 - s.length());
        s += description;
    }
    writeLine(s);
}

void BTerminal::writeHelpLine(const QString &usage, const BTranslation &description, bool translate)
{
    writeHelpLine(usage, translate ? description.translate() : description.sourceText());
}

void BTerminal::writeHelpLines(const CommandHelpList &list, bool translate)
{
    foreach (const CommandHelp &h, list)
        writeHelpLine(h.usage, h.description, translate);
}

void BTerminal::writeLine(const QString &text)
{
    write(text + "\n");
}

void BTerminal::writeLineErr(const QString &text)
{
    writeErr(text + "\n");
}

/*============================== Static protected methods ==================*/

bool BTerminal::handleHelp(const QString &, const QStringList &args)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return false;
    B_DS(BTerminal);
    if (StandardMode != ds->Mode)
        return false;
    QString h = ds->translations ? ds->help.translate() : ds->help.sourceText();
    if (args.isEmpty()) {
        if (h.isEmpty()) {
            writeLine(ds->translations ? tr("Nothing to display") : QString("Nothing to display"));
            return false;
        }
        writeLine(h);
    } else if (args.size() == 1) {
        if (args.first() == "--all") {
            if (h.isEmpty() && ds->commandHelp.isEmpty()) {
                writeLine(ds->translations ? tr("Nothing to display") : QString("Nothing to display"));
                return false;
            }
            if (!h.isEmpty()) {
                writeLine(h);
                if (!ds->commandHelp.isEmpty())
                    writeLine();
            }
            if (!ds->commandHelp.isEmpty()) {
                writeLine(ds->translations ? tr("The following commands are available:") :
                                                   QString("The following commands are available:"));
                writeLine();
                foreach (const CommandHelpList &ch, bWithoutDuplicates(ds->commandHelp.values(), &areEqual))
                    writeHelpLines(ch, ds->translations);
            }
        } else if (args.first() == "--commands") {
            if (ds->commandHelp.isEmpty()) {
                writeLine(ds->translations ? tr("Nothing to display") : QString("Nothing to display"));
                return false;
            }
            writeLine(ds->translations ? tr("The following commands are available:") :
                                               QString("The following commands are available:"));
            writeLine();
            foreach (const CommandHelpList &ch, bWithoutDuplicates(ds->commandHelp.values(), &areEqual))
                writeHelpLines(ch, ds->translations);
        } else if (args.first() == "--settings") {
            if (!ds->root) {
                writeLine(ds->translations ? tr("Settings structure not set") :
                                                   QString("Settings structure not set"));
                return false;
            }
            ds->root->showTree();
        } else if (args.first() == "--about" || args.first() == "--about-beqt") {
            if (args.first() == "--about-beqt")
                writeLine(BApplicationBase::beqtInfo(BApplicationBase::Copyringt));
            else
                writeLine(BApplicationBase::applicationInfo(BApplicationBase::Copyringt));
        } else if (ds->commandHelp.contains(args.first())) {
            writeHelpLines(ds->commandHelp.value(args.first()), ds->translations);
        } else {
            writeLine(ds->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
            return false;
        }
    } else if (args.size() == 2) {
        if (args.first() != "--about" && args.first() != "--about-beqt") {
            writeLine(ds->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
            return false;
        }
        BApplicationBase::About type;
        if (args.last() == "description") {
            type = BApplicationBase::Description;
        }
        else if (args.last() == "changelog") {
            type = BApplicationBase::ChangeLog;
        }
        else if (args.last() == "license") {
            type = BApplicationBase::License;
        }
        else if (args.last() == "authors") {
            type = BApplicationBase::Authors;
        }
        else if (args.last() == "translators") {
            type = BApplicationBase::Translators;
        }
        else if (args.last() == "thanksto") {
            type = BApplicationBase::ThanksTo;
        }
        else {
            writeLine(ds->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
            return false;
        }
        QString s = (args.first() == "--about-beqt") ? BApplicationBase::beqtInfo(type) :
                                                       BApplicationBase::applicationInfo(type);
        if (s.isEmpty()) {
            writeLine(ds->translations ? tr("Nothing to display") : QString("Nothing to display"));
            return false;
        }
        writeLine(s);
        return true;
    } else {
        writeLine(ds->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
        return false;
    }
    return true;
}

bool BTerminal::handleLast(const QString &cmd, const QStringList &args)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return false;
    B_DS(BTerminal);
    if (StandardMode != ds->Mode)
        return false;
    if (args.size() > 2) {
        writeLine(ds->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
        return false;
    }
    if (args.isEmpty()) {
        if (ds_func()->commandHistory.isEmpty()) {
            writeLine(ds->translations ? tr("History is empty") : QString("History is empty"));
            return true;
        }
        QString s = ds_func()->commandHistory.first();
        if (s == cmd || s.startsWith(cmd + " "))
            return true;
        writeLine(s);
        QMetaObject::invokeMethod(ds_func(), "lineRead", Qt::QueuedConnection, Q_ARG(QString, s));
    } else if (args.size() == 1) {
        if (args.first() == "--show") {
            if (ds_func()->commandHistory.isEmpty()) {
                writeLine(ds->translations ? tr("History is empty") : QString("History is empty"));
                return true;
            }
            int n = 10;
            if (n > ds_func()->commandHistory.size())
                n = ds_func()->commandHistory.size();
            foreach (int i, bRangeD(0, n - 1))
                writeLine(ds_func()->commandHistory.at(i));
        } else {
            bool ok = false;
            int n = args.first().toInt(&ok);
            if (!ok) {
                writeLine(ds->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
                return false;
            }
            if (ds_func()->commandHistory.isEmpty()) {
                writeLine(ds->translations ? tr("History is empty") : QString("History is empty"));
                return true;
            }
            if (n < 1 || n > ds_func()->commandHistory.size()) {
                writeLine(ds->translations ? tr("Invalid index") : QString("Invalid index"));
                return false;
            }
            QString s = ds_func()->commandHistory.at(n - 1);
            if (s.startsWith(cmd + " "))
                return true;
            writeLine(s);
            QMetaObject::invokeMethod(ds_func(), "lineRead", Qt::QueuedConnection, Q_ARG(QString, s));
        }
    } else if (args.size() == 2) {
        if (args.first() == "--show") {
            bool ok = false;
            int n = args.last().toInt(&ok);
            if (!ok) {
                writeLine(ds->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
                return false;
            }
            if (ds_func()->commandHistory.isEmpty()) {
                writeLine(ds->translations ? tr("History is empty") : QString("History is empty"));
                return true;
            }
            if (n < 1)
                n = 10;
            if (n > ds_func()->commandHistory.size())
                n = ds_func()->commandHistory.size();
            foreach (int i, bRangeD(0, n - 1))
                writeLine(ds_func()->commandHistory.at(i));
        } else {
            writeLine(ds->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
            return false;
        }
    }
    return true;
}

bool BTerminal::handleQuit(const QString &, const QStringList &)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return false;
    if (StandardMode != ds_func()->Mode)
        return false;
    QCoreApplication::quit();
    return true;
}

bool BTerminal::handleSet(const QString &, const QStringList &args)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return false;
    B_DS(BTerminal);
    if (StandardMode != ds->Mode)
        return false;
    if (args.size() < 1) {
        writeLine(ds->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
        return false;
    }
    if (!ds->root) {
        writeLine(ds->translations ? tr("Settings structure not set") : QString("Settings structure not set"));
        return false;
    }
    if (args.first() == "--tree") {
        ds->root->showTree();
    } else if (args.first() == "--show") {
        if (args.size() != 2) {
            writeLine(ds->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
            return false;
        }
        if (!ds->root->show(args.last())) {
            writeLine(ds->translations ? tr("Failed to show value") : QString("Failed to show value"));
            return false;
        }
    } else if (args.first() == "--description") {
        if (args.size() != 2) {
            writeLine(ds->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
            return false;
        }
        BSettingsNode *n = ds->root->find(args.last());
        if (!n) {
            writeLine(ds->translations ? tr("No such option") : QString("No such option"));
            return false;
        }
        BTranslation t = n->description();
        if (!t.isValid()) {
            writeLine(ds->translations ? tr("No description") : QString("No description"));
            return false;
        }
        writeLine(ds->translations ? t.translate() : t.sourceText());
    } else {
        BSettingsNode *n = ds->root->find(args.first());
        if (!n) {
            writeLine(ds->translations ? tr("No such option") : QString("No such option"));
            return false;
        }
        if (args.size() == 2) {
            bool b = false;
            QVariant v = BSettingsNode::stringToVariant(args.last(), n->type(), &b);
            if (!b) {
                writeLine(ds->translations ? tr("Invalid value") : QString("Invalid value"));
                return false;
            }
            if (!ds->root->set(args.first(), v)) {
                writeLine(ds->translations ? tr("Failed to set value") : QString("Failed to set value"));
                return false;
            } else {
                writeLine(ds->translations ? tr("OK") : QString("OK"));
            }
        } else if (args.size() == 1) {
            if (!ds->root->set(args.first())) {
                writeLine(ds->translations ? tr("Failed to set value") : QString("Failed to set value"));
                return false;
            } else {
                writeLine(ds->translations ? tr("OK") : QString("OK"));
            }
        } else {
            writeLine(ds->translations ? tr("Invalid parameters") : QString("Invalid parameters"));
            return false;
        }
    }
    return true;
}
