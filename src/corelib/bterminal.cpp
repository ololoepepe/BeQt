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
#include "bglobal.h"
#include "bbaseobject.h"
#include "bbaseobject_p.h"
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
#include <QSize>

#include <QDebug>

#include <cstdio>

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#elif defined(Q_OS_WIN)
#include "windows.h"
#endif

/*============================================================================
================================ Static global functions =====================
============================================================================*/

static bool areEqual(const BTerminal::CommandHelpList &l1, const BTerminal::CommandHelpList &l2)
{
    if (l1.size() != l2.size())
        return false;
    foreach (int i, bRangeD(0, l1.size() - 1))
    {
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
    if (!v.isNull())
    {
        bool ok = false;
        s = BSettingsNode::variantToString(v, &ok);
        if (!ok)
            return false;
    }
    else
    {
        s = bReadLine(BeQt::translate("BTerminal", "Enter locale:") + " ");
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
================================ BTerminalThread =============================
============================================================================*/

/*============================== Public constructors =======================*/

BTerminalThread::BTerminalThread() :
    QThread(0), readStream(stdin, QIODevice::ReadOnly)
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
================================ BTerminalPrivate ============================
============================================================================*/

/*============================== Public constructors =======================*/

BTerminalPrivate::BTerminalPrivate(BTerminal *q, BTerminal::Mode m) :
    BBaseObjectPrivate(q), Mode(m)
{
    //
}

BTerminalPrivate::~BTerminalPrivate()
{
    switch (Mode)
    {
    case BTerminal::StandardMode:
        delete root;
        removeThread();
        break;
    default:
        break;
    }
}

/*============================== Static public methods =====================*/

bool BTerminalPrivate::testInit(const char *where)
{
    const char *w = where ? where : "BTerminal";
    B_QS(BTerminal);
    return bTest(qs, w, "There must be a BTerminal instance");
}

BTerminalThread *BTerminalPrivate::initThread(bool silent)
{
    if (readThread)
        return readThread;
    QMutexLocker locker(&threadMutex);
    if (readThread)
        return readThread;
    if (silent && !QCoreApplication::instance())
        return 0;
    if (!silent && !bTest(QCoreApplication::instance(), "BTerminalPrivate",
                          "There must be a QCoreApplication instance"))
        return 0;
    readThread = new BTerminalThread;
    readThread->start();
    return readThread;
}

void BTerminalPrivate::removeThread()
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

void BTerminalPrivate::resetColor()
{
#if defined(Q_OS_UNIX)
    QString s = "\e[";
    if (BTerminal::DefaultColor != textColor)
    {
        s += "0;3" + QString::number(textColor);
        if (BTerminal::DefaultColor != backgroundColor)
            s += ";4" + QString::number(backgroundColor);
    }
    else if (BTerminal::DefaultColor != backgroundColor)
    {
        s += "7;3" + QString::number(backgroundColor);
    }
    else
    {
        s += "0";
    }
    s += "m";
    BTerminal::write(s);
#elif defined (Q_OS_WIN)
    init_once(WORD, DefCol, WORD())
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        DefCol = csbi.wAttributes;
    }
    WORD c = DefCol;
    if (BTerminal::DefaultColor != textColor)
    {
        c = c & ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        switch (textColor)
        {
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
    if (BTerminal::DefaultColor != backgroundColor)
    {
        c = c & ~(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
        switch (backgroundColor)
        {
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

/*============================== Public methods ============================*/

void BTerminalPrivate::init()
{
    root = 0;
    translations = true;
    switch (Mode)
    {
    case BTerminal::StandardMode:
    {
        BTerminalThread *t = initThread();
        if (!t)
            return;
        t->disconnect(SIGNAL(lineRead(QString)));
        connect(t, SIGNAL(lineRead(QString)), this, SLOT(lineRead(QString)), Qt::QueuedConnection);
        break;
    }
    default:
        break;
    }
}

/*============================== Public slots ==============================*/

void BTerminalPrivate::lineRead(const QString &text)
{
    lastArgs = BTextTools::splitCommand(text);
    lastCommand = !lastArgs.isEmpty() ? lastArgs.takeFirst() : QString();
    if (lastCommand.isEmpty())
        return;
    B_Q(BTerminal);
    QMetaObject::invokeMethod(q, "commandEntered", Q_ARG(QString, lastCommand), Q_ARG(QStringList, lastArgs));
    if (internalHandlers.contains(lastCommand))
        (q->*internalHandlers.value(lastCommand))(lastCommand, lastArgs);
    else if (externalHandlers.contains(lastCommand))
        externalHandlers.value(lastCommand)(q, lastCommand, lastArgs);
    else
        q->writeLine(translations ? tr("Unknown command") : QString("Unknown command"));
}

/*============================== Static public variables ===================*/

QMutex BTerminalPrivate::colorMutex;
QMutex BTerminalPrivate::echoMutex;
QMutex BTerminalPrivate::titleMutex;
QMutex BTerminalPrivate::readMutex;
QMutex BTerminalPrivate::writeMutex;
QMutex BTerminalPrivate::writeErrMutex;
QTextStream BTerminalPrivate::writeStream(stdout, QIODevice::WriteOnly);
QTextStream BTerminalPrivate::writeErrStream(stderr, QIODevice::WriteOnly);
BTerminalThread *BTerminalPrivate::readThread = 0;
QMutex BTerminalPrivate::threadMutex;
BTerminal::Color BTerminalPrivate::textColor = BTerminal::DefaultColor;
BTerminal::Color BTerminalPrivate::backgroundColor = BTerminal::DefaultColor;
BTerminal::Mode BTerminalPrivate::mode = BTerminal::NoMode;

/*============================================================================
================================ BTerminal ===================================
============================================================================*/

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

void BTerminal::setMode(Mode mode)
{
    if (mode == BTerminalPrivate::mode)
        return;
    if (_m_self)
        destroy();
    if (NoMode == mode)
        return;
    _m_self = new BTerminal(mode);
}

BTerminal::Mode BTerminal::mode()
{
    return BTerminalPrivate::mode;
}

void BTerminal::destroy()
{
    delete _m_self;
    _m_self = 0;
}

QString BTerminal::command(StandardCommand cmd)
{
    QStringList sl = commands(cmd);
    return !sl.isEmpty() ? sl.first() : QString();
}

QStringList BTerminal::commands(StandardCommand cmd)
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

BTerminal::CommandHelp BTerminal::commandHelp(StandardCommand cmd)
{
    CommandHelpList l = commandHelpList(cmd);
    return !l.isEmpty() ? l.first() : CommandHelp();
}

BTerminal::CommandHelpList BTerminal::commandHelpList(StandardCommand cmd)
{
    CommandHelpList l;
    CommandHelp h;
    switch (cmd)
    {
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
    default:
        break;
    }
    return l;
}

BTerminal::InternalHandler BTerminal::handler(StandardCommand cmd)
{
    switch (cmd)
    {
    case QuitCommand:
        return &BTerminal::handleQuit;
    case SetCommand:
        return &BTerminal::handleSet;
    case HelpCommand:
        return &BTerminal::handleHelp;
    default:
        return 0;
    }
}

void BTerminal::connectToCommandEntered(QObject *receiver, const char *method)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    if (!receiver || !method)
        return;
    connect(_m_self, SIGNAL(commandEntered(QString,QStringList)), receiver, method);
}

void BTerminal::disconnectFromCommandEntered(QObject *receiver, const char *method)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    if (!receiver || !method)
        return;
    disconnect(_m_self, SIGNAL(commandEntered(QString,QStringList)), receiver, method);
}

QString BTerminal::readLine(const QString &text)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return QString();
    if (StandardMode != ds_func()->Mode)
        return QString();
    if (!text.isEmpty())
        write(text);
    BTerminalThread *t = BTerminalPrivate::initThread(true);
    QMutexLocker locker(&BTerminalPrivate::readMutex);
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

QString BTerminal::readLineSecure(const QString &text)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return QString();
    if (StandardMode != ds_func()->Mode)
        return QString();
    setStdinEchoEnabled(false);
    QString line = readLine(text);
    setStdinEchoEnabled(true);
    writeLine();
    return line;
}

void BTerminal::write(const QString &text)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    if (text.isEmpty())
        return;
    QMutexLocker locker(&BTerminalPrivate::writeMutex);
    BTerminalPrivate::writeStream << text;
    BTerminalPrivate::writeStream.flush();
}

void BTerminal::writeLine(const QString &text)
{
    write(text + "\n");
}

void BTerminal::writeErr(const QString &text)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    QMutexLocker locker(&BTerminalPrivate::writeErrMutex);
    BTerminalPrivate::writeErrStream << text;
    BTerminalPrivate::writeErrStream.flush();
}

void BTerminal::writeLineErr(const QString &text)
{
    writeErr(text + "\n");
}

void BTerminal::writeHelpLine(const QString &usage, const QString &description)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
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

void BTerminal::writeHelpLine(const QString &usage, const BTranslation &description, bool translate)
{
    writeHelpLine(usage, translate ? description.translate() : description.sourceText());
}

void BTerminal::writeHelpLines(const CommandHelpList &list, bool translate)
{
    foreach (const CommandHelp &h, list)
        writeHelpLine(h.usage, h.description, translate);
}

void BTerminal::setStdinEchoEnabled(bool enabled)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    QMutexLocker locker(&BTerminalPrivate::echoMutex);
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

void BTerminal::setTitle(const QString &title)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    QMutexLocker locker1(&BTerminalPrivate::titleMutex);
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    QMutexLocker locker2(&BTerminalPrivate::writeMutex);
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

void BTerminal::setTextColor(Color color)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    QMutexLocker locker(&BTerminalPrivate::colorMutex);
    BTerminalPrivate::textColor = color;
    BTerminalPrivate::resetColor();
}

void BTerminal::setBackgroundColor(Color color)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    QMutexLocker locker(&BTerminalPrivate::colorMutex);
    BTerminalPrivate::backgroundColor = color;
    BTerminalPrivate::resetColor();
}

BTerminal::Color BTerminal::textColor()
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return DefaultColor;
    if (StandardMode != ds_func()->Mode)
        return DefaultColor;
    QMutexLocker locker(&BTerminalPrivate::colorMutex);
    return BTerminalPrivate::textColor;
}

BTerminal::Color BTerminal::backgroundColor()
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return DefaultColor;
    if (StandardMode != ds_func()->Mode)
        return DefaultColor;
    QMutexLocker locker(&BTerminalPrivate::colorMutex);
    return BTerminalPrivate::backgroundColor;
}

QSize BTerminal::size()
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return QSize();
    if (StandardMode != ds_func()->Mode)
        return QSize();
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

int BTerminal::columnCount()
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return 0;
    if (StandardMode != ds_func()->Mode)
        return 0;
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

int BTerminal::rowCount()
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return 0;
    if (StandardMode != ds_func()->Mode)
        return 0;
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

void BTerminal::installHandler(const QString &command, InternalHandler handler)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    if (command.isEmpty() || !handler)
        return;
    B_DS(BTerminal);
    if (ds->internalHandlers.contains(command))
        return;
    ds->internalHandlers.insert(command, handler);
}

void BTerminal::installHandler(const QString &command, ExternalHandler handler)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    if (command.isEmpty() || !handler)
        return;
    B_DS(BTerminal);
    if (ds->externalHandlers.contains(command))
        return;
    ds->externalHandlers.insert(command, handler);
}

void BTerminal::installHandler(StandardCommand cmd)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    InternalHandler h = handler(cmd);
    CommandHelpList l = commandHelpList(cmd);
    foreach (const QString &s, commands(cmd))
    {
        installHandler(s, h);
        setCommandHelp(s, l);
    }
}

void BTerminal::installHandler(StandardCommand cmd, InternalHandler handler)
{
    foreach (const QString &s, commands(cmd))
        installHandler(s, handler);
}

void BTerminal::installHandler(StandardCommand cmd, ExternalHandler handler)
{
    foreach (const QString &s, commands(cmd))
        installHandler(s, handler);
}

QString BTerminal::lastCommand(QStringList *args)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return QString();
    if (StandardMode != ds_func()->Mode)
        return QString();
    return bRet(args, ds_func()->lastArgs, ds_func()->lastCommand);
}

void BTerminal::setRootSettingsNode(BSettingsNode *root)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    B_DS(BTerminal);
    if (ds->root)
        delete ds->root;
    ds->root = root;
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

void BTerminal::setHelpDescription(const BTranslation &t)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    ds_func()->help = t;
}

void BTerminal::setCommandHelp(const QString &command, const CommandHelp &help)
{
    setCommandHelp(command, CommandHelpList() << help);
}

void BTerminal::setCommandHelp(const QString &command, const CommandHelpList &list)
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return;
    if (StandardMode != ds_func()->Mode)
        return;
    if (command.isEmpty())
        return;
    ds_func()->commandHelp.insert(command, list);
}

BSettingsNode *BTerminal::rootSettingsNode()
{
    if (!BTerminalPrivate::testInit("BTerminal"))
        return 0;
    if (StandardMode != ds_func()->Mode)
        return 0;
    return ds_func()->root;
}

/*============================== Protected methods =========================*/

bool BTerminal::handleQuit(const QString &, const QStringList &)
{
    QCoreApplication::quit();
    return true;
}

bool BTerminal::handleSet(const QString &, const QStringList &args)
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

bool BTerminal::handleHelp(const QString &, const QStringList &args)
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

BTerminal *BTerminal::_m_self = 0;
