/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtWidgets module of the BeQt library.
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

#ifndef BAPPLICATION_P_H
#define BAPPLICATION_P_H

class BApplicationPrivate;
class BPluginWrapper;

class QAction;
class QRect;
class QSystemTrayIcon;
class QSettings;

#include "bapplication.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bapplicationbase_p.h>

#include <QObject>
#include <QString>
#include <QMap>
#include <QIcon>
#include <QPointer>
#include <QStringList>

/*============================================================================
================================ BApplicationPrivate =========================
============================================================================*/

class B_WIDGETS_EXPORT BApplicationPrivate : public BApplicationBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BApplication)
public:
    explicit BApplicationPrivate(BApplication *q);
    ~BApplicationPrivate();
public:
    static void retranslateStandardAction(QAction *act);
    static QString findImage( const QString &subdir, const QString &name,
                              const QStringList &preferredFormats = QStringList() );
    static QIcon iconFromTheme(const QString &name);
public:
    void init();
    void emitPluginActivated(BPluginWrapper *pluginWrapper);
    void emitPluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void emitLanguageChanged();
    void initAboutDlg();
    void showAbout();
    QString helpContext(QWidget *widget = 0) const;
    QString getHelpIndex() const;
    QStringList helpSearchPaths() const;
    void showHelp( const QString &file = QString() );
    QIcon cacheIcon(const QIcon &icon, const QString &name);
public Q_SLOTS:
    void retranslateUi();
    void actionDestroyed(QObject *act);
public:
    QString homepage;
    QPointer<BAboutDialog> aboutDlg;
    BApplication::SettingsTabNavigation navigation;
    QString helpIndex;
    QMap<QString, QIcon> iconCache;
    bool iconCaching;
    QMap<QObject *, QAction *> actions;
    QRect helpBrowserGeometry;
    bool themedIcons;
    QStringList preferredIconFormats;
    QSystemTrayIcon *trayIcon;
private:
    Q_DISABLE_COPY(BApplicationPrivate)
};

#endif // BAPPLICATION_P_H
