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

#ifndef BTERMINAL_P_H
#define BTERMINAL_P_H

class BTerminalPrivate;

class BSettingsNode;

#include "bterminal.h"

#include "bbaseobject_p.h"
#include "btranslation.h"

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QThread>

/*============================================================================
================================ BTerminalThread =============================
============================================================================*/

class B_CORE_EXPORT BTerminalThread : public QThread
{
    Q_OBJECT
public:
    BTerminalPrivate * const TerminalPrivate;
public:
    explicit BTerminalThread(BTerminalPrivate *tp);
    ~BTerminalThread();
protected:
    void run();
private:
    Q_DISABLE_COPY(BTerminalThread)
};

/*============================================================================
================================ BTerminalPrivate ============================
============================================================================*/

class B_CORE_EXPORT BTerminalPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BTerminal)
    B_DECLARE_PUBLIC_S(BTerminal)
public:
    static BTerminal::Color backgroundColor;
    static QMap<QString, BTerminal::CommandHelpList> commandHelp;
    static QStringList commandHistory;
    static BTerminal::HandlerFunction defaultHandler;
    static QMap<QString, BTerminal::HandlerFunction> handlers;
    static BTranslation help;
    static QStringList lastArgs;
    static QString lastCommand;
    static QString line;
    static QMutex lineMutex;
    static bool lineWasRead;
    static BTerminal::Mode mode;
    static QMutex mutex;
    static bool readLine;
    static QMutex readLineMutex;
    static QTextStream readStream;
    static BTerminalThread *readThread;
    static BSettingsNode *root;
    static BTerminal::Color textColor;
    static bool translations;
    static QTextStream writeErrStream;
    static QTextStream writeStream;
public:
    const BTerminal::Mode Mode;
public:
    explicit BTerminalPrivate(BTerminal *q, BTerminal::Mode m);
    ~BTerminalPrivate();
public:
    static void resetColor();
    static bool testInit(const char *where = 0);
public:
    void init();
public Q_SLOTS:
    void commandEntered(const QString &cmd, const QStringList &args);
    void createThread();
    void destroyThread();
    void lineRead(const QString &text);
private:
    Q_DISABLE_COPY(BTerminalPrivate)
};

#endif // BTERMINAL_P_H
