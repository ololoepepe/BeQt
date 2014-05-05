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

#ifndef BTERMINALIOHANDLER_H
#define BTERMINALIOHANDLER_H

class BTerminalIOHandlerPrivate;
class BSettingsNode;

class QString;
class QStringLit;

#include "bglobal.h"
#include "bbase.h"
#include "btranslation.h"

#include <QObject>

#define bReadLine BTerminalIOHandler::readLine
#define bReadLineSecure BTerminalIOHandler::readLineSecure
#define bWrite BTerminalIOHandler::write
#define bWriteLine BTerminalIOHandler::writeLine
#define bWriteErr BTerminalIOHandler::writeErr
#define bWriteLineErr BTerminalIOHandler::writeLineErr

/*============================================================================
================================ BTerminalIOHandler ==========================
============================================================================*/

class B_CORE_EXPORT BTerminalIOHandler : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTerminalIOHandler)
    B_DECLARE_PRIVATE_S(BTerminalIOHandler)
public:
    enum StandardCommand
    {
        QuitCommand,
        SetCommand,
        HelpCommand
    };
    enum Color
    {
        DefaultColor = -1,
        Black = 0,
        Red,
        Green,
        Brown,
        Blue,
        Magenta,
        Cyan,
        Lightgray
    };
public:
    struct CommandHelp
    {
        QString usage;
        BTranslation description;
    };
public:
    typedef QList<CommandHelp> CommandHelpList;
    typedef bool (BTerminalIOHandler::*InternalHandler)(const QString &cmd, const QStringList &args);
    typedef bool (*ExternalHandler)(BTerminalIOHandler *, const QString &cmd, const QStringList &args);
public:
    explicit BTerminalIOHandler(QObject *parent = 0);
    ~BTerminalIOHandler();
protected:
    explicit BTerminalIOHandler(BTerminalIOHandlerPrivate &d, QObject *parent = 0);
public:
    static QString command(StandardCommand cmd);
    static QStringList commands(StandardCommand cmd);
    static CommandHelp commandHelp(StandardCommand cmd);
    static CommandHelpList commandHelpList(StandardCommand cmd);
    static InternalHandler handler(StandardCommand cmd);
    static BTerminalIOHandler *instance();
    static QStringList splitCommand(const QString &command);
    static QString mergeArguments(const QStringList &arguments);
    static QString readLine(const QString &text = QString());
    static QString readLineSecure(const QString &text = QString());
    static void write(const QString &text);
    static void writeLine( const QString &text = QString() );
    static void writeErr(const QString &text);
    static void writeLineErr( const QString &text = QString() );
    static void writeHelpLine(const QString &usage, const QString &description);
    static void writeHelpLine(const QString &usage, const BTranslation &description, bool translate = true);
    static void writeHelpLines(const CommandHelpList &list, bool translate = true);
    static void setStdinEchoEnabled(bool enabled = true);
    static void setTerminalTitle(const QString &title);
    static void setTextColor(Color color);
    static void setBackgroundColor(Color color);
    static Color textColor();
    static Color backgroundColor();
    static void installHandler(const QString &command, InternalHandler handler);
    static void installHandler(const QString &command, ExternalHandler handler);
    static void installHandler(StandardCommand cmd);
    static void installHandler(StandardCommand cmd, InternalHandler handler);
    static void installHandler(StandardCommand cmd, ExternalHandler handler);
    static QString lastCommand(QStringList *args = 0);
    static void setRootSettingsNode(BSettingsNode *root);
    static BSettingsNode *createBeQtSettingsNode(BSettingsNode *parent = rootSettingsNode());
    static void setTranslationsEnabled(bool enabled);
    static void setHelpDescription(const BTranslation &t);
    static void setCommandHelp(const QString &command, const CommandHelp &help);
    static void setCommandHelp(const QString &command, const CommandHelpList &list);
    static BSettingsNode *rootSettingsNode();
    static bool translationsEnabled();
protected:
    virtual bool handleCommand(const QString &command, const QStringList &arguments);
    bool handleQuit(const QString &command, const QStringList &arguments);
    bool handleSet(const QString &command, const QStringList &arguments);
    bool handleHelp(const QString &command, const QStringList &arguments);
Q_SIGNALS:
    void commandEntered(const QString &command, const QStringList &arguments);
protected:
    static BTerminalIOHandler *_m_self;
private:
    Q_DISABLE_COPY(BTerminalIOHandler)
};

#endif // BTERMINALIOHANDLER_H
