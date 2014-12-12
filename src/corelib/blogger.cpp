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

#include "bbase.h"
#include "bbase_p.h"
#include "bdirtools.h"
#include "bnamespace.h"
#include "bterminal.h"

#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QTimer>

/*============================================================================
================================ BLoggerPrivate ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BLoggerPrivate::BLoggerPrivate(BLogger *q) :
    BBaseObjectPrivate(q)
{
    //
}

BLoggerPrivate::~BLoggerPrivate()
{
    if (file.isOpen())
        file.close();
}

/*============================== Public methods ============================*/

QString BLoggerPrivate::constructMessage(const QString &text, BLogger::Level lvl) const
{
    QMutexLocker locker(&formatMutex);
    QString msg;
    if (includeDateTime && !format.isEmpty())
        msg += QDateTime::currentDateTime().toString(format) + " ";
    if (includeLevel) {
        QString level = BLogger::levelToString(lvl);
        if (!level.isEmpty())
            msg += level + ": ";
    }
    msg += text + "\n";
    return msg;
}

void BLoggerPrivate::init()
{
    useStderr = true;
    includeLevel = true;
    includeDateTime = true;
    format = "dd/MMM/yyy hh:mm:ss";
    logToConsole = true;
    logToFile = true;
    fileFlushInterval = 10 * BeQt::Second; // < 0 - instant; == 0 -never; > 0 - periodically
    connect(&fileFlushTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    resetFileFlushTimer();
}

void BLoggerPrivate::resetFileFlushTimer()
{
    fileFlushTimer.stop();
    if (fileFlushInterval > 0)
        fileFlushTimer.start(fileFlushInterval);
}

void BLoggerPrivate::tryLogToConsole(const QString &text, bool stderrLevel)
{
    QMutexLocker locker(&consoleMutex);
    if (!logToConsole)
        return;
    if (stderrLevel && useStderr)
        BTerminal::writeErr(text);
    else
        BTerminal::write(text);
}

void BLoggerPrivate::tryLogToFile(const QString &text)
{
    QMutexLocker locker(&fileMutex);
    if (!logToFile || !file.isOpen())
        return;
    fileStream << text;
    if (fileFlushInterval < 0)
        fileStream.flush();
}

/*============================== Public slots ==============================*/

void BLoggerPrivate::timeout()
{
    if (file.isOpen())
        fileStream.flush();
}

/*============================================================================
================================ BLogger =====================================
============================================================================*/

/*!
\class BLogger
\brief The BLogger class provides means of logging for BeQt classes.

This class is used by some BeQt classes for logging to a file or standard output (stdout/stderr). It may also be used
in your applications.

The BApplicationBase holds a global BLogger instance of BLogger. That instance is used internally by BeQt classes.

Example:

\snippet src/corelib/blogger.cpp 0

\sa {Logging}, BRemoteLogger
*/

/*!
\property BLogger::dateTimeFormat

This property gets and sets the BLogger's date and time format, used when formatting the date and time included in
logged messages.

By default, this property contains "dd/MMM/yyy hh:mm:ss".
*/

/*!
\property BLogger::dateTimeIncluded

This property gets and sets the BLogger's flag that determines if date and time must be included to the logged
messages.

By default, this property contains true.
*/

/*!
\property BLogger::fileFlushInterval

This property gets and sets the BLogger's file flush interval (in milliseconds). If the interval is greater than zero,
the text from the internal buffer is written to file periodically using this interval.

By default, this property contains 10 * BeQt::Second (10000 milliseconds, or 10 seconds).
*/

/*!
\property BLogger::fileName

This property gets and sets the BLogger's target log file.

By default, this property contains an empty QString.
*/

/*!
\property BLogger::levelIncluded

This property gets and sets the BLogger's flag that determines if logging level string must be included to the logged
messages.

By default, this property contains true.
*/

/*!
\property BLogger::logToConsoleEnabled

This property gets and sets the BLogger's flag that determines if the messages must be logged to standard output
(stdout/stderr).

By default, this property contains true.
*/

/*!
\property BLogger::logToFileEnabled

This property gets and sets the BLogger's flag that determines if the messages must be logged to s file.

By default, this property contains true.
*/

/*!
\property BLogger::stderrUsed

This property gets and sets the BLogger's flag that determines if the messages with logging level DebugLevel or higher
must be logged to stderr instead of stdout.

By default, this property contains true.
*/

/*!
\enum BLogger::Level

This enum type is used to describe the logging level.

\value NoLevel
No level. Message with no specific level.

\value InfoLevel
Information level. A message containing some information.

\value TraceLevel
Trace level. A message used to trace something.

\value DebugLevel
Debugging level. A message used for debugging purposes.

\value WarningLevel
Warning level. A message use to warn about something.

\value CriticalLevel
Critical error level. A message indicating that something has gone wrong.

\value FatalLevel
Fatal error level. A message indicating that a fatal error occured and the application is about to be terminated.
*/

/*============================== Public constructors =======================*/

/*!
Constructs a logger and sets it's parent object to \a parent.
*/

BLogger::BLogger(QObject *parent) :
    QObject(parent), BBaseObject(*new BLoggerPrivate(this))
{
    d_func()->init();
}

/*!
Constructs a logger. Sets log file name to \a fileName, and parent object to \a parent.
*/

BLogger::BLogger(const QString &fileName, QObject *parent) :
    QObject(parent), BBaseObject(*new BLoggerPrivate(this))
{
    d_func()->init();
    setFileName(fileName);
}

/*!
Destroys the logger.
*/

BLogger::~BLogger()
{
    //
}

/*============================== Protected constructors ====================*/

/*!
Constructs a logger and associates the given data object \a d with it. Sets parent object to \a parent.
*/

BLogger::BLogger(BLoggerPrivate &d, QObject *parent) :
    QObject(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

/*!
Returns true if access level \a lvl is DebugLevel or higher and should be written to stderr instead of stdout when
logging to console. Otherwise returns false.
*/

bool BLogger::isStderrLevel(Level lvl)
{
    return lvl >= DebugLevel;
}

/*!
Returns the string representation of access level \a lvl.

\table
\header
    \li Access level
    \li String representation
\row
    \li NoLevel
    \li (empty string)
\row
    \li InfoLevel
    \li INFO
\row
    \li TraceLevel
    \li TRACE
\row
    \li DebugLevel
    \li DEBUG
\row
    \li WarningLevel
    \li WARN
\row
    \li CriticalLevel
    \li ERROR
\row
    \li FatalLevel
    \li FATAL
\endtable
*/

QString BLogger::levelToString(Level lvl)
{
    switch (lvl) {
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

QString BLogger::dateTimeFormat() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->formatMutex);
    return d->format;
}

int BLogger::fileFlushInterval() const
{
    return d_func()->fileFlushInterval;
}

QString BLogger::fileName() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->fileMutex);
    return d->file.fileName();
}

bool BLogger::isDateTimeIncluded() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->formatMutex);
    return d->includeDateTime;
}

bool BLogger::isLevelIncluded() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->formatMutex);
    return d->includeLevel;
}

bool BLogger::isLogToConsoleEnabled() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->consoleMutex);
    return d->logToConsole;
}

bool BLogger::isLogToFileEnabled() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->fileMutex);
    return d->logToFile;
}

bool BLogger::isStderrUsed() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->consoleMutex);
    return d->useStderr;
}

/*!
Logs \a text with logging level \a lvl.

If logToConsoleEnabled is true, logs to standard output (stdout or stderr, depending on logging level and stderrUsed
property value), and if logToFileEnabled is true, logs to the specified file.

\note By default, \a text is not written to file immediately. It is added to the internal buffer, which is written to
the file every fileFlushInterval milliseconds.

\sa logToConsoleEnabled, logToFileEnabled, fileFlushInterval
*/

void BLogger::log(const QString &text, Level lvl)
{
    B_D(BLogger);
    QString msg = d->constructMessage(text, lvl);
    d->tryLogToConsole(msg, isStderrLevel(lvl));
    d->tryLogToFile(msg);
    userLog(msg, lvl);
}

void BLogger::setDateTimeFormat(const QString &format)
{
    B_D(BLogger);
    QMutexLocker locker(&d->formatMutex);
    d->format = format;
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

void BLogger::setFileName(const QString &fileName)
{
    B_D(BLogger);
    QMutexLocker locker(&d->fileMutex);
    if (d->file.isOpen())
        d->file.close();
    d->file.setFileName(fileName);
    d->fileStream.setDevice(0);
    if (fileName.isEmpty())
        return;
    if (!BDirTools::touch(fileName) || !d->file.open(QFile::WriteOnly | QFile::Append))
        return d->file.setFileName("");
    d->fileStream.setDevice(&d->file);
}

void BLogger::setIncludeDateTime(bool b)
{
    B_D(BLogger);
    QMutexLocker locker(&d->formatMutex);
    d->includeDateTime = b;
}

void BLogger::setIncludeLevel(bool b)
{
    B_D(BLogger);
    QMutexLocker locker(&d->formatMutex);
    d->includeLevel = b;
}

void BLogger::setLogToConsoleEnabled(bool enabled)
{
    B_D(BLogger);
    QMutexLocker locker(&d->consoleMutex);
    d->logToConsole = enabled;
}

void BLogger::setLogToFileEnabled(bool enabled)
{
    B_D(BLogger);
    QMutexLocker locker(&d->fileMutex);
    d->logToFile = enabled;
}

void BLogger::setUseStderr(bool b)
{
    B_D(BLogger);
    QMutexLocker locker(&d->consoleMutex);
    d->useStderr = b;
}

/*============================== Public slots ==============================*/

/*!
When logToFileEnabled property is true and fileName is set, calling this method flushes all pending data to the target
log file.

\sa logToFileEnabled, fileName
*/

void BLogger::flushFile()
{
    B_D(BLogger);
    if (!d->file.isOpen())
        return;
    d->fileStream.flush();
    d->resetFileFlushTimer();
}

/*!
Logs \a text with logging level NoLevel.

This function is just a wrapper over the log() slot.

\sa log()
*/

void BLogger::log(const QString &text)
{
    log(text, NoLevel);
}

/*!
Logs \a text with logging level CriticalLevel.

This function is just a wrapper over the log() slot.

\sa log()
*/

void BLogger::logCritical(const QString &text)
{
    log(text, CriticalLevel);
}

/*!
Logs \a text with logging level DebugLevel.

This function is just a wrapper over the log() slot.

\sa log()
*/

void BLogger::logDebug(const QString &text)
{
    log(text, DebugLevel);
}

/*!
Logs \a text with logging level FatalLevel.

This function is just a wrapper over the log() slot.

\sa log()
*/

void BLogger::logFatal(const QString &text)
{
    log(text, FatalLevel);
}

/*!
Logs \a text with logging level InfoLevel.

This function is just a wrapper over the log() slot.

\sa log()
*/

void BLogger::logInfo(const QString &text)
{
    log(text, InfoLevel);
}

/*!
Logs \a text with logging level TraceLevel.

This function is just a wrapper over the log() slot.

\sa log()
*/

void BLogger::logTrace(const QString &text)
{
    log(text, TraceLevel);
}

/*!
Logs \a text with logging level WarningLevel.

This function is just a wrapper over the log() slot.

\sa log()
*/

void BLogger::logWarning(const QString &text)
{
    log(text, WarningLevel);
}

/*============================== Protected methods =========================*/

/*!
This virtual function is called from the log() method. It may be reimplemented to provide custom functionality to
BLogger subclasses.

Typically, this function should log \a text with logging level \a level to some target.

The default implementation does nothing.
*/

void BLogger::userLog(const QString &, Level)
{
    //
}
