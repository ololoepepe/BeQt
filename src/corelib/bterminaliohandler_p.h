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

#ifndef BTERMINALIOHANDLER_P_H
#define BTERMINALIOHANDLER_P_H

class BTerminalIOHandlerPrivate;
class BSettingsNode;

#include "bterminaliohandler.h"
#include "bglobal.h"
#include "bbase_p.h"
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
================================ BTerminalIOHandlerThread ====================
============================================================================*/

class BTerminalIOHandlerThread : public QThread
{
    Q_OBJECT
public:
    explicit BTerminalIOHandlerThread();
    ~BTerminalIOHandlerThread();
protected:
    void run();
signals:
    void lineRead(const QString &text);
public:
    QTextStream readStream;
    QEventLoop readLoop;
    QString lastLine;
private:
    Q_DISABLE_COPY(BTerminalIOHandlerThread)
};

/*============================================================================
================================ BTerminalIOHandlerPrivate ===================
============================================================================*/

class B_CORE_EXPORT BTerminalIOHandlerPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BTerminalIOHandler)
    B_DECLARE_PUBLIC_S(BTerminalIOHandler)
public:
    explicit BTerminalIOHandlerPrivate(BTerminalIOHandler *q);
    ~BTerminalIOHandlerPrivate();
public:
    static bool testInit(const char *where = 0);
    static bool testUnique();
    static BTerminalIOHandlerThread *initThread(bool silent = false);
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
    static BTerminalIOHandlerThread *readThread;
    static QMutex threadMutex;
    static BTerminalIOHandler::Color textColor;
    static BTerminalIOHandler::Color backgroundColor;
public:
    QMap<QString, BTerminalIOHandler::InternalHandler> internalHandlers;
    QMap<QString, BTerminalIOHandler::ExternalHandler> externalHandlers;
    QString lastCommand;
    QStringList lastArgs;
    BSettingsNode *root;
    bool translations;
    BTranslation help;
    QMap<QString, BTerminalIOHandler::CommandHelpList> commandHelp;
private:
    Q_DISABLE_COPY(BTerminalIOHandlerPrivate)
    friend class BTerminalIOHandlerThread;
};

#endif // BTERMINALIOHANDLER_P_H
