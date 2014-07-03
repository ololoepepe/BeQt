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
    explicit BTerminalThread();
    ~BTerminalThread();
protected:
    void run();
signals:
    void lineRead(const QString &text);
public:
    QTextStream readStream;
    QEventLoop readLoop;
    QString lastLine;
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
    explicit BTerminalPrivate(BTerminal *q);
    ~BTerminalPrivate();
public:
    static bool testInit(const char *where = 0);
    static bool testUnique();
    static BTerminalThread *initThread(bool silent = false);
    static void removeThread();
    static void resetColor();
public:
    void init();
public Q_SLOTS:
    void lineRead(const QString &text);
public:
    static QMutex colorMutex;
    static QMutex echoMutex;
    static QMutex titleMutex;
    static QMutex readMutex;
    static QMutex writeMutex;
    static QMutex writeErrMutex;
    static QTextStream writeStream;
    static QTextStream writeErrStream;
    static BTerminalThread *readThread;
    static QMutex threadMutex;
    static BTerminal::Color textColor;
    static BTerminal::Color backgroundColor;
public:
    QMap<QString, BTerminal::InternalHandler> internalHandlers;
    QMap<QString, BTerminal::ExternalHandler> externalHandlers;
    QString lastCommand;
    QStringList lastArgs;
    BSettingsNode *root;
    bool translations;
    BTranslation help;
    QMap<QString, BTerminal::CommandHelpList> commandHelp;
private:
    Q_DISABLE_COPY(BTerminalPrivate)
    friend class BTerminalThread;
};

#endif // BTERMINAL_P_H
