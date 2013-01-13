#ifndef BLOGGER_P_H
#define BLOGGER_P_H

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
    virtual void tryLog(const QString &msg);
    void tryLogToConsole(const QString &text);
    void tryLogToFile(const QString &text);
    QString constructMessage(const QString &text, BLogger::Level lvl) const;
public:
    bool includeLevel;
    bool includeDateTime;
    QString format;
    bool logToConsole;
    bool logToFile;
    QFile file;
    QFile stdoutWrapper;
    QTextStream consoleStream;
    QTextStream fileStream;
    mutable QMutex formatMutex;
    mutable QMutex consoleMutex;
    mutable QMutex fileMutex;
private:
    Q_DISABLE_COPY(BLoggerPrivate)
};

#endif // BLOGGER_P_H
