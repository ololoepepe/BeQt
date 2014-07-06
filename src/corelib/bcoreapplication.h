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

#include "bapplicationbase.h"

#include <QCoreApplication>
#include <QObject>
#include <QString>

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
    explicit BCoreApplication(int &argc, char **argv, const QString &applicationName = QString(),
                              const QString &organizationName = QString());
    explicit BCoreApplication(int &argc, char **argv, const InitialSettings &s);
    ~BCoreApplication();
protected:
    explicit BCoreApplication(BCoreApplicationPrivate &d, int &argc, char **argv,
                              const QString &applicationName = QString(),
                              const QString &organizationName = QString());
    explicit BCoreApplication(BCoreApplicationPrivate &d, int &argc, char **argv, const InitialSettings &s);
Q_SIGNALS:
    void languageChanged();
    void pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void pluginActivated(BPluginWrapper *pluginWrapper);
private:
    Q_DISABLE_COPY(BCoreApplication)
};

#endif // BCOREAPPLICATION_H
