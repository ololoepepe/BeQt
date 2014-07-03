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

#ifndef BCOREAPPLICATION_H
#define BCOREAPPLICATION_H

class BCoreApplicationPrivate;
class BPluginWrapper;

class QSettings;

#include "bglobal.h"
#include "bapplicationbase.h"

#include <QObject>
#include <QCoreApplication>

#if !defined(bApp)
#   define bApp (static_cast<BCoreApplication *>(BApplicationBase::binstance()))
#endif

/*============================================================================
================================ BCoreApplication ============================
============================================================================*/

class B_CORE_EXPORT BCoreApplication : public QCoreApplication, public BApplicationBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BCoreApplication)
    B_DECLARE_PRIVATE_S(BCoreApplication)
public:
    explicit BCoreApplication(int &argc, char **argv, const QString &applicationName);
    explicit BCoreApplication(int &argc, char **argv, const InitialSettings &s = InitialSettings());
    ~BCoreApplication();
protected:
    explicit BCoreApplication(BCoreApplicationPrivate &d, int &argc, char **argv, const QString &applicationName);
    explicit BCoreApplication(BCoreApplicationPrivate &d, int &argc, char **argv,
                              const InitialSettings &s = InitialSettings());
public:
    static BCoreApplication *binstance();
Q_SIGNALS:
    void pluginActivated(BPluginWrapper *pluginWrapper);
    void pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void languageChanged();
    void settingsLoaded(QSettings *s);
    void settingsSaved(QSettings *s);
private:
    Q_DISABLE_COPY(BCoreApplication)
};

#endif // BCOREAPPLICATION_H
