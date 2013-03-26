#ifndef BLOGGER_H
#define BLOGGER_H

class BLoggerPrivate;

#include "bglobal.h"
#include "bbase.h"

#include <QString>
#include <QObject>

/*============================================================================
================================ BLogger =====================================
============================================================================*/

class B_CORE_EXPORT BLogger : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BLogger)
public:
    enum Level
    {
        NoLevel = 0,
        InfoLevel,
        TraceLevel,
        DebugLevel,
        WarningLevel,
        CriticalLevel,
        FatalLevel
    };
public:
    explicit BLogger(QObject *parent = 0);
    explicit BLogger(const QString &fileName, QObject *parent = 0);
    ~BLogger();
protected:
    explicit BLogger(BLoggerPrivate &d, QObject *parent = 0);
public:
    static bool isStderrLevel(BLogger::Level lvl);
public:
    void setUseStderr(bool b);
    void setIncludeLevel(bool b);
    void setIncludeDateTime(bool b);
    void setDateTimeFormat(const QString &format);
    void setLogToConsoleEnabled(bool enabled);
    void setLogToFileEnabled(bool enabled);
    void setFileName(const QString &fileName);
    void setFileFlushInterval(int msecs);
    bool isStderrUsed() const;
    bool isLevelIncluded() const;
    bool isDateTimeIncluded() const;
    QString dateTimeFormat() const;
    bool isLogToConsoleEnabled() const;
    bool isLogToFileEnabled() const;
    QString fileName() const;
    int fileFlushInterval() const;
    void log(const QString &text, Level lvl);
public slots:
    void log(const QString &text);
    void logInfo(const QString &text);
    void logTrace(const QString &text);
    void logDebug(const QString &text);
    void logWarning(const QString &text);
    void logCritical(const QString &text);
    void logFatal(const QString &text);
    void flushFile();
private:
    Q_DISABLE_COPY(BLogger)
};

#endif // BLOGGER_H
