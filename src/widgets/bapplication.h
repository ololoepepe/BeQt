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
class QFont;
class QStringList;
class QToolBar;
class QToolButton;
class QObject;
class QVBoxLayout;
class QSystemTrayIcon;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BApplicationBase>
#include <BeQtCore/BPersonInfoProvider>

#include <QObject>
#include <QSize>
#include <QString>
#include <QIcon>
#include <QFrame>
#include <QWidget>
#include <QFormLayout>
#include <QLayout>
#include <QStack>
#include <QLayoutItem>
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
    explicit BApplication(int &argc, char **argv, const QString &applicationName);
    explicit BApplication(int &argc, char **argv, const InitialSettings &s = InitialSettings());
    ~BApplication();
protected:
    explicit BApplication(BApplicationPrivate &d, int &argc, char **argv, const QString &applicationName);
    explicit BApplication(BApplicationPrivate &d, int &argc, char **argv,
                          const InitialSettings &s = InitialSettings());
public:
    template <typename T> static T *labelForField(QWidget *field);
    template <typename T> static T *labelForField(QLayout *field);
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
    static QAction *createStandardAction(StandardAction type, QObject *parent = 0);
    static QAction *createSeparator(QObject *parent = 0);
    static QFrame *createFrame(QFrame::Shape shape, QWidget *parent = 0);
    static QFrame *createFrame(QFrame::Shape shape, QFrame::Shadow shadow, QWidget *parent = 0);
    static QFont createMonospaceFont();
    static void setHelpBrowserDefaultGeometry(const QRect &geometry);
    static QToolButton *toolButtonForAction(QToolBar *toolBar, QAction *action);
    static void addRow(QVBoxLayout *vlt, const QString &label, QWidget *field);
    static void addRow(QVBoxLayout *vlt, const QString &label, QLayout *field);
    static QFormLayout *formLayout(QWidget *field);
    static QFormLayout *formLayout(QLayout *field);
    static void setRowVisible(QWidget *field, bool visible);
    static void setRowVisible(QLayout *field, bool visible);
    static void setRowEnabled(QWidget *field, bool enabled);
    static void setRowEnabled(QLayout *field, bool enabled);
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
    friend class BAboutDialogPrivate;
};

/*============================================================================
================================ BApplication ================================
============================================================================*/

/*============================== Static public template methods=============*/

template <typename T> T *BApplication::labelForField(QWidget *field)
{
    QWidget *pw = field ? field->parentWidget() : 0;
    if (!pw)
        return 0;
    QStack<QLayout *> s;
    s.push(pw->layout());
    while (!s.isEmpty())
    {
        QLayout *lt = s.pop();
        if (!lt)
            continue;
        QFormLayout *flt = qobject_cast<QFormLayout *>(lt);
        if (flt)
        {
            QWidget *w = flt->labelForField(field);
            if (w)
                return qobject_cast<T *>(w);
        }
        foreach (int i, bRangeD(0, lt->count() - 1))
            s.push(lt->itemAt(i)->layout());
    }
    return 0;
}

template <typename T> T *BApplication::labelForField(QLayout *field)
{
    QWidget *pw = field ? field->parentWidget() : 0;
    if (!pw)
        return 0;
    QStack<QLayout *> s;
    s.push(pw->layout());
    while (!s.isEmpty())
    {
        QLayout *lt = s.pop();
        if (!lt)
            continue;
        QFormLayout *flt = qobject_cast<QFormLayout *>(lt);
        if (flt)
        {
            QWidget *w = flt->labelForField(field);
            if (w)
                return qobject_cast<T *>(w);
        }
        foreach (int i, bRangeD(0, lt->count() - 1))
            s.push(lt->itemAt(i)->layout());
    }
    return 0;
}

#endif // BAPPLICATION_H
