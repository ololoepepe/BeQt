#include "blogger.h"

#include "bglobal.h"
#include "bbase.h"
#include "bbase_p.h"

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

class BLoggerPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BLogger)
public:
    explicit BLoggerPrivate(BLogger *q);
    ~BLoggerPrivate();
public:
    static QString levelToString(BLogger::Level lvl);
public:
    void init();
    void tryLogToConsole(const QString &text);
    void tryLogToFile(const QString &text);
    void tryLogToRemote(const QString &text);
    QString constructMessage(const QString &text, BLogger::Level lvl) const;
public:
    bool includeLevel;
    bool includeDateTime;
    QString format;
    QString hostName;
    quint16 port;
    bool logToConsole;
    bool logToFile;
    bool logToRemote;
    QFile file;
    QFile stdoutWrapper;
    QTextStream consoleStream;
    QTextStream fileStream;
    mutable QMutex formatMutex;
    mutable QMutex consoleMutex;
    mutable QMutex fileMutex;
    mutable QMutex remoteMutex;
private:
    Q_DISABLE_COPY(BLoggerPrivate)
};

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
    //
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

/*============================== Public methods ============================*/

void BLoggerPrivate::init()
{
    includeLevel = true;
    includeDateTime = true;
    format = "dd/MMM/yyy hh:mm:ss";
    logToConsole = true;
    logToFile = true;
    logToRemote = true;
    stdoutWrapper.open(stdout, QFile::WriteOnly);
    consoleStream.setDevice(&stdoutWrapper);
}

void BLoggerPrivate::tryLogToConsole(const QString &text)
{
    QMutexLocker locker(&consoleMutex);
    if (!logToConsole)
        return;
    consoleStream << text;
}

void BLoggerPrivate::tryLogToFile(const QString &text)
{
    QMutexLocker locker(&fileMutex);
    if ( !logToFile || !file.isOpen() )
        return;
    fileStream << text;
    fileStream.flush();
}

void BLoggerPrivate::tryLogToRemote(const QString &text)
{
    QMutexLocker locker(&remoteMutex);
    if (!logToRemote)
        return;
}

QString BLoggerPrivate::constructMessage(const QString &text, BLogger::Level lvl) const
{
    QMutexLocker locker(&formatMutex);
    QString msg;
    if ( includeDateTime && !format.isEmpty() )
        msg += "[" + QDateTime::currentDateTime().toString(format) + "] ";
    if (includeLevel)
    {
        QString level = levelToString(lvl);
        if ( !level.isEmpty() )
            msg += level + ": ";
    }
    msg += text;
    return msg;
}

/*============================================================================
================================ BLogger =====================================
============================================================================*/

/*============================== Public constructors =======================*/

BLogger::BLogger() :
    BBase( *new BLoggerPrivate(this) )
{
    d_func()->init();
}

BLogger::BLogger(const QString &fileName, const QString &hostName, quint16 port) :
    BBase( *new BLoggerPrivate(this) )
{
    d_func()->init();
    setFileName(fileName);
    setRemote(hostName, port);
}

BLogger::BLogger(const QString &hostName, quint16 port, const QString &fileName) :
    BBase( *new BLoggerPrivate(this) )
{
    d_func()->init();
    setFileName(fileName);
    setRemote(hostName, port);
}

BLogger::~BLogger()
{
    //
}

/*============================== Protected constructors ====================*/

BLogger::BLogger(BLoggerPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

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

void BLogger::setLogToRemoteEnabled(bool enabled)
{
    B_D(BLogger);
    QMutexLocker locker(&d->remoteMutex);
    d->logToRemote = enabled;
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
        return;
    d->fileStream.setDevice(&d->file);
}

void BLogger::setRemote(const QString &hostName, quint16 port)
{
    B_D(BLogger);
    QMutexLocker locker(&d->remoteMutex);
    d->hostName = hostName;
    d->port = port;
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

bool BLogger::isLogToRemoteEnabled() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->remoteMutex);
    return d->logToRemote;
}

QString BLogger::fileName() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->fileMutex);
    return d->file.fileName();
}

QString BLogger::hostName() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->remoteMutex);
    return d->hostName;
}

quint16 BLogger::port() const
{
    const B_D(BLogger);
    QMutexLocker locker(&d->remoteMutex);
    return d->port;
}

void BLogger::log(const QString &text, Level lvl)
{
    B_D(BLogger);
    QString msg = d->constructMessage(text, lvl);
    d->tryLogToConsole(msg);
    d->tryLogToFile(msg);
    d->tryLogToRemote(msg);
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
