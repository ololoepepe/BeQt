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

class QStringList;
class QSettings;

#include "bglobal.h"
#include "bplugininterface.h"
#include "bbase.h"
#include "bversion.h"

#include <QObject>
#include <QList>
#include <QString>

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
    static void setAcceptableTypes(const QStringList &list);
    static void setInterfaceTestFunction(InterfaceTestFunction function);
    static QStringList acceptableTypes();
    static InterfaceTestFunction interfaceTestFunction();
    static BPluginWrapper *parentWrapper(const BPluginInterface *i);
    static bool removeSettings(const QString &pluginName);
public:
    void setFileName(const QString &fn);
    void install();
    void remove();
    bool isLoaded() const;
    bool isActivated() const;
    QString fileName() const;
    QString type() const;
    QString name() const;
    BVersion version() const;
    bool prefereStaticInfo() const;
    BPluginInterface::PluginInfo info() const;
    BPluginInterface::StaticPluginInfo staticInfo() const;
    QObject *instance() const;
    BPluginInterface *interface() const;
    QSettings *settings() const;
public Q_SLOTS:
    bool load();
    void unload();
    bool activate();
    void deactivate();
    void setLoaded(bool b);
    void setActivated(bool b);
Q_SIGNALS:
    void activated();
    void aboutToBeDeactivated();
private:
    Q_DISABLE_COPY(BPluginWrapper)
};

#endif // BPLUGINWRAPPER_H
