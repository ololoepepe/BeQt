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

#ifndef BLOGGER_P_H
#define BLOGGER_P_H

#include "blogger.h"

#include "bbaseobject_p.h"

#include <QFile>
#include <QMutex>
#include <QString>
#include <QTextStream>
#include <QTimer>

/*============================================================================
================================ BLoggerPrivate ==============================
============================================================================*/

class B_CORE_EXPORT BLoggerPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BLogger)
public:
    explicit BLoggerPrivate(BLogger *q);
    ~BLoggerPrivate();
public:
    QString constructMessage(const QString &text, BLogger::Level lvl) const;
    void init();
    void resetFileFlushTimer();
    void tryLogToConsole(const QString &text, bool stderrLevel);
    void tryLogToFile(const QString &text);
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
    QTimer fileFlushTimer;
    QTextStream fileStream;
    mutable QMutex formatMutex;
    mutable QMutex consoleMutex;
    mutable QMutex fileMutex;
private:
    Q_DISABLE_COPY(BLoggerPrivate)
};

#endif // BLOGGER_P_H
