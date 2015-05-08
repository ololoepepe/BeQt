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
#include <QTimer>

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
    //
}

BTerminalThread::~BTerminalThread()
{
    //
}

/*============================== Protected methods =========================*/

void BTerminalThread::run()
{
    forever {
        QString line = BTerminalPrivate::readStream.readLine();
        QMutexLocker readLineLocker(&BTerminalPrivate::readLineMutex);
        if (BTerminalPrivate::readLine) {
            QMutexLocker lineLocker(&BTerminalPrivate::lineMutex);
            BTerminalPrivate::line = line;
            BTerminalPrivate::lineWasRead = true;
        } else {
            QMetaObject::invokeMethod(TerminalPrivate, "lineRead", Qt::QueuedConnection, Q_ARG(QString, line));
        }
    }
}

/*============================================================================
================================ BTerminalPrivate ============================
============================================================================*/

/*============================== Static public variables ===================*/

BTerminal::Color BTerminalPrivate::backgroundColor = BTerminal::DefaultColor;
QMap<QString, BTerminal::CommandHelpList> BTerminalPrivate::commandHelp;
QStringList BTerminalPrivate::commandHistory;
BTerminal::HandlerFunction BTerminalPrivate::defaultHandler = 0;
QMap<QString, BTerminal::HandlerFunction> BTerminalPrivate::handlers;
BTranslation BTerminalPrivate::help;
QStringList BTerminalPrivate::lastArgs;
QString BTerminalPrivate::lastCommand;
QString BTerminalPrivate::line;
QMutex BTerminalPrivate::lineMutex(QMutex::Recursive);
bool BTerminalPrivate::lineWasRead = false;
BTerminal::Mode BTerminalPrivate::mode = BTerminal::NoMode;
QMutex BTerminalPrivate::mutex(QMutex::Recursive);
bool BTerminalPrivate::readLine = false;
QMutex BTerminalPrivate::readLineMutex(QMutex::Recursive);
QTextStream BTerminalPrivate::readStream(stdin, QIODevice::ReadOnly);
BTerminalThread *BTerminalPrivate::readThread = 0;
BSettingsNode *BTerminalPrivate::root = 0;
BTerminal::Color BTerminalPrivate::textColor = BTerminal::DefaultColor;
bool BTerminalPrivate::translations = true;
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
        destroyThread();
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
    do_once(destroy)
        qAddPostRoutine(&BTerminal::destroy);
    switch (Mode) {
    case BTerminal::StandardMode:
        QTimer::singleShot(0, this, SLOT(createThread()));
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
    if (lastCommand == "last")
        return;
    QString histCmd = BTextTools::mergeArguments(lastCommand, lastArgs);
    if (!commandHistory.isEmpty() && commandHistory.first() == histCmd)
        return;
    commandHistory.prepend(histCmd);
}

void BTerminalPrivate::createThread()
{
    readThread = new BTerminalThread(this);
    readThread->start();
}

void BTerminalPrivate::destroyThread()
{
    if (!readThread)
        return;
    readThread->terminate();
    readThread->wait();
    delete readThread;
    readThread = 0;
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

/*!
\class BTerminal
\inmodule BeQtCore
\brief The BTerminal class provides means of interaction with a terminal for applications.

The BTerminal class is used to read from the standard input (stdin) and write to the standard output (stdout/stderr).
It is also used to automatically handle commands entered by users. There are some additional functions for interacting
with a terminal, such as setTitle() or setBackgroundColor().

\warning BTerminal must not be instantiated or deleted explicitly. It is done automatically under the scenes. You also
must not inherit BTerminal.

\section1 Interacting with a terminal

Basic interaction with BTerminal consists of reading and writing to/from standard streams (stdin, stdout, stderr). You
may enable/disable echo to make the text entered into terminal (in)visible by calling setStdinEchoMode(). This is
useful when entering passwords.

You may also change the text color and background color of a terminal, get terminal size (number of rows and columns),
get and set terminal window title.

Eample:

\snippet src/corelib/bterminal.cpp 0

\section1 User command handling

By default, BTerminal does not provide capability to handle user commands. You have to set it's mode to StandardMode to
enable automatic command handling.

\note BTerminal instance is created and destroyed implicitly. You don't have to manually delete the instance, but you
may do so by calling destroy() or setting the mode to NoMode.

To make BTerminal handle a command, you have to install your own command handling function (see HandlerFunction) or use
one of those provided by BeQt:

\table
\header
    \li Standard command
    \li Command (what user enters)
    \li Corresponding function
    \li Description
\row
    \li HelpCommand
    \li help
    \li handleHelp()
    \li Prints information about the application, available commands, etc.
\row
    \li LastCommand
    \li last
    \li handleLast()
    \li Re-enters the last entered command, or shows the list of the commands entered (command history).
\row
    \li QuitCommand
    \li quit
    \li handleQuit()
    \li Calls QCoreApplication::quit() slot.
\row
    \li SetCommand
    \li set
    \li handleSet()
    \li Sets or shows an application setting, or shows the settings structure.
\endtable

You may also provide description of your command using setCommandHelp() method.

Example:

\snippet src/corelib/bterminal.cpp 1

BTerminal runs a separate thread which reads form the standard input. This operation is blocking, that's why a separate
thread is used. When another thread calls readLine(), that thread is terminated, and then started again, when the
readLine() method finishes.

All commands entered by user are stored in history and may be shown or called later with the help of the LastCommand
command.

\section1 Settings handling

BTerminal provides a builtin command for changing application settings by simply typing the setting name and it's
value. The value may also be shown with the help of that command.

To make BTerminal know about your settings hierarchy, you have to create the corresponding BSettingsNode hierarchy,
where each node corresponds to the part of a setting path.

To add builtin BeQt settings, use createBeQtSettingsNode() method (parent node must be set to the root node to make it
work correctly).

Example:

\snippet src/corelib/bterminal.cpp 2

For more datailed description of settings handling, see BSettingsNode class documentation.
*/

/*!
\enum BTerminal::Color

This enum type is used to describe terminal text or background color.

\value DefaultColor
\value Black
\value Red
\value Green
\value Brown
\value Blue
\value Magenta
\value Cyan
\value Lightgray
*/

/*!
\enum BTerminal::Mode

This enum type is used to describe terminal mode.

\value NoMode
BTerminal can perform only basic terminal interaction.

\value StandardMode
BTerminal can perform basic terminal interaction and automatically handle commands entered by user.
*/

/*!
\enum BTerminal::StandardCommand

This enum type is used to describe standard commands provided by BTerminal.

\value HelpCommand
\value LastCommand
\value QuitCommand
\value SetCommand
*/

/*!
\class BTerminal::CommandHelp
\brief The CommandHelp struct represents a command help (it's usage and description).
*/

/*!
\variable BTerminal::CommandHelp::description
Description of the command. As it is a BTranslation, this variable is translated to an appropriate language depending
on the current application locale.
*/

/*!
\variable BTerminal::CommandHelp::usage
Usage of a command. For example: "sum <summand1> <summand2> [summandN...]"
*/

/*!
\typedef BTerminal::CommandHelpList

The BTerminal::CommandHelpList typedef provides CommandHelp list for BTerminal.
*/

/*!
\typedef BTerminal::HandlerFunction

The BTerminal::HandlerFunction typedef provides a pointer to a function taking a const referance to a QString, and a
const reference to a QStringList, and returning bool.
*/

/*!
\fn void BTerminal::commandEntered(const QString &command, const QStringList &arguments)

This signal is emitted immediately after a user enters some command. The signal is emitted only when the mode of the
BTerminal is StandardMode.

\a command is the comman entered, and the \a arguments are it's arguments.
*/

/*============================== Static protected variables ================*/

/*!
\variable BTerminal::_m_self
This static variable is used to access BTerminal instance. You should not use it directly.
*/

BTerminal *BTerminal::_m_self = 0;

/*============================== Protected constructors ====================*/

/*!
Constructs a terminal and sets it's mode to \a mode. Must not be used directly.
*/

BTerminal::BTerminal(Mode mode) :
    QObject(0), BBaseObject(*new BTerminalPrivate(this, mode))
{
    d_func()->init();
}

/*!
Constructs an object and associates the given data object \a d with it. Must not be used directly.
*/

BTerminal::BTerminal(BTerminalPrivate &d) :
    QObject(0), BBaseObject(d)
{
    d_func()->init();
}

/*!
Destroys the object, deleting the associated data object. Must not be used directly.
*/

BTerminal::~BTerminal()
{
    _m_self = 0;
}

/*============================== Static public methods =====================*/

/*!
Returns the current background color of the terminal.

\sa setBackgroundColor()
*/

BTerminal::Color BTerminal::backgroundColor()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    return BTerminalPrivate::backgroundColor;
}

/*!
Returns the number of columns in the terminal (the terminal width in symbols).
*/

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

/*!
Returns the name of the standard command \a cmd.

\sa StandardCommand
*/

QString BTerminal::command(StandardCommand cmd)
{
    QStringList sl = commands(cmd);
    return !sl.isEmpty() ? sl.first() : QString();
}

/*!
Returns the default CommandHelp for the standard command \a cmd.

\sa CommandHelp, commandHelpList()
*/

BTerminal::CommandHelp BTerminal::commandHelp(StandardCommand cmd)
{
    CommandHelpList l = commandHelpList(cmd);
    return !l.isEmpty() ? l.first() : CommandHelp();
}

/*!
Returns the CommandHelpList for the standard command \a cmd.

\sa CommandHelp, CommandHelpList, commandHelp()
*/

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

/*!
Retunrs the history of entered commands.

\sa setCommandHistory()
*/

QStringList BTerminal::commandHistory()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return QStringList();
    if (StandardMode != ds_func()->Mode)
        return QStringList();
    return ds_func()->commandHistory;
}

/*!
Retunrs the list of names (including aliases) of the standard command \a cmd.
*/

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

/*!
Connects BTerminal's signal commandEntered() to the \a {receiver}'s slot or signal \a method.
*/

void BTerminal::connectToCommandEntered(QObject *receiver, const char *method)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    if (!receiver || !method)
        return;
    connect(_m_self, SIGNAL(commandEntered(QString, QStringList)), receiver, method);
}

/*!
Returns a pointer to a BSettingsNode object describing the BeQt settings structure. The node's parnt is set to
\a parent.

BTerminal does \e not take ownership of the object. You have to delete it manually, unless you install it on BTerminal.

The following BeQt settings are available:

\list
    \li BeQt.Core.locale
\endlist

\sa {Settings handling}, BSettingsNode
*/

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

/*!
Destroys the terminal instance. If there is no BTerminal instance, nothing is done.
*/

void BTerminal::destroy()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!_m_self)
        return;
    delete _m_self;
    _m_self = 0;
}

/*!
Disconnects BTerminal's signal commandEntered() from the \a {receiver}'s slot or signal \a method.
*/

void BTerminal::disconnectFromCommandEntered(QObject *receiver, const char *method)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    if (!receiver || !method)
        return;
    disconnect(_m_self, SIGNAL(commandEntered(QString, QStringList)), receiver, method);
}

/*!
Emulates user input of a \a command with \a arguments.

The command and it's arguments are printed to a terminal, and then the command is handled as if it was entered by user.
*/

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

/*!
Retunrs the pointer to a handler function for standard command \a cmd.
*/

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

/*!
Installs a default handler function \a handler.

The default handler is called when no other handler matches a command. It may be used to print custom message notifying
the user about that.

\sa {User command handling}, installedDefaultHandler(), installHandler()
*/

void BTerminal::installDefaultHandler(HandlerFunction handler)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    ds_func()->defaultHandler = handler;
}

/*!
Returns a pointer to the default handler function.

\sa {User command handling}, installDefaultHandler(), installedHandler()
*/

BTerminal::HandlerFunction BTerminal::installedDefaultHandler()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return 0;
    if (StandardMode != ds_func()->Mode)
        return 0;
    return ds_func()->defaultHandler;
}

/*!
Returns the handler function installed for the command \a command.

\sa {User command handling}, installHandler()
*/

BTerminal::HandlerFunction BTerminal::installedHandler(const QString &command)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return 0;
    if (StandardMode != ds_func()->Mode)
        return 0;
    return !command.isEmpty() ? ds_func()->handlers.value(command) : 0;
}

/*!
Installs the \a handler function for the command \a command.

\sa {User command handling}, installedHandler()
*/

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

/*!
\overload
Installs the standard handler function for the standard command \a cmd.

\sa {User command handling}
*/

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

/*!
\overload
Installs the handler function \a handler for the standard command \a cmd.

\sa {User command handling}
*/

void BTerminal::installHandler(StandardCommand cmd, HandlerFunction handler)
{
    foreach (const QString &s, commands(cmd))
        installHandler(s, handler);
}

/*!
Returns the last command entered by user.

If \a args is a non-null pointer, it's value is set to the last command's arguments.
*/

QString BTerminal::lastCommand(QStringList *args)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return QString();
    if (StandardMode != ds_func()->Mode)
        return QString();
    return bRet(args, ds_func()->lastArgs, ds_func()->lastCommand);
}

/*!
Retunrs the curretn mode of the terminal.

\sa Mode
*/

BTerminal::Mode BTerminal::mode()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    return BTerminalPrivate::mode;
}

/*!
Reads a line of text that user enters into the terminal.

If \a text is not empty, it is written to standard output before reading the user input.

\sa {Interacting with a terminal}, readLineSecure()
*/

QString BTerminal::readLine(const QString &text)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    Mode m = mode();
    if (NoMode != m && StandardMode != m)
        return QString();
    if (!text.isEmpty())
        write(text);
    B_DS(BTerminal);
    if (ds && ds->readThread) {
        BTerminalPrivate::readLineMutex.lock();
        BTerminalPrivate::readLine = true;
        BTerminalPrivate::readLineMutex.unlock();
        forever {
            QMutexLocker locker(&BTerminalPrivate::lineMutex);
            if (BTerminalPrivate::lineWasRead) {
                BTerminalPrivate::lineWasRead = false;
                BTerminalPrivate::readLineMutex.lock();
                BTerminalPrivate::readLine = false;
                BTerminalPrivate::readLineMutex.unlock();
                QString line = BTerminalPrivate::line;
                BTerminalPrivate::line.clear();
                return line;
            }
            BeQt::msleep(10);
        }
    }
    QString line = BTerminalPrivate::readStream.readLine();
    return line;
}

/*!
Reads a line of text that user enters into the terminal.

If \a text is not empty, it is written to standard output before reading the user input.

Unlike readLine(), this functions always disables the echo mode first to prevent showing the text that a user will
enter. After reading a line, the echo mode is set to it's previous state.

\sa {Interacting with a terminal}, readLine(), setStdinEchoEnabled()
*/

QString BTerminal::readLineSecure(const QString &text)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    Mode m = mode();
    if (NoMode != m && StandardMode != m)
        return QString();
    bool prevEcho = stdinEchoEnabled();
    if (prevEcho)
        setStdinEchoEnabled(false);
    QString line = readLine(text);
    if (prevEcho)
        setStdinEchoEnabled(true);
    writeLine();
    return line;
}

/*!
Returns a pointer to the root settings node.

\sa {Settings handling}, BSettingsNode
*/

BSettingsNode *BTerminal::rootSettingsNode()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return 0;
    if (StandardMode != ds_func()->Mode)
        return 0;
    return ds_func()->root;
}

/*!
Returns the number of rows in the terminal (the terminal height in lines).
*/

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

/*!
Sets the terminal background color to \a color.

\sa backgroundColor()
*/

void BTerminal::setBackgroundColor(Color color)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    BTerminalPrivate::backgroundColor = color;
    BTerminalPrivate::resetColor();
}

/*!
Sets the command help for \a command to \a help.
*/

void BTerminal::setCommandHelp(const QString &command, const CommandHelp &help)
{
    setCommandHelp(command, CommandHelpList() << help);
}

/*!
Sets the list of command help for \a command to \a list.
*/

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

/*!
Sets the history of entered commands to \a list.

\sa commandHistory()
*/

void BTerminal::setCommandHistory(const QStringList &list)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    ds_func()->commandHistory = list;
}

/*!
Sets the basic message for HelpCommand to \a t.
*/

void BTerminal::setHelpDescription(const BTranslation &t)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    ds_func()->help = t;
}

/*!
Sets the terminal mode to \a mode.

If the \a mode is equal to current mode, nothing is done.

A BTerminal instance is created or deleted depending on the new mode.
*/

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

/*!
Sets a pointer to the root settings node to \a root.

\sa {Settings handling}, BSettingsNode
*/

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

/*!
If \a enabled is true, sets the echo mode of a terminal enabled. The text entered by a user to the terminal will be
visible.

If \a enabled is false, the text entered by a user to the terminal will not be displayed in the terminal.

\sa stdinEchoEnabled()
*/

void BTerminal::setStdinEchoEnabled(bool enabled)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (enabled)
        tty.c_lflag |= ECHO;
    else
        tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#elif defined(Q_OS_WIN)
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    if (enabled)
        mode |= ENABLE_ECHO_INPUT;
    else
        mode &= ~ENABLE_ECHO_INPUT;
    SetConsoleMode(hStdin, mode);
#endif
}

/*!
Sets the color of the terminal text to \a color.

\sa textColor()
*/

void BTerminal::setTextColor(Color color)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    BTerminalPrivate::textColor = color;
    BTerminalPrivate::resetColor();
}

/*!
Sets the title of the terminal to \a title.

The title is usually diaplayed in the window header.
*/

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

/*!
If \a enabled is true, all potentially localizable text printed by BTerminal will be translated. Otherwise, the printed
text will not be translated.

\sa translationsEnabled()
*/

void BTerminal::setTranslationsEnabled(bool enabled)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    ds_func()->translations = enabled;
}

/*!
Returns the size of the terminal in columns (number of symbols in a line) and rows (number of lines).
*/

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


/*!
Returns the terminal echo mode.

\sa setStdinEchoEnabled()
*/

bool BTerminal::stdinEchoEnabled()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    return (tty.c_lflag & ECHO);
#elif defined(Q_OS_WIN)
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    return (mode & ENABLE_ECHO_INPUT);
#endif
}

/*!
Returns the color of the terminal text.

\sa setTextColor()
*/

BTerminal::Color BTerminal::textColor()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    return BTerminalPrivate::textColor;
}

/*!
Returns true if translation of the printed text is enabled; otherwise returns false.

\sa setTranslationsEnabled()
*/

bool BTerminal::translationsEnabled()
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    if (!BTerminalPrivate::testInit("BTerminal"))
        return false;
    return ds_func()->translations;
}

/*!
Writes \a text to the standard output (stdout).
*/

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

/*!
Writes \a text to the standard error stream (stderr).
*/

void BTerminal::writeErr(const QString &text)
{
    QMutexLocker locker(&BTerminalPrivate::mutex);
    Mode m = mode();
    if (NoMode != m && StandardMode != m)
        return;
    BTerminalPrivate::writeErrStream << text;
    BTerminalPrivate::writeErrStream.flush();
}

/*!
Prints the formatted help for the command with \a usage and \a description.
*/

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

/*!
\overload
Prints the formatted help for the command with \a usage and \a description.

If \a translate is true, \a description is translated (the current application locale is used); otherwise it is not
translated.
*/

void BTerminal::writeHelpLine(const QString &usage, const BTranslation &description, bool translate)
{
    writeHelpLine(usage, translate ? description.translate() : description.sourceText());
}

/*!
Prints the formatted help for each of the CommandHelo from \a list.

If \a translate is true, description is translated (the current application locale is used); otherwise it is not
translated.
*/

void BTerminal::writeHelpLines(const CommandHelpList &list, bool translate)
{
    foreach (const CommandHelp &h, list)
        writeHelpLine(h.usage, h.description, translate);
}

/*!
Writes \a text to the standard output (stdout). The end of line ('\n') is appended to the text.
*/

void BTerminal::writeLine(const QString &text)
{
    write(text + "\n");
}

/*!
Writes \a text to the standard error stream (stderr). The end of line ('\n') is appended to the text.
*/

void BTerminal::writeLineErr(const QString &text)
{
    writeErr(text + "\n");
}

/*============================== Static protected methods ==================*/

/*!
Handles command \a command with arguments \a args.

Usage:

\table
\header
    \li Usage
    \li Description
\row
    \li help <command>
    \li Shows description of <command>.
\row
    \li help [--commands|--settings|--all]
    \li Shows basic help, or:

        --commands - list of all available commands

        --settings - list of all available settings

        --all - all of the above (including basic help).
\row
    \li help --about [description|changelog|license|authors|translations|thanksto]
    \li Shows information about this application.
\row
    \li help --about-beqt [description|changelog|license|authors|translations|thanksto]
    \li Shows information about BeQt libraries.
\endtable

Returns true if there is a BTerminal instance and if the terminal mode is StandardMode. Otherwise returns false.
*/

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

/*!
Handles command \a cmd with arguments \a args.

Usage:

\table
\header
    \li Usage
    \li Description
\row
    \li last [n]
    \li Reenters last n-th command (the very last by default).
\row
    \li last --show [n]
    \li Shows n last commands (10 by default).
\endtable

Returns true if there is a BTerminal instance and if the terminal mode is StandardMode. Otherwise returns false.
*/

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

/*!
Handles command \a command with arguments \a arguments.

Usage:

\table
\header
    \li Usage
    \li Description
\row
    \li quit [whatever...]
    \li Calls QCoreApplication::quit() slot. Arguments are ignored.
\endtable

Returns true if there is a BTerminal instance and if the terminal mode is StandardMode. Otherwise returns false.
*/

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

/*!
Handles command \a command with arguments \a args.

Usage:

\table
\header
    \li Usage
    \li Description
\row
    \li set --tree
    \li Shows list of all available settings.
\row
    \li set --show|--description <key>
    \li Shows the value for <key> or it's description.
\row
    \li set <key> [vlue]
    \li Sets the value for <key> to [value] (if specified) or requests value input.
\endtable

Returns true if there is a BTerminal instance and if the terminal mode is StandardMode. Otherwise returns false.
*/

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
