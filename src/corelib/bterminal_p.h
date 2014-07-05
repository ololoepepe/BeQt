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
#include "bglobal.h"
#include "bbaseobject_p.h"
#include "btranslation.h"

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QTextStream>
#include <QFile>
#include <QEventLoop>
#include <QMap>
#include <QString>
#include <QStringList>

/*============================================================================
================================ BTerminalThread =============================
============================================================================*/

class BTerminalThread : public QThread
{
    Q_OBJECT
public:
    explicit BTerminalThread(BTerminalPrivate *tp);
    ~BTerminalThread();
protected:
    void run();
public:
    BTerminalPrivate *const TerminalPrivate;
public:
    QTextStream readStream;
    QString lastLine;
    QEventLoop *loop;
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
    explicit BTerminalPrivate(BTerminal *q, BTerminal::Mode m);
    ~BTerminalPrivate();
public:
    static bool testInit(const char *where = 0);
    static void resetColor();
public:
    void init();
public Q_SLOTS:
    void lineRead(const QString &text);
public:
    static QMutex mutex;
    static QTextStream writeStream;
    static QTextStream writeErrStream;
    static BTerminal::Color textColor;
    static BTerminal::Color backgroundColor;
    static BTerminal::Mode mode;
public:
    const BTerminal::Mode Mode;
public:
    BTerminalThread *readThread;
    QMap<QString, BTerminal::InternalHandler> internalHandlers;
    QMap<QString, BTerminal::ExternalHandler> externalHandlers;
    QString lastCommand;
    QStringList lastArgs;
    QStringList commandHistory;
    BSettingsNode *root;
    bool translations;
    BTranslation help;
    QMap<QString, BTerminal::CommandHelpList> commandHelp;
private:
    Q_DISABLE_COPY(BTerminalPrivate)
    friend class BTerminalThread;
};

#endif // BTERMINAL_P_H
