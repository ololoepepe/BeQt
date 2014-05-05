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

#ifndef BSIGNALDELAYPROXY_H
#define BSIGNALDELAYPROXY_H

class BSignalDelayProxyPrivate;

class QString;

#include "bglobal.h"
#include "bnamespace.h"
#include "bbase.h"

#include <QObject>

/*============================================================================
================================ BSignalDelayProxy ===========================
============================================================================*/

class B_CORE_EXPORT BSignalDelayProxy : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSignalDelayProxy)
public:
    explicit BSignalDelayProxy(QObject *parent);
    explicit BSignalDelayProxy(int intermediateDelay = 500, int maximumDelay = BeQt::Second, QObject *parent = 0);
    ~BSignalDelayProxy();
protected:
    explicit BSignalDelayProxy(BSignalDelayProxyPrivate &d, QObject *parent = 0);
public:
    void setIntermediateDelay(int msecs);
    void setMaximumDelay(int msecs);
    int intermediateDelay() const;
    int maximumDelay() const;
    void setConnection(QObject *sender, const char *signal, QObject *receiver, const char *method);
    void setIntConnection(QObject *sender, const char *signal, QObject *receiver, const char *method);
    void setStringConnection(QObject *sender, const char *signal, QObject *receiver, const char *method);
public Q_SLOTS:
    void trigger();
    void trigger(int number);
    void trigger(const QString &string);
Q_SIGNALS:
    void triggered();
    void triggered(int number);
    void triggered(const QString &string);
private:
    Q_DISABLE_COPY(BSignalDelayProxy)
};

#endif // BSIGNALDELAYPROXY_H

