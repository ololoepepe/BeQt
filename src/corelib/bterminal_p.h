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

class QEventLoop;

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
    QString lastLine;
    QEventLoop *loop;
    QTextStream readStream;
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
    static BTerminal::Mode mode;
    static QMutex mutex;
    static BTerminal::Color textColor;
    static QTextStream writeErrStream;
    static QTextStream writeStream;
public:
    const BTerminal::Mode Mode;
public:
    QMap<QString, BTerminal::CommandHelpList> commandHelp;
    QStringList commandHistory;
    QMap<QString, BTerminal::ExternalHandler> externalHandlers;
    BTranslation help;
    QMap<QString, BTerminal::InternalHandler> internalHandlers;
    QStringList lastArgs;
    QString lastCommand;
    BTerminalThread *readThread;
    BSettingsNode *root;
    bool translations;
public:
    explicit BTerminalPrivate(BTerminal *q, BTerminal::Mode m);
    ~BTerminalPrivate();
public:
    static void resetColor();
    static bool testInit(const char *where = 0);
public:
    void init();
public Q_SLOTS:
    void lineRead(const QString &text);
private:
    Q_DISABLE_COPY(BTerminalPrivate)
};

#endif // BTERMINAL_P_H
