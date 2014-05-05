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

#ifndef BSPAMNOTIFIER_P_H
#define BSPAMNOTIFIER_P_H

class QTimer;

#include "bspamnotifier.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
class QElapsedTimer;
#endif

/*============================================================================
================================ BSpamNotifierPrivate
============================================================================*/

class B_CORE_EXPORT BSpamNotifierPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSpamNotifier)
public:
    explicit BSpamNotifierPrivate(BSpamNotifier *q);
    ~BSpamNotifierPrivate();
public:
    void init();
    void testSpam(int dcount = 1);
    int timeElapsed() const;
public Q_SLOTS:
    void timeout();
public:
    QTimer *timer;
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    QElapsedTimer *etimer;
#endif
    int interval;
    int limit;
    int count;
    int elapsed;
    bool enabled;
private:
    Q_DISABLE_COPY(BSpamNotifierPrivate)
};

#endif // BSPAMNOTIFIER_P_H
