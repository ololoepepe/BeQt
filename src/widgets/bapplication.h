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
class BAboutDialogPrivate;
class BPersonInfoProvider;
class BAboutDialog;
class BAbstractSettingsTab;

class QPixmap;
class QAction;
class QStringList;
class QObject;
class QSystemTrayIcon;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BApplicationBase>
#include <BeQtCore/BPersonInfoProvider>

#include <QObject>
#include <QSize>
#include <QString>
#include <QIcon>
#include <QWidget>
#include <QApplication>

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
    enum SettingsTabNavigation
    {
        DefaultNavigation,
        ListNavigation,
        TabbedNavigation
    };
    enum StandardAction
    {
        SettingsAction,
        HomepageAction,
        HelpContentsAction,
        ContextualHelpAction,
        WhatsThisAction,
        AboutAction
    };
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
    static QIcon icon( const QString &name, const QIcon &fallback = QIcon() );
    static QIcon beqtIcon(const QString &name);
    static QPixmap beqtPixmap( const QString &name, const QSize &scale = QSize() );
    static void setIconCachingEnabled(bool enabled);
    static void clearIconCache();
    static void setThemedIconsEnabled(bool enabled);
    static bool themedIconsEnabled();
    static void setPreferredIconFormats(const QStringList &suffixes);
    static QStringList preferredIconFormats();
    static BAboutDialog *aboutDialogInstance();
    static void setSettingsTabDefaultNavigation(SettingsTabNavigation navigation);
    static void setHelpIndex(const QString &index);
    static void setHelpBrowserDefaultGeometry(const QRect &geometry);
    static QRect helpBrowserDefaultGeometry();
    static QAction *createStandardAction(StandardAction type, QObject *parent = 0);
    static QSystemTrayIcon *trayIcon();
public Q_SLOTS:
    void showAboutDialog();
    void showSettingsDialog();
    void showSettingsDialog(SettingsTabNavigation navigation);
    void showHelpContents();
    void showContextualHelp();
    void openHomepage();
    bool openLocalFile(const QString &fileName);
Q_SIGNALS:
    void pluginActivated(BPluginWrapper *pluginWrapper);
    void pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void languageChanged();
    void settingsLoaded(QSettings *s);
    void settingsSaved(QSettings *s);
protected:
    virtual QList<BAbstractSettingsTab *> createSettingsTabs() const;
private:
    Q_DISABLE_COPY(BApplication)
};

#endif // BAPPLICATION_H
