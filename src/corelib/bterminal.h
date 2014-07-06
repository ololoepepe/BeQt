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

#ifndef BTERMINAL_H
#define BTERMINAL_H

class BTerminalPrivate;

class BSettingsNode;

class QStringList;
class QSize;

#include "bbaseobject.h"
#include "btranslation.h"

#include <QObject>
#include <QString>

#define bReadLine BTerminal::readLine
#define bReadLineSecure BTerminal::readLineSecure
#define bWrite BTerminal::write
#define bWriteLine BTerminal::writeLine
#define bWriteErr BTerminal::writeErr
#define bWriteLineErr BTerminal::writeLineErr

/*============================================================================
================================ BTerminal ==================================
============================================================================*/

class B_CORE_EXPORT BTerminal : public QObject, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTerminal)
    B_DECLARE_PRIVATE_S(BTerminal)
public:
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
    enum Mode
    {
        NoMode = 0,
        StandardMode
    };
    enum StandardCommand
    {
        HelpCommand,
        LastCommand,
        QuitCommand,
        SetCommand
    };
public:
    struct CommandHelp
    {
        BTranslation description;
        QString usage;
    };
public:
    typedef QList<CommandHelp> CommandHelpList;
    typedef bool (*ExternalHandler)(BTerminal *, const QString &cmd, const QStringList &args);
    typedef bool (BTerminal::*InternalHandler)(const QString &cmd, const QStringList &args);
protected:
    static BTerminal *_m_self;
protected:
    explicit BTerminal(Mode mode);
    explicit BTerminal(BTerminalPrivate &d);
    ~BTerminal();
public:
    static Color backgroundColor();
    static int columnCount();
    static QString command(StandardCommand cmd);
    static CommandHelp commandHelp(StandardCommand cmd);
    static CommandHelpList commandHelpList(StandardCommand cmd);
    static QStringList commandHistory();
    static QStringList commands(StandardCommand cmd);
    static void connectToCommandEntered(QObject *receiver, const char *method);
    static BSettingsNode *createBeQtSettingsNode(BSettingsNode *parent = rootSettingsNode());
    static void destroy();
    static void disconnectFromCommandEntered(QObject *receiver, const char *method);
    static InternalHandler handler(StandardCommand cmd);
    static void installHandler(const QString &command, InternalHandler handler);
    static void installHandler(const QString &command, ExternalHandler handler);
    static void installHandler(StandardCommand cmd);
    static void installHandler(StandardCommand cmd, InternalHandler handler);
    static void installHandler(StandardCommand cmd, ExternalHandler handler);
    static QString lastCommand(QStringList *args = 0);
    static Mode mode();
    static QString readLine(const QString &text = QString());
    static QString readLineSecure(const QString &text = QString());
    static BSettingsNode *rootSettingsNode();
    static int rowCount();
    static void setBackgroundColor(Color color);
    static void setCommandHelp(const QString &command, const CommandHelp &help);
    static void setCommandHelp(const QString &command, const CommandHelpList &list);
    static void setCommandHistory(const QStringList &list);
    static void setHelpDescription(const BTranslation &t);
    static void setMode(Mode mode);
    static void setRootSettingsNode(BSettingsNode *root);
    static void setStdinEchoEnabled(bool enabled = true);
    static void setTextColor(Color color);
    static void setTitle(const QString &title);
    static void setTranslationsEnabled(bool enabled);
    static QSize size();
    static Color textColor();
    static bool translationsEnabled();
    static void write(const QString &text);
    static void writeErr(const QString &text);
    static void writeHelpLine(const QString &usage, const QString &description);
    static void writeHelpLine(const QString &usage, const BTranslation &description, bool translate = true);
    static void writeHelpLines(const CommandHelpList &list, bool translate = true);
    static void writeLine( const QString &text = QString() );
    static void writeLineErr( const QString &text = QString() );
protected:
    bool handleHelp(const QString &command, const QStringList &arguments);
    bool handleLast(const QString &command, const QStringList &arguments);
    bool handleQuit(const QString &command, const QStringList &arguments);
    bool handleSet(const QString &command, const QStringList &arguments);
Q_SIGNALS:
    void commandEntered(const QString &command, const QStringList &arguments);
private:
    Q_DISABLE_COPY(BTerminal)
};

#endif // BTERMINAL_H
