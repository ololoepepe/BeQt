#include "blogger.h"
#include "blogger_p.h"
#include "bglobal.h"
#include "bbase.h"
#include "bbase_p.h"
#include "bterminaliohandler.h"

#include <QObject>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

/*============================================================================
================================ BLoggerPrivate ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BLoggerPrivate::BLoggerPrivate(BLogger *q) :
    BBasePrivate(q)
{
    //
}

BLoggerPrivate::~BLoggerPrivate()
{
    if ( file.isOpen() )
        file.close();
}

/*============================== Static public methods =====================*/

QString BLoggerPrivate::levelToString(BLogger::Level lvl)
{
    switch (lvl)
    {
    case BLogger::InfoLevel:
        return "INFO";
    case BLogger::TraceLevel:
        return "TRACE";
    case BLogger::DebugLevel:
        return "DEBUG";
    case BLogger::WarningLevel:
        return "WARN";
    case BLogger::CriticalLevel:
        return "ERROR";
    case BLogger::FatalLevel:
        return "FATAL";
    default:
        return "";
    }
}

bool BLoggerPrivate::isStderrLevel(BLogger::Level lvl)
{
    switch (lvl)
    {
    case BLogger::DebugLevel:
    case BLogger::WarningLevel:
    case BLogger::CriticalLevel:
    case BLogger::FatalLevel:
        return true;
    default:
        return false;
    }
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
}

void BLoggerPrivate::tryLog(const QString &msg, bool stderrLevel)
{
    tryLogToConsole(msg, stderrLevel);
    tryLogToFile(msg);
}

void BLoggerPrivate::tryLogToConsole(const QString &text, bool stderrLevel)
{
    QMutexLocker locker(&consoleMutex);
    if (!logToConsole)
        return;
    if (stderrLevel && useStderr)
        BTerminalIOHandler::writeErr(text);
    else
        BTerminalIOHandler::write(text);
}

void BLoggerPrivate::tryLogToFile(const QString &text)
{
    QMutexLocker locker(&fileMutex);
    if ( !logToFile || !file.isOpen() )
        return;
    fileStream << text;
    fileStream.flush();
}

QString BLoggerPrivate::constructMessage(const QString &text, BLogger::Level lvl) const
{
    QMutexLocker locker(&formatMutex);
    QString msg;
    if ( includeDateTime && !format.isEmpty() )
        msg += QDateTime::currentDateTime().toString(format) + " ";
    if (includeLevel)
    {
        QString level = levelToString(lvl);
        if ( !level.isEmpty() )
            msg += level + ": ";
    }
    msg += text + "\n";
    return msg;
}

/*============================================================================
================================ BLogger =====================================
============================================================================*/

/*============================== Public constructors =======================*/

BLogger::BLogger(QObject *parent) :
    QObject(parent), BBase( *new BLoggerPrivate(this) )
{
    d_func()->init();
}

BLogger::BLogger(const QString &fileName, QObject *parent) :
    QObject(parent), BBase( *new BLoggerPrivate(this) )
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
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BLogger::setUseStderr(bool b)
{
    B_D(BLogger);
    QMutexLocker locker(&d->consoleMutex);
    d->useStderr = b;
}

void BLogger::setIncludeLevel(bool b)
{
    B_D(BLogger);
    QMutexLocker locker(&d->formatMutex);
    d->includeLevel = b;
}

void BLogger::setIncludeDateTime(bool b)
{
    B_D(BLogger);
    QMutexLocker locker(&d->formatMutex);
    d->includeDateTime = b;
}

void BLogger::setDateTimeFormat(const QString &format)
{
    B_D(BLogger);
    QMutexLocker locker(&d->formatMutex);
    d->format = format;
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

void BLogger::setFileName(const QString &fileName)
{
    B_D(BLogger);
    QMutexLocker locker(&d->fileMutex);
    if ( d->file.isOpen() )
        d->file.close();
    d->file.setFileName(fileName);
    d->fileStream.setDevice(0);
    if ( fileName.isEmpty() )
        return;
    if ( !d->file.open(QFile::WriteOnly | QFile::Append) )
        return d->file.setFileName("");
    d->fileStream.setDevice(&d->file);
}

bool BLogger::isStderrUsed() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->consoleMutex);
    return d->useStderr;
}

bool BLogger::isLevelIncluded() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->formatMutex);
    return d->includeLevel;
}

bool BLogger::isDateTimeIncluded() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->formatMutex);
    return d->includeDateTime;
}

QString BLogger::dateTimeFormat() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->formatMutex);
    return d->format;
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

QString BLogger::fileName() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->fileMutex);
    return d->file.fileName();
}

void BLogger::log(const QString &text, Level lvl)
{
    B_D(BLogger);
    QString msg = d->constructMessage(text, lvl);
    d->tryLog( msg, BLoggerPrivate::isStderrLevel(lvl) );
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
