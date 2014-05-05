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

#ifndef BSPAMNOTIFIER_H
#define BSPAMNOTIFIER_H

class BSpamNotifierPrivate;

#include "bglobal.h"
#include "bbase.h"

#include <QObject>

/*============================================================================
================================ BSpamNotifier ===============================
============================================================================*/

class B_CORE_EXPORT BSpamNotifier : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSpamNotifier)
public:
    explicit BSpamNotifier(QObject *parent = 0);
    explicit BSpamNotifier(QObject *parent, int checkInterval, int eventLimit);
    ~BSpamNotifier();
protected:
    explicit BSpamNotifier(BSpamNotifierPrivate &d, QObject *parent = 0);
public:
    void setCheckInterval(int msecs);
    void setEventLimit(int count);
    int checkInterval() const;
    int eventLimit() const;
    int eventCount() const;
    int timeElapsed() const;
    bool isEnabled() const;
    bool isActive() const;
public Q_SLOTS:
    void setEnabled(bool enabled);
    void spam(int eventWeight = 1);
Q_SIGNALS:
    void spammed(int msecsElapsed);
private:
    Q_DISABLE_COPY(BSpamNotifier)
};

#endif // BSPAMNOTIFIER_H
