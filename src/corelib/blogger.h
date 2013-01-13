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
    explicit BLogger();
    explicit BLogger(const QString &fileName, const QString &hostName = QString(), quint16 port = 0);
    explicit BLogger( const QString &hostName, quint16 port, const QString &fileName = QString() );
    ~BLogger();
protected:
    explicit BLogger(BLoggerPrivate &d);
public:
    void setIncludeLevel(bool b);
    void setIncludeDateTime(bool b);
    void setDateTimeFormat(const QString &format);
    void setLogToConsoleEnabled(bool enabled);
    void setLogToFileEnabled(bool enabled);
    void setLogToRemoteEnabled(bool enabled);
    void setFileName(const QString &fileName);
    void setRemote(const QString &hostName, quint16 port);
    bool isLevelIncluded() const;
    bool isDateTimeIncluded() const;
    QString dateTimeFormat() const;
    bool isLogToConsoleEnabled() const;
    bool isLogToFileEnabled() const;
    bool isLogToRemoteEnabled() const;
    QString fileName() const;
    QString hostName() const;
    quint16 port() const;
    void log(const QString &text, Level lvl);
public slots:
    void log(const QString &text);
    void logInfo(const QString &text);
    void logTrace(const QString &text);
    void logDebug(const QString &text);
    void logWarning(const QString &text);
    void logCritical(const QString &text);
    void logFatal(const QString &text);
private:
    Q_DISABLE_COPY(BLogger)
};

#endif // BLOGGER_H

