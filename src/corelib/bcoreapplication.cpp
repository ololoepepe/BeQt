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

class BPluginWrapper;

class QWidget;
class QSettings;

#include "bcoreapplication.h"
#include "bglobal.h"
#include "bcoreapplication_p.h"

#include <QObject>
#include <QCoreApplication>
#include <QtGlobal>
#include <QMetaObject>

#include <QDebug>

/*============================================================================
================================ BCoreApplicationPrivate =====================
============================================================================*/

/*============================== Public constructors =======================*/

BCoreApplicationPrivate::BCoreApplicationPrivate(BCoreApplication *q) :
    BApplicationBasePrivate(q)
{
    //
}

BCoreApplicationPrivate::~BCoreApplicationPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BCoreApplicationPrivate::init()
{
    //
}

void BCoreApplicationPrivate::emitPluginActivated(BPluginWrapper *pluginWrapper)
{
    QMetaObject::invokeMethod(q_func(), "pluginActivated", Q_ARG(BPluginWrapper *, pluginWrapper));
}

void BCoreApplicationPrivate::emitPluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper)
{
    QMetaObject::invokeMethod(q_func(), "pluginAboutToBeDeactivated", Q_ARG(BPluginWrapper *, pluginWrapper));
}

void BCoreApplicationPrivate::emitLanguageChanged()
{
    QMetaObject::invokeMethod(q_func(), "languageChanged");
}

/*============================================================================
================================ BCoreApplication ============================
============================================================================*/

/*============================== Public constructors =======================*/

BCoreApplication::BCoreApplication(int &argc, char **argv, const QString &applicationName,
                                   const QString &organizationName) :
    QCoreApplication(argc, argv),
    BApplicationBase(*new BCoreApplicationPrivate(this), applicationName, organizationName)
{
    d_func()->init();
}

BCoreApplication::BCoreApplication(int &argc, char **argv, const InitialSettings &s) :
    QCoreApplication(argc, argv), BApplicationBase(*new BCoreApplicationPrivate(this), s)
{
    d_func()->init();
}

BCoreApplication::~BCoreApplication()
{
    //
}

/*============================== Protected constructors ====================*/

BCoreApplication::BCoreApplication(BCoreApplicationPrivate &d, int &argc, char **argv, const QString &applicationName,
                                   const QString &organizationName) :
    QCoreApplication(argc, argv), BApplicationBase(d, applicationName, organizationName)
{
    d_func()->init();
}

BCoreApplication::BCoreApplication(BCoreApplicationPrivate &d, int &argc, char **argv, const InitialSettings &s) :
    QCoreApplication(argc, argv), BApplicationBase(d, s)
{
    d_func()->init();
}
