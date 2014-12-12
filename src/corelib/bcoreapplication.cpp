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

#include "bcoreapplication.h"
#include "bcoreapplication_p.h"

#include <QCoreApplication>
#include <QDebug>
#include <QMetaObject>
#include <QObject>

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

void BCoreApplicationPrivate::emitLanguageChanged()
{
    QMetaObject::invokeMethod(q_func(), "languageChanged");
}

void BCoreApplicationPrivate::emitPluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper)
{
    QMetaObject::invokeMethod(q_func(), "pluginAboutToBeDeactivated", Q_ARG(BPluginWrapper *, pluginWrapper));
}

void BCoreApplicationPrivate::emitPluginActivated(BPluginWrapper *pluginWrapper)
{
    QMetaObject::invokeMethod(q_func(), "pluginActivated", Q_ARG(BPluginWrapper *, pluginWrapper));
}

void BCoreApplicationPrivate::init()
{
    //
}

/*============================================================================
================================ BCoreApplication ============================
============================================================================*/

/*!
\macro bApp()
\relates BCoreApplication
Calls BApplicationBase::binstance() and casts it to pointer to BCoreApplication. Returns the pointer casted.
*/

/*!
\class BCoreApplication
\brief The BCoreApplication class provides both QCoreApplication and BApplicationBase functionality.

This class is used by non-GUI applications to provide their event loop, resources, plugins and translations systems.
For non-GUI application that uses Qt (and BeQt), there should be exactly one BCoreApplication object. For GUI
applications, see BApplication.
*/

/*============================== Public constructors =======================*/

/*!
Constructs a BCoreApplication object. Application and organization names are set to \a applicationName and
\a organizationName, respectively.

\a argc and \a argv are passed to QCoreApplication superclass.
*/

BCoreApplication::BCoreApplication(int &argc, char **argv, const QString &applicationName,
                                   const QString &organizationName) :
    QCoreApplication(argc, argv),
    BApplicationBase(*new BCoreApplicationPrivate(this), applicationName, organizationName)
{
    d_func()->init();
}

/*!
Constructs a BCoreApplication object. Application and organization names are set according to the corresponding
InitialSettings \a s members. You may also specify application portability mode.

\a argc and \a argv are passed to QCoreApplication superclass.
*/

BCoreApplication::BCoreApplication(int &argc, char **argv, const InitialSettings &s) :
    QCoreApplication(argc, argv), BApplicationBase(*new BCoreApplicationPrivate(this), s)
{
    d_func()->init();
}

/*!
Destroys the object. Plugins, translators, settings and logger instances owned by the object are unloaded/deleted.
*/

BCoreApplication::~BCoreApplication()
{
    //
}

/*============================== Protected constructors ====================*/

/*!
Constructs a BCoreApplication object and associates the given data object \a d with it. Application and organization
names are set to \a applicationName and \a organizationName, respectively.

\a argc and \a argv are passed to QCoreApplication superclass.
*/

BCoreApplication::BCoreApplication(BCoreApplicationPrivate &d, int &argc, char **argv, const QString &applicationName,
                                   const QString &organizationName) :
    QCoreApplication(argc, argv), BApplicationBase(d, applicationName, organizationName)
{
    d_func()->init();
}

/*!
Constructs a BCoreApplication object and associates the given data object \a d with it. Application and organization
names are set according to the corresponding InitialSettings \a s members. You may also specify application portability
mode.

\a argc and \a argv are passed to QCoreApplication superclass.
*/

BCoreApplication::BCoreApplication(BCoreApplicationPrivate &d, int &argc, char **argv, const InitialSettings &s) :
    QCoreApplication(argc, argv), BApplicationBase(d, s)
{
    d_func()->init();
}

/*!
\fn void BCoreApplication::languageChanged()

This signal is emitted when the application locale has been changed, for example by calling
BApplicationBase::setLocale().
*/

/*!
\fn void BCoreApplication::pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper)

This signal is emitted when a plugin owned by \a pluginWrapper registered in BApplicationBase instance is about to be
activated.

For details, see \l {Plugins system}

\sa BApplicationBase::loadPlugins(), BApplicationBase::setPluginActivated(), BPluginWrapper
*/

/*!
\fn void BCoreApplication::pluginActivated(BPluginWrapper *pluginWrapper)

This signal is emitted when a plugin owned by \a pluginWrapper registered in BApplicationBase instance is about to be
deactivated.

For details, see \l {Plugins system}

\sa BApplicationBase::unloadPlugins(), BApplicationBase::setPluginActivated(), BPluginWrapper
*/
