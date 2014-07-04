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

#ifndef BCOREAPPLICATION_P_H
#define BCOREAPPLICATION_P_H

class BPluginWrapper;

class QSettings;

#include "bcoreapplication.h"
#include "bglobal.h"
#include "bapplicationbase_p.h"

#include <QtGlobal>

/*============================================================================
================================ BCoreApplicationPrivate =====================
============================================================================*/

class B_CORE_EXPORT BCoreApplicationPrivate : public BApplicationBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BCoreApplication)
    B_DECLARE_PUBLIC_S(BCoreApplication)
public:
    explicit BCoreApplicationPrivate(BCoreApplication *q);
    ~BCoreApplicationPrivate();
public:
    void init();
    void emitPluginActivated(BPluginWrapper *pluginWrapper);
    void emitPluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void emitLanguageChanged();
private:
    Q_DISABLE_COPY(BCoreApplicationPrivate)
};

#endif // BCOREAPPLICATION_P_H
