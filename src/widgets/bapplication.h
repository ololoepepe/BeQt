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

#ifndef BAPPLICATION_H
#define BAPPLICATION_H

class BApplicationPrivate;

class BAboutDialog;
class BPluginsSettingsTabPrivate;
class BPluginWrapper;

class QPixmap;
class QRect;
class QStringList;
class QSystemTrayIcon;

#include "bsettingsdialog.h"

#include <BeQtCore/BApplicationBase>

#include <QApplication>
#include <QIcon>
#include <QList>
#include <QObject>
#include <QSize>
#include <QString>

#if defined(bApp)
#   undef bApp
#endif
#define bApp (static_cast<BApplication *>(BApplicationBase::binstance()))

/*============================================================================
================================ BApplication ================================
============================================================================*/

class B_WIDGETS_EXPORT BApplication : public QApplication, public BApplicationBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BApplication)
    B_DECLARE_PRIVATE_S(BApplication)
public:
    explicit BApplication(int &argc, char **argv, const QString &applicationName = QString(),
                          const QString &organizationName = QString());
    explicit BApplication(int &argc, char **argv, const InitialSettings &s);
    ~BApplication();
protected:
    explicit BApplication(BApplicationPrivate &d, int &argc, char **argv, const QString &applicationName = QString(),
                          const QString &organizationName = QString());
    explicit BApplication(BApplicationPrivate &d, int &argc, char **argv, const InitialSettings &s);
public:
    static BAboutDialog *aboutDialogInstance();
    static QIcon beqtIcon(const QString &name);
    static QPixmap beqtPixmap(const QString &name, const QSize &scale = QSize());
    static void clearIconCache();
    static QRect helpBrowserDefaultGeometry();
    static QIcon icon(const QString &name, const QIcon &fallback = QIcon());
    static QStringList preferredIconFormats();
    static void setHelpBrowserDefaultGeometry(const QRect &geometry);
    static void setHelpIndex(const QString &index);
    static void setIconCachingEnabled(bool enabled);
    static void setPreferredIconFormats(const QStringList &suffixes);
    static void setThemedIconsEnabled(bool enabled);
    static void setSettingsTabDefaultNavigation(BSettingsDialog::TabNavigation navigation);
    static bool themedIconsEnabled();
    static QSystemTrayIcon *trayIcon();
public Q_SLOTS:
    void openHomepage();
    bool openLocalFile(const QString &fileName);
    void showAboutDialog();
    void showContextualHelp();
    void showHelpContents();
    void showSettingsDialog();
    void showSettingsDialog(BSettingsDialog::TabNavigation navigation);
Q_SIGNALS:
    void languageChanged();
    void pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void pluginActivated(BPluginWrapper *pluginWrapper);
protected:
    virtual QList<BAbstractSettingsTab *> createSettingsTabs() const;
private:
    Q_DISABLE_COPY(BApplication)
    friend class BPluginsSettingsTabPrivate;
};

#endif // BAPPLICATION_H
