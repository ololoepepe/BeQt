#ifndef BLOGGER_P_H
#define BLOGGER_P_H

class QTimer;

#include "blogger.h"
#include "bglobal.h"
#include "bbase_p.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>

/*============================================================================
================================ BLoggerPrivate ==============================
============================================================================*/

class B_CORE_EXPORT BLoggerPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BLogger)
public:
    explicit BLoggerPrivate(BLogger *q);
    ~BLoggerPrivate();
public:
    static QString levelToString(BLogger::Level lvl);
public:
    void init();
    virtual void tryLog(const QString &msg, bool stderrLevel);
    void tryLogToConsole(const QString &text, bool stderrLevel);
    void tryLogToFile(const QString &text);
    QString constructMessage(const QString &text, BLogger::Level lvl) const;
    void resetFileFlushTimer();
public Q_SLOTS:
    void timeout();
public:
    bool useStderr;
    bool includeLevel;
    bool includeDateTime;
    QString format;
    bool logToConsole;
    bool logToFile;
    QFile file;
    int fileFlushInterval;
    QTimer *fileFlushTimer;
    QTextStream fileStream;
    mutable QMutex formatMutex;
    mutable QMutex consoleMutex;
    mutable QMutex fileMutex;
private:
    Q_DISABLE_COPY(BLoggerPrivate)
};

#endif // BLOGGER_P_H
