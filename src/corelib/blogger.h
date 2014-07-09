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

#ifndef BLOGGER_H
#define BLOGGER_H

class BLoggerPrivate;

#include "bbaseobject.h"

#include <QObject>
#include <QString>

/*============================================================================
================================ BLogger =====================================
============================================================================*/

class B_CORE_EXPORT BLogger : public QObject, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BLogger)
public:
    enum Level
    {
        //This order reflects the importance of each level of messages
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
    static bool isStderrLevel(Level lvl);
    static QString levelToString(Level lvl);
public:
    QString dateTimeFormat() const;
    int fileFlushInterval() const;
    QString fileName() const;
    bool isDateTimeIncluded() const;
    bool isLevelIncluded() const;
    bool isLogToConsoleEnabled() const;
    bool isLogToFileEnabled() const;
    bool isStderrUsed() const;
    void log(const QString &text, Level lvl);
    void setDateTimeFormat(const QString &format);
    void setFileFlushInterval(int msecs);
    void setFileName(const QString &fileName);
    void setIncludeDateTime(bool b);
    void setIncludeLevel(bool b);
    void setLogToConsoleEnabled(bool enabled);
    void setLogToFileEnabled(bool enabled);
    void setUseStderr(bool b);
public Q_SLOTS:
    void flushFile();
    void log(const QString &text);
    void logCritical(const QString &text);
    void logDebug(const QString &text);
    void logFatal(const QString &text);
    void logInfo(const QString &text);
    void logTrace(const QString &text);
    void logWarning(const QString &text);
protected:
    virtual void userLog(const QString &text, Level level);
private:
    Q_DISABLE_COPY(BLogger)
};

#endif // BLOGGER_H
