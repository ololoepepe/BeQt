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

#include "blogger.h"
#include "blogger_p.h"
#include "bglobal.h"
#include "bbase.h"
#include "bbase_p.h"
#include "bterminaliohandler.h"
#include "bdirtools.h"
#include "bnamespace.h"

#include <QObject>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QTimer>

/*============================================================================
================================ BLoggerPrivate ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BLoggerPrivate::BLoggerPrivate(BLogger *q) :
    BBaseObjectPrivate(q)
{
    formatMutex = new QMutex(QMutex::Recursive);
    consoleMutex = new QMutex(QMutex::Recursive);
    fileMutex = new QMutex(QMutex::Recursive);
}

BLoggerPrivate::~BLoggerPrivate()
{
    if ( file.isOpen() )
        file.close();
    delete fileFlushTimer;
}

/*============================== Public methods ============================*/

void BLoggerPrivate::init()
{
    useStderr = true;
    includeLevel = true;
    includeDateTime = true;
    format = "dd/MMM/yyy hh:mm:ss";
    logToConsole = true;
    logToFile = true;
    fileFlushInterval = 10 * BeQt::Second; // < 0 - instant; == 0 -never; > 0 - periodically
    fileFlushTimer = new QTimer(this);
    connect( fileFlushTimer, SIGNAL( timeout() ), this, SLOT( timeout() ) );
    resetFileFlushTimer();
}

void BLoggerPrivate::tryLogToConsole(const QString &text, bool stderrLevel)
{
    QMutexLocker locker(consoleMutex);
    if (!logToConsole)
        return;
    if (stderrLevel && useStderr)
        BTerminalIOHandler::writeErr(text);
    else
        BTerminalIOHandler::write(text);
}

void BLoggerPrivate::tryLogToFile(const QString &text)
{
    QMutexLocker locker(fileMutex);
    if ( !logToFile || !file.isOpen() )
        return;
    fileStream << text;
    if (fileFlushInterval < 0)
        fileStream.flush();
}

QString BLoggerPrivate::constructMessage(const QString &text, BLogger::Level lvl) const
{
    QMutexLocker locker(formatMutex);
    QString msg;
    if ( includeDateTime && !format.isEmpty() )
        msg += QDateTime::currentDateTime().toString(format) + " ";
    if (includeLevel)
    {
        QString level = BLogger::levelToString(lvl);
        if ( !level.isEmpty() )
            msg += level + ": ";
    }
    msg += text + "\n";
    return msg;
}

void BLoggerPrivate::resetFileFlushTimer()
{
    fileFlushTimer->stop();
    if (fileFlushInterval > 0)
        fileFlushTimer->start(fileFlushInterval);
}

/*============================== Public slots ==============================*/

void BLoggerPrivate::timeout()
{
    if ( file.isOpen() )
        fileStream.flush();
}

/*============================================================================
================================ BLogger =====================================
============================================================================*/

/*============================== Public constructors =======================*/

BLogger::BLogger(QObject *parent) :
    QObject(parent), BBaseObject( *new BLoggerPrivate(this) )
{
    d_func()->init();
}

BLogger::BLogger(const QString &fileName, QObject *parent) :
    QObject(parent), BBaseObject( *new BLoggerPrivate(this) )
{
    d_func()->init();
    setFileName(fileName);
}

BLogger::~BLogger()
{
    //
}

/*============================== Protected constructors ====================*/

BLogger::BLogger(BLoggerPrivate &d, QObject *parent) :
    QObject(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

bool BLogger::isStderrLevel(Level lvl)
{
    return lvl >= DebugLevel;
}

QString BLogger::levelToString(Level lvl)
{
    switch (lvl)
    {
    case InfoLevel:
        return "INFO";
    case TraceLevel:
        return "TRACE";
    case DebugLevel:
        return "DEBUG";
    case WarningLevel:
        return "WARN";
    case CriticalLevel:
        return "ERROR";
    case FatalLevel:
        return "FATAL";
    default:
        return "";
    }
}

/*============================== Public methods ============================*/

void BLogger::setUseStderr(bool b)
{
    B_D(BLogger);
    QMutexLocker locker(d->consoleMutex);
    d->useStderr = b;
}

void BLogger::setIncludeLevel(bool b)
{
    B_D(BLogger);
    QMutexLocker locker(d->formatMutex);
    d->includeLevel = b;
}

void BLogger::setIncludeDateTime(bool b)
{
    B_D(BLogger);
    QMutexLocker locker(d->formatMutex);
    d->includeDateTime = b;
}

void BLogger::setDateTimeFormat(const QString &format)
{
    B_D(BLogger);
    QMutexLocker locker(d->formatMutex);
    d->format = format;
}

void BLogger::setLogToConsoleEnabled(bool enabled)
{
    B_D(BLogger);
    QMutexLocker locker(d->consoleMutex);
    d->logToConsole = enabled;
}

void BLogger::setLogToFileEnabled(bool enabled)
{
    B_D(BLogger);
    QMutexLocker locker(d->fileMutex);
    d->logToFile = enabled;
}

void BLogger::setFileName(const QString &fileName)
{
    B_D(BLogger);
    QMutexLocker locker(d->fileMutex);
    if ( d->file.isOpen() )
        d->file.close();
    d->file.setFileName(fileName);
    d->fileStream.setDevice(0);
    if ( fileName.isEmpty() )
        return;
    if ( !BDirTools::touch(fileName) || !d->file.open(QFile::WriteOnly | QFile::Append) )
        return d->file.setFileName("");
    d->fileStream.setDevice(&d->file);
}

void BLogger::setFileFlushInterval(int msecs)
{
    if (msecs < 0)
        msecs = -1;
    B_D(BLogger);
    if (msecs == d->fileFlushInterval)
        return;
    d->fileFlushInterval = msecs;
    d->resetFileFlushTimer();
}

bool BLogger::isStderrUsed() const
{
    const B_D(BLogger);
    QMutexLocker locker(d->consoleMutex);
    return d->useStderr;
}

bool BLogger::isLevelIncluded() const
{
    const B_D(BLogger);
    QMutexLocker locker(d->formatMutex);
    return d->includeLevel;
}

bool BLogger::isDateTimeIncluded() const
{
    const B_D(BLogger);
    QMutexLocker locker(d->formatMutex);
    return d->includeDateTime;
}

QString BLogger::dateTimeFormat() const
{
    const B_D(BLogger);
    QMutexLocker locker(d->formatMutex);
    return d->format;
}

bool BLogger::isLogToConsoleEnabled() const
{
    const B_D(BLogger);
    QMutexLocker locker(d->consoleMutex);
    return d->logToConsole;
}

bool BLogger::isLogToFileEnabled() const
{
    const B_D(BLogger);
    QMutexLocker locker(d->fileMutex);
    return d->logToFile;
}

QString BLogger::fileName() const
{
    const B_D(BLogger);
    QMutexLocker locker(d->fileMutex);
    return d->file.fileName();
}

int BLogger::fileFlushInterval() const
{
    return d_func()->fileFlushInterval;
}

void BLogger::log(const QString &text, Level lvl)
{
    B_D(BLogger);
    QString msg = d->constructMessage(text, lvl);
    d->tryLogToConsole(msg, isStderrLevel(lvl));
    d->tryLogToFile(msg);
    userLog(msg, lvl);
}

/*============================== Public slots ==============================*/

void BLogger::log(const QString &text)
{
    log(text, NoLevel);
}

void BLogger::logInfo(const QString &text)
{
    log(text, InfoLevel);
}

void BLogger::logTrace(const QString &text)
{
    log(text, TraceLevel);
}

void BLogger::logDebug(const QString &text)
{
    log(text, DebugLevel);
}

void BLogger::logWarning(const QString &text)
{
    log(text, WarningLevel);
}

void BLogger::logCritical(const QString &text)
{
    log(text, CriticalLevel);
}

void BLogger::logFatal(const QString &text)
{
    log(text, FatalLevel);
}

void BLogger::flushFile()
{
    B_D(BLogger);
    if ( !d->file.isOpen() )
        return;
    d->fileStream.flush();
    d->resetFileFlushTimer();
}

/*============================== Protected methods =========================*/

void BLogger::userLog(const QString &, Level)
{
    //
}
