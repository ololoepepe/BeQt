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

#ifndef BPLUGINWRAPPER_H
#define BPLUGINWRAPPER_H

class BPluginWrapperPrivate;

class BVersion;

class QSettings;
class QString;
class QStringList;

#include "bplugininterface.h"
#include "bbase.h"

#include <QObject>
#include <QList>

/*============================================================================
================================ BPluginWrapper ==============================
============================================================================*/

class B_CORE_EXPORT BPluginWrapper : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPluginWrapper)
public:
    typedef bool (*InterfaceTestFunction)(const QObject *);
public:
    explicit BPluginWrapper(QObject *parent = 0);
    explicit BPluginWrapper(const QString &fileName, QObject *parent = 0);
    ~BPluginWrapper();
protected:
    explicit BPluginWrapper(BPluginWrapperPrivate &d, QObject *parent = 0);
public:
    static QStringList acceptableTypes();
    static InterfaceTestFunction interfaceTestFunction();
    static BPluginWrapper *parentWrapper(const BPluginInterface *i);
    static bool removeSettings(const QString &pluginName);
    static void setAcceptableTypes(const QStringList &list);
    static void setInterfaceTestFunction(InterfaceTestFunction function);
public:
    QString fileName() const;
    BPluginInterface::PluginInfo info() const;
    void install();
    QObject *instance() const;
    BPluginInterface *interface() const;
    bool isActivated() const;
    bool isLoaded() const;
    QString name() const;
    bool prefereStaticInfo() const;
    void remove();
    void setFileName(const QString &fn);
    QSettings *settings() const;
    BPluginInterface::StaticPluginInfo staticInfo() const;
    QString type() const;
    BVersion version() const;
public Q_SLOTS:
    bool activate();
    void deactivate();
    bool load();
    void setActivated(bool b);
    void setLoaded(bool b);
    void unload();
Q_SIGNALS:
    void aboutToBeDeactivated();
    void activated();
private:
    Q_DISABLE_COPY(BPluginWrapper)
};

#endif // BPLUGINWRAPPER_H
