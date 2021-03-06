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

class BPluginWrapper;

#include "bapplication.h"
#include "bapplication_p.h"

#include "baboutdialog.h"
#include "bguitools.h"
#include "bhelpbrowser.h"
#include "bsettingsdialog.h"

#include <BeQtCore/BDirTools>
#include <BeQtCore/private/bapplicationbase_p.h>

#include <QAction>
#include <QApplication>
#include <QByteArray>
#include <QDebug>
#include <QDesktopServices>
#include <QFileInfo>
#include <QIcon>
#include <QImageReader>
#include <QKeySequence>
#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QObject>
#include <QPixmap>
#include <QPointer>
#include <QRect>
#include <QScopedPointer>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QSystemTrayIcon>
#include <QUrl>
#include <QVariant>
#include <QWhatsThis>
#include <QWidget>

/*============================================================================
================================ BApplicationPrivate =========================
============================================================================*/

/*============================== Public constructors =======================*/

BApplicationPrivate::BApplicationPrivate(BApplication *q) :
    BApplicationBasePrivate(q)
{
    //
}

BApplicationPrivate::~BApplicationPrivate()
{
    if (!aboutDlg.isNull()) {
        aboutDlg->close();
        delete aboutDlg;
    }
#if defined(BEQT_BUILTIN_RESOURCES)
    Q_CLEANUP_RESOURCE(beqtwidgets_1);
    Q_CLEANUP_RESOURCE(beqtwidgets_2);
    Q_CLEANUP_RESOURCE(beqtwidgets_3);
    Q_CLEANUP_RESOURCE(beqtwidgets_4);
    Q_CLEANUP_RESOURCE(beqtwidgets_5);
#endif
}

/*============================== Static public methods =====================*/

QAction *BApplicationPrivate::createStandardAction(BGuiTools::StandardAction type, QObject *parent)
{
    if (!testInit("BApplicationPrivate"))
        return 0;
    QAction *act = 0;
    switch (type) {
    case BGuiTools::SettingsAction:
        act = new QAction(parent);
        act->setMenuRole(QAction::PreferencesRole);
        act->setObjectName("ActionSettings");
        act->setIcon(BApplication::icon("configure"));
        act->setShortcut(QKeySequence::Preferences);
        connect(act, SIGNAL(triggered()), qs_func(), SLOT(showSettingsDialog()));
        break;
    case BGuiTools::HomepageAction:
        act = new QAction(parent);
        act->setObjectName("ActionHomepage");
        act->setIcon(BApplication::icon("gohome"));
        connect(act, SIGNAL(triggered()), qs_func(), SLOT(openHomepage()));
        break;
    case BGuiTools::HelpContentsAction:
        act = new QAction(parent);
        act->setObjectName("ActionHelpContents");
        act->setIcon(BApplication::beqtIcon("help_contents"));
        connect(act, SIGNAL(triggered()), qs_func(), SLOT(showHelpContents()));
        break;
    case BGuiTools::ContextualHelpAction:
        act = new QAction(parent);
        act->setObjectName("ActionContextualHelp");
        act->setIcon(BApplication::icon("help_contextual"));
        connect(act, SIGNAL(triggered()), qs_func(), SLOT(showContextualHelp()));
        break;
    case BGuiTools::WhatsThisAction:
        act = QWhatsThis::createAction(parent);
        act->setObjectName("ActionWhatsThis");
        act->setIcon(BApplication::beqtIcon("help_hint"));
        break;
    case BGuiTools::AboutAction:
        act = new QAction(parent);
        act->setMenuRole(QAction::AboutRole);
        act->setObjectName("ActionAbout");
        act->setIcon(BApplication::icon("help_about"));
        connect(act, SIGNAL(triggered()), qs_func(), SLOT(showAboutDialog()));
        break;
    default:
        return 0;
    }
    act->setProperty("beqt/standard_action_type", type);
    qs_func()->ds_func()->actions.insert(act, act);
    connect(act, SIGNAL(destroyed(QObject *)), qs_func()->ds_func(), SLOT(actionDestroyed(QObject *)));
    retranslateStandardAction(act);
    return act;
}

QString BApplicationPrivate::findImage(const QString &subdir, const QString &name, const QStringList &preferredFormats)
{
    if (!testInit("BApplicationPrivate"))
        return "";
    if (subdir.isEmpty() || name.isEmpty())
        return "";
    QString bfn = QFileInfo(name).baseName();
    QStringList suffixes;
    foreach (const QByteArray &ba, QImageReader::supportedImageFormats())
        suffixes << QString(ba);
    int indsvg = suffixes.indexOf("svg");
    if (indsvg >= 0)
        suffixes.insert(indsvg, "svgz");
    for (int i = preferredFormats.size() - 1; i >= 0; --i) {
        const QString &fmt = preferredFormats.at(i);
        if (suffixes.contains(fmt)) {
            suffixes.removeAll(fmt);
            suffixes.prepend(fmt);
        }
    }
    foreach (const QString &suff, suffixes) {
        if (suff.isEmpty())
            continue;
        QString fn = BDirTools::findResource(subdir + "/" + bfn + "." + suff, BDirTools::GlobalOnly);
        if (!fn.isEmpty())
            return fn;
    }
    return "";
}

QIcon BApplicationPrivate::iconFromTheme(const QString &name)
{
    QStringList sp = QIcon::themeSearchPaths();
    QStringList spx = sp;
    foreach (const QString &path, BApplication::locations(BApplication::IconsPath))
        spx.insert(spx.size() - 1, path);
    QIcon::setThemeSearchPaths(spx);
    QIcon icn = QIcon::fromTheme(name);
    QIcon::setThemeSearchPaths(sp);
    return icn;
}

void BApplicationPrivate::retranslateStandardAction(QAction *act)
{
    if (!act)
        return;
    QVariant v = act->property("beqt/standard_action_type");
    if (v.isNull())
        return;
    switch (static_cast<BGuiTools::StandardAction>(v.toInt())) {
    case BGuiTools::SettingsAction:
        act->setText(tr("Settings...", "act text"));
        act->setToolTip(tr("Show application settings", "act toolTip"));
        act->setWhatsThis(tr("Use this action to show application settings", "act whatsThis"));
        break;
    case BGuiTools::HomepageAction:
        act->setText(tr("Homepage", "act text"));
        act->setToolTip(tr("Open homepage", "act toolTip"));
        act->setWhatsThis(tr("Use this action to open application homepage with your browser", "act whatsThis"));
        break;
    case BGuiTools::HelpContentsAction:
        act->setText(tr("Help contents", "act text"));
        act->setToolTip(tr("Show Help contents", "act toolTip"));
        act->setWhatsThis(tr("Use this action to show Help contents (index)", "act whatsThis"));
        break;
    case BGuiTools::ContextualHelpAction:
        act->setText(tr("Contextual Help", "act text"));
        act->setToolTip(tr("Show contextual Help", "act toolTip"));
        act->setWhatsThis(tr("Use this action to open context-specific Help page", "act whatsThis"));
        break;
    case BGuiTools::WhatsThisAction:
        act->setText(tr("What's this?", "act text"));
        act->setToolTip(act->text());
        break;
    case BGuiTools::AboutAction:
        act->setText(tr("About...", "act text"));
        act->setToolTip(tr("Show info about this application", "act toolTip"));
        act->setWhatsThis(tr("Use this action to show information about the application, it's authors, etc.",
                             "act whatsThis") );
        break;
    default:
        break;
    }
}

/*============================== Public methods ============================*/

QIcon BApplicationPrivate::cacheIcon(const QIcon &icon, const QString &name)
{
    if (iconCaching && !icon.isNull())
        iconCache.insert(name, icon);
    return icon;
}

void BApplicationPrivate::emitLanguageChanged()
{
    QMetaObject::invokeMethod(q_func(), "languageChanged");
}

void BApplicationPrivate::emitPluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper)
{
    QMetaObject::invokeMethod(q_func(), "pluginAboutToBeDeactivated", Q_ARG(BPluginWrapper *, pluginWrapper));
}

void BApplicationPrivate::emitPluginActivated(BPluginWrapper *pluginWrapper)
{
    QMetaObject::invokeMethod(q_func(), "pluginActivated", Q_ARG(BPluginWrapper *, pluginWrapper));
}

QString BApplicationPrivate::getHelpIndex() const
{
    return !helpIndex.isEmpty() ? helpIndex : QString("index.html");
}

QString BApplicationPrivate::helpContext(QWidget *widget) const
{
    if (!widget)
        widget = QApplication::focusWidget();
    if (!widget)
        return "";
    QString context = widget->property("help").toString();
    if (context.isEmpty()) {
        QWidget *pw = widget->parentWidget();
        return pw ? helpContext(pw) : QString();
    } else {
        return context;
    }
}

QStringList BApplicationPrivate::helpSearchPaths() const
{
    QStringList paths;
    foreach (const QString &loc, BApplication::locations(BApplication::DocumentationPath))
        paths << BDirTools::localeBasedDirName(loc);
    return paths;
}

void BApplicationPrivate::init()
{
#if defined(BEQT_BUILTIN_RESOURCES)
    Q_INIT_RESOURCE(beqtwidgets_1);
    Q_INIT_RESOURCE(beqtwidgets_2);
    Q_INIT_RESOURCE(beqtwidgets_3);
    Q_INIT_RESOURCE(beqtwidgets_4);
    Q_INIT_RESOURCE(beqtwidgets_5);
#endif
    iconCaching = false;
    navigation = BSettingsDialog::ListNavigation;
    helpBrowserGeometry = QRect(200, 200, 800, 600);
    themedIcons = true;
    connect(q_func(), SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
    trayIcon = 0;
}

void BApplicationPrivate::initAboutDlg()
{
    if (!aboutDlg.isNull())
        return;
    aboutDlg = new BAboutDialog;
    aboutDlg->setAboutQtShown(true);
    aboutDlg->setAboutBeqtShown(true);
    aboutDlg->setWindowModality(Qt::NonModal);
}

void BApplicationPrivate::showAbout()
{
    initAboutDlg();
    if (aboutDlg->isVisible()) {
        aboutDlg->activateWindow();
    } else {
        aboutDlg->setParent(QApplication::activeWindow(), aboutDlg->windowFlags());
        aboutDlg->resetTabs();
        aboutDlg->exec();
    }
}

void BApplicationPrivate::showHelp(const QString &file)
{
    QString index = getHelpIndex();
    QString fn = !file.isEmpty() ? file : index;
    BHelpBrowser *hb = new BHelpBrowser(helpSearchPaths(), index, fn);
    hb->setAttribute(Qt::WA_DeleteOnClose, true);
    hb->setGeometry(helpBrowserGeometry);
    hb->show();
}

/*============================== Public slots ==============================*/

void BApplicationPrivate::actionDestroyed(QObject *act)
{
    if (!act)
        return;
    actions.remove(act);
}

void BApplicationPrivate::retranslateUi()
{
    foreach (QAction *act, actions)
        retranslateStandardAction(act);
}

/*============================================================================
================================ BApplication ================================
============================================================================*/

/*============================== Public constructors =======================*/

BApplication::BApplication(int &argc, char **argv, const QString &applicationName, const QString &organizationName) :
    QApplication(argc, argv), BApplicationBase(*new BApplicationPrivate(this), applicationName, organizationName)
{
    d_func()->init();
}

BApplication::BApplication(int &argc, char **argv, const InitialSettings &s) :
    QApplication(argc, argv), BApplicationBase(*new BApplicationPrivate(this), s)
{
    d_func()->init();
}

BApplication::~BApplication()
{
    //
}

/*============================== Protected constructors ====================*/

BApplication::BApplication(BApplicationPrivate &d, int &argc, char **argv, const QString &applicationName,
                           const QString &organizationName) :
    QApplication(argc, argv), BApplicationBase(d, applicationName, organizationName)
{
    d_func()->init();
}

BApplication::BApplication(BApplicationPrivate &d, int &argc, char **argv, const InitialSettings &s) :
    QApplication(argc, argv), BApplicationBase(d, s)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

BAboutDialog *BApplication::aboutDialogInstance()
{
    if (!BApplicationPrivate::testInit("BApplication"))
        return 0;
    if (ds_func()->aboutDlg.isNull())
        ds_func()->initAboutDlg();
    return ds_func()->aboutDlg.data();
}

QIcon BApplication::beqtIcon(const QString &name)
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return QIcon();
    B_DS(BApplication);
    QString suff = "svgz";
    int png = ds->preferredIconFormats.indexOf("png");
    if (png >= 0) {
        int svg = ds->preferredIconFormats.indexOf("svg");
        if (svg < 0)
            svg = ds->preferredIconFormats.indexOf("svgz");
        if (svg < 0 || png < svg)
            suff = "png";
    }
    QString fn = BApplicationPrivate::findImage("beqt/icons", name, QStringList() << suff);
    if (fn.isEmpty())
        return QIcon();
    return QIcon(fn);
}

QPixmap BApplication::beqtPixmap(const QString &name, const QSize &scale)
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return QPixmap();
    QString fn = BApplicationPrivate::findImage("beqt/pixmaps", name);
    if (name.isEmpty())
        return QPixmap();
    QPixmap pm(fn);
    return scale.isEmpty() ? pm : pm.scaled(scale, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void BApplication::clearIconCache()
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return;
    ds_func()->iconCache.clear();
}

QRect BApplication::helpBrowserDefaultGeometry()
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return QRect();
    return ds_func()->helpBrowserGeometry;
}

QIcon BApplication::icon(const QString &name, const QIcon &fallback)
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return QIcon();
    B_DS(BApplication);
    if (ds->iconCaching && ds->iconCache.contains(name))
        return ds->iconCache.value(name);
    QIcon icn;
    if (ds->themedIcons)
        icn = BApplicationPrivate::iconFromTheme(name);
    if (!icn.isNull())
        return ds->cacheIcon(icn, name);
    icn = QIcon(BApplicationPrivate::findImage("icons", name, ds->preferredIconFormats));
    if (!icn.isNull())
        return ds->cacheIcon(icn, name);
    icn = beqtIcon(name);
    if (!icn.isNull())
        return ds->cacheIcon(icn, name);
    return fallback;
}

QStringList BApplication::preferredIconFormats()
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return QStringList();
    return ds_func()->preferredIconFormats;
}

void BApplication::setHelpBrowserDefaultGeometry(const QRect &geometry)
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return;
    if (!geometry.isValid())
        return;
    ds_func()->helpBrowserGeometry = geometry;
}

void BApplication::setHelpIndex(const QString &index)
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return;
    ds_func()->helpIndex = index;
}

void BApplication::setIconCachingEnabled(bool enabled)
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return;
    ds_func()->iconCaching = enabled;
}

void BApplication::setPreferredIconFormats(const QStringList &suffixes)
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return;
    ds_func()->preferredIconFormats = suffixes;
}

void BApplication::setSettingsTabDefaultNavigation(BSettingsDialog::TabNavigation navigation)
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return;
    ds_func()->navigation = navigation;
}

void BApplication::setThemedIconsEnabled(bool enabled)
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return;
    ds_func()->themedIcons = enabled;
}

BSettingsDialog::TabNavigation BApplication::settingsTabDefaultNavigation()
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return BSettingsDialog::ListNavigation;
    return ds_func()->navigation;
}

bool BApplication::themedIconsEnabled()
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return false;
    return ds_func()->themedIcons;
}

QSystemTrayIcon *BApplication::trayIcon()
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return 0;
    if (!ds_func()->trayIcon) {
        ds_func()->trayIcon = new QSystemTrayIcon(QApplication::windowIcon(), ds_func());
        ds_func()->trayIcon->setToolTip(QApplication::applicationName());
    }
    return ds_func()->trayIcon;
}

/*============================== Public slots ==============================*/

void BApplication::openHomepage()
{
    const B_D(BApplication);
    QString url = !d->homepage.isEmpty() ? d->homepage : QApplication::organizationDomain();
    QDesktopServices::openUrl( QUrl::fromUserInput(url) );
}

bool BApplication::openLocalFile(const QString &fileName)
{
    return !fileName.isEmpty() && QDesktopServices::openUrl( QUrl::fromLocalFile(fileName) );
}

void BApplication::showAboutDialog()
{
    if (!BApplicationBasePrivate::testInit("BApplication"))
        return;
    d_func()->showAbout();
}

void BApplication::showContextualHelp()
{
    B_D(BApplication);
    d->showHelp(d->helpContext());
}

void BApplication::showHelpContents()
{
    d_func()->showHelp();
}

void BApplication::showSettingsDialog()
{
    showSettingsDialog(d_func()->navigation);
}

void BApplication::showSettingsDialog(BSettingsDialog::TabNavigation navigation)
{
    QScopedPointer<BSettingsDialog> sd(new BSettingsDialog(createSettingsTabs(), navigation,
                                                           QApplication::activeWindow()));
    if (!sd->isValid()) {
        QMessageBox msg(QApplication::activeWindow());
        msg.setWindowTitle(tr("No settings", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Information);
        msg.setText(tr("This application has no settings", "msgbox text"));
        msg.setInformativeText(tr("This message may be shown due to a bug. Try contact the authors",
                                  "msgbox informativeText"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }
    QSettings *s = ds_func()->settings;
    sd->restoreGeometry(s->value("BeQt/Widgets/settings_dialog_geometry").toByteArray());
    sd->restoreState(s->value("BeQt/Widgets/settings_dialog_state").toByteArray());
    sd->exec();
    s->setValue("BeQt/Widgets/settings_dialog_geometry", sd->saveGeometry());
    s->setValue("BeQt/Widgets/settings_dialog_state", sd->saveState());
}

/*============================== Protected methods =========================*/

QList<BAbstractSettingsTab *> BApplication::createSettingsTabs() const
{
    return QList<BAbstractSettingsTab *>();
}
