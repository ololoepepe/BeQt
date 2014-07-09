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

class BPluginWrapper;

class QAction;
class QSystemTrayIcon;
class QWidget;

#include "bapplication.h"

#include "baboutdialog.h"
#include "bguitools.h"
#include "bsettingsdialog.h"

#include <BeQtCore/private/bapplicationbase_p.h>

#include <QIcon>
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QRect>
#include <QString>
#include <QStringList>

/*============================================================================
================================ BApplicationPrivate =========================
============================================================================*/

class B_WIDGETS_EXPORT BApplicationPrivate : public BApplicationBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BApplication)
    B_DECLARE_PUBLIC_S(BApplication)
public:
    QPointer<BAboutDialog> aboutDlg;
    QMap<QObject *, QAction *> actions;
    QRect helpBrowserGeometry;
    QString helpIndex;
    QString homepage;
    QMap<QString, QIcon> iconCache;
    bool iconCaching;
    BSettingsDialog::TabNavigation navigation;
    QStringList preferredIconFormats;
    bool themedIcons;
    QSystemTrayIcon *trayIcon;
public:
    explicit BApplicationPrivate(BApplication *q);
    ~BApplicationPrivate();
public:
    static QAction *createStandardAction(BGuiTools::StandardAction type, QObject *parent = 0);
    static QString findImage(const QString &subdir, const QString &name,
                             const QStringList &preferredFormats = QStringList());
    static QIcon iconFromTheme(const QString &name);
    static void retranslateStandardAction(QAction *act);
public:
    QIcon cacheIcon(const QIcon &icon, const QString &name);
    void emitLanguageChanged();
    void emitPluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void emitPluginActivated(BPluginWrapper *pluginWrapper);
    QString getHelpIndex() const;
    QString helpContext(QWidget *widget = 0) const;
    QStringList helpSearchPaths() const;
    void init();
    void initAboutDlg();
    void showAbout();
    void showHelp(const QString &file = QString());
public Q_SLOTS:
    void actionDestroyed(QObject *act);
    void retranslateUi();
private:
    Q_DISABLE_COPY(BApplicationPrivate)
};

#endif // BAPPLICATION_P_H
