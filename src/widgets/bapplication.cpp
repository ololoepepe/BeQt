#include "bapplication.h"
#include "bapplication_p.h"
#include "baboutdialog.h"
#include "bsettingsdialog.h"
#include "bhelpbrowser.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BCoreApplication>
#include <BeQtCore/private/bcoreapplication_p.h>
#include <BeQtCore/BDirTools>
#include <BeQtCore/BPersonInfoProvider>

#include <QObject>
#include <QIcon>
#include <QString>
#include <QFileInfo>
#include <QApplication>
#include <QPixmap>
#include <QSize>
#include <QScopedPointer>
#include <QStringList>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QTextBrowser>
#include <QAction>
#include <QMap>
#include <QWhatsThis>
#include <QKeySequence>
#include <QList>
#include <QImageReader>
#include <QByteArray>
#include <QFont>
#include <QFontInfo>
#include <QPointer>
#include <QRect>
#include <QMessageBox>

#include <QDebug>
#include <QPointer>

/*============================================================================
================================ BApplicationPrivate =========================
============================================================================*/

/*============================== Public constructors =======================*/

BApplicationPrivate::BApplicationPrivate(BApplication *q) :
    BCoreApplicationPrivate(q)
{
    //
}

BApplicationPrivate::~BApplicationPrivate()
{
    if ( !aboutDlg.isNull() )
    {
        aboutDlg->close();
        delete aboutDlg;
    }
#if defined(BEQT_BUILTIN_RESOURCES)
    Q_CLEANUP_RESOURCE(beqtwidgets);
#endif
}

/*============================== Static public methods =====================*/

void BApplicationPrivate::retranslateStandardAction(QAction *act)
{
    if (!act)
        return;
    QVariant v = act->property("beqt/standard_action_type");
    if ( v.isNull() )
        return;
    switch ( static_cast<BApplication::StandardAction>( v.toInt() ) )
    {
    case BApplication::SettingsAction:
        act->setText( tr("Settings...", "act text") );
        act->setToolTip( tr("Show application settings", "act toolTip") );
        act->setWhatsThis( tr("Use this action to show application settings", "act whatsThis") );
        break;
    case BApplication::HomepageAction:
        act->setText( tr("Homepage", "act text") );
        act->setToolTip( tr("Open homepage", "act toolTip") );
        act->setWhatsThis( tr("Use this action to open application homepage with your browser", "act whatsThis") );
        break;
    case BApplication::HelpContentsAction:
        act->setText( tr("Help contents", "act text") );
        act->setToolTip( tr("Show Help contents", "act toolTip") );
        act->setWhatsThis( tr("Use this action to show Help contents (index)", "act whatsThis") );
        break;
    case BApplication::ContextualHelpAction:
        act->setText( tr("Contextual Help", "act text") );
        act->setToolTip( tr("Show contextual Help", "act toolTip") );
        act->setWhatsThis( tr("Use this action to open context-specific Help page", "act whatsThis") );
        break;
    case BApplication::WhatsThisAction:
    {
        act->setText( tr("What's this?", "act text") );
        act->setToolTip( act->text() );
        break;
    }
    case BApplication::AboutAction:
        act->setText( tr("About", "act text") );
        act->setToolTip( tr("Show info about this application", "act toolTip") );
        act->setWhatsThis( tr("Use this action to show information about the application, it's authors, etc.",
                               "act whatsThis") );
        break;
    default:
        break;
    }
}

QString BApplicationPrivate::findImage(const QString &subdir, const QString &name, const QStringList &preferredFormats)
{
    if ( !testCoreInit("BApplication") )
        return "";
    if ( subdir.isEmpty() || name.isEmpty() )
        return "";
    QString bfn = QFileInfo(name).baseName();
    QStringList suffixes;
    foreach ( const QByteArray &ba, QImageReader::supportedImageFormats() )
        suffixes << QString(ba);
    int indsvg = suffixes.indexOf("svg");
    if (indsvg >= 0)
        suffixes.insert(indsvg, "svgz");
    for (int i = preferredFormats.size() - 1; i >= 0; --i)
    {
        const QString &fmt = preferredFormats.at(i);
        if ( suffixes.contains(fmt) )
        {
            suffixes.removeAll(fmt);
            suffixes.prepend(fmt);
        }
    }
    foreach (const QString &suff, suffixes)
    {
        if ( suff.isEmpty() )
            continue;
        QString fn = BDirTools::findResource(subdir + "/" + bfn + "." + suff, BDirTools::GlobalOnly);
        if ( !fn.isEmpty() )
            return fn;
    }
    return "";
}

QIcon BApplicationPrivate::iconFromTheme(const QString &name)
{
    QStringList sp = QIcon::themeSearchPaths();
    QStringList spx = sp;
    foreach ( const QString &path, BApplication::locations("icons") )
        spx.insert(spx.size() - 1, path);
    QIcon::setThemeSearchPaths(spx);
    QIcon icn = QIcon::fromTheme(name);
    QIcon::setThemeSearchPaths(sp);
    return icn;
}

/*============================== Public methods ============================*/

void BApplicationPrivate::init()
{
#if defined(BEQT_BUILTIN_RESOURCES)
    Q_INIT_RESOURCE(beqtwidgets);
#endif
    iconCaching = false;
    navigation = BApplication::DefaultNavigation;
    helpBrowserGeometry = QRect(200, 200, 800, 600);
    themedIcons = true;
    connect( q_func(), SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

void BApplicationPrivate::initAboutDlg()
{
    if ( !aboutDlg.isNull() )
        return;
    aboutDlg = new BAboutDialog;
    aboutDlg->setAboutQtShown(true);
    aboutDlg->setAboutBeqtShown(true);
    aboutDlg->setWindowModality(Qt::NonModal);
}

void BApplicationPrivate::showAbout()
{
    initAboutDlg();
    if ( aboutDlg->isVisible() )
    {
        aboutDlg->activateWindow();
    }
    else
    {
        aboutDlg->resetTabs();
        aboutDlg->open();
    }
}

QString BApplicationPrivate::helpContext(QWidget *widget) const
{
    if (!widget)
        widget = QApplication::focusWidget();
    if (!widget)
        return "";
    QString context = widget->property("help").toString();
    if ( context.isEmpty() )
    {
        QWidget *pw = widget->parentWidget();
        return pw ? helpContext(pw) : QString();
    }
    else
    {
        return context;
    }
}

QString BApplicationPrivate::getHelpIndex() const
{
    return !helpIndex.isEmpty() ? helpIndex : QString("index.html");
}

QStringList BApplicationPrivate::helpSearchPaths() const
{
    QStringList paths;
    foreach ( const QString &loc, BApplication::locations(BApplication::DocumentationPath) )
        paths << BDirTools::localeBasedDirName(loc);
    return paths;
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

QIcon BApplicationPrivate::cacheIcon(const QIcon &icon, const QString &name)
{
    if ( iconCaching && !icon.isNull() )
        iconCache.insert(name, icon);
    return icon;
}

/*============================== Public slots ==============================*/

void BApplicationPrivate::retranslateUi()
{
    foreach (QAction *act, actions)
        retranslateStandardAction(act);
}

void BApplicationPrivate::actionDestroyed(QObject *act)
{
    if (!act)
        return;
    actions.remove(act);
}


/*============================================================================
================================ BApplication ================================
============================================================================*/

/*============================== Public constructors =======================*/

BApplication::BApplication() :
    BCoreApplication( *new BApplicationPrivate(this) )
{
    d_func()->init();
}

BApplication::~BApplication()
{
    //
}

/*============================== Protected constructors ====================*/

BApplication::BApplication(BApplicationPrivate &d) :
    BCoreApplication(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

QIcon BApplication::icon(const QString &name, const QIcon &fallback)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return QIcon();
    B_DS(BApplication);
    if ( ds->iconCaching && ds->iconCache.contains(name) )
        return ds->iconCache.value(name);
    QIcon icn;
    if (ds->themedIcons)
        icn = BApplicationPrivate::iconFromTheme(name);
    if ( !icn.isNull() )
        return ds->cacheIcon(icn, name);
    icn = QIcon( BApplicationPrivate::findImage("icons", name, ds->preferredIconFormats) );
    if ( !icn.isNull() )
        return ds->cacheIcon(icn, name);
    icn = beqtIcon(name);
    if ( !icn.isNull() )
        return ds->cacheIcon(icn, name);
    return fallback;
}

QIcon BApplication::beqtIcon(const QString &name)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return QIcon();
    B_DS(BApplication);
    QString suff = "svgz";
    int png = ds->preferredIconFormats.indexOf("png");
    if (png >= 0)
    {
        int svg = ds->preferredIconFormats.indexOf("svg");
        if (svg < 0)
            svg = ds->preferredIconFormats.indexOf("svgz");
        if (svg < 0 || png < svg)
            suff = "png";
    }
    QString fn = BApplicationPrivate::findImage("beqt/icons", name, QStringList() << suff);
    if ( fn.isEmpty() )
        return QIcon();
    return QIcon(fn);
}

QPixmap BApplication::beqtPixmap(const QString &name, const QSize &scale)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return QPixmap();
    QString fn = BApplicationPrivate::findImage("beqt/pixmaps", name);
    if ( name.isEmpty() )
        return QPixmap();
    QPixmap pm(fn);
    return scale.isEmpty() ? pm : pm.scaled(scale, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void BApplication::setIconCachingEnabled(bool enabled)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    ds_func()->iconCaching = enabled;
}

void BApplication::clearIconCache()
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    ds_func()->iconCache.clear();
}

void BApplication::setThemedIconsEnabled(bool enabled)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    ds_func()->themedIcons = enabled;
}

bool BApplication::themedIconsEnabled()
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return false;
    return ds_func()->themedIcons;
}

void BApplication::setPreferredIconFormats(const QStringList &suffixes)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    ds_func()->preferredIconFormats = suffixes;
}

QStringList BApplication::preferredIconFormats()
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return QStringList();
    return ds_func()->preferredIconFormats;
}

BAboutDialog *BApplication::aboutDialogInstance()
{
    if ( !BApplicationPrivate::testCoreInit("BApplication") )
        return 0;
    if ( ds_func()->aboutDlg.isNull() )
        ds_func()->initAboutDlg();
    return ds_func()->aboutDlg.data();
}

void BApplication::setSettingsTabDefaultNavigation(SettingsTabNavigation navigation)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    ds_func()->navigation = navigation;
}

void BApplication::setHelpIndex(const QString &index)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    ds_func()->helpIndex = index;
}

QAction *BApplication::createStandardAction(StandardAction type, QObject *parent)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return 0;
    QAction *act = 0;
    switch (type)
    {
    case SettingsAction:
        act = new QAction(parent);
        act->setObjectName("ActionSettings");
        act->setIcon( icon("configure") );
        act->setShortcut(QKeySequence::Preferences);
        connect( act, SIGNAL( triggered() ), _m_self, SLOT( showSettingsDialog() ) );
        break;
    case HomepageAction:
        act = new QAction(parent);
        act->setObjectName("ActionHomepage");
        act->setIcon( icon("network") );
        connect( act, SIGNAL( triggered() ), _m_self, SLOT( openHomepage() ) );
        break;
    case HelpContentsAction:
        act = new QAction(parent);
        act->setObjectName("ActionHelpContents");
        act->setIcon( beqtIcon("help_contents") );
        connect( act, SIGNAL( triggered() ), _m_self, SLOT( showHelpContents() ) );
        break;
    case ContextualHelpAction:
        act = new QAction(parent);
        act->setObjectName("ActionContextualHelp");
        act->setIcon( icon("help_contextual") );
        connect( act, SIGNAL( triggered() ), _m_self, SLOT( showContextualHelp() ) );
        break;
    case WhatsThisAction:
        act = QWhatsThis::createAction(parent);
        act->setObjectName("ActionWhatsThis");
        act->setIcon( beqtIcon("help_hint") );
        break;
    case AboutAction:
        act = new QAction(parent);
        act->setObjectName("ActionAbout");
        act->setIcon( icon("help_about") );
        connect( act, SIGNAL( triggered() ), _m_self, SLOT( showAboutDialog() ) );
        break;
    default:
        return 0;
    }
    act->setProperty("beqt/standard_action_type", type);
    ds_func()->actions.insert(act, act);
    connect( act, SIGNAL( destroyed(QObject *) ), ds_func(), SLOT( actionDestroyed(QObject *) ) );
    BApplicationPrivate::retranslateStandardAction(act);
    return act;
}

QFont BApplication::createMonospaceFont()
{
    //Using such a construct to get default monospace font family name
    QFont fnt( QFontInfo( QFont("monospace") ).family() );
    fnt.setPointSize( QApplication::font().pointSize() );
    return fnt;
}

void BApplication::setHelpBrowserDefaultGeometry(const QRect &geometry)
{
    if ( !geometry.isValid() )
        return;
    ds_func()->helpBrowserGeometry = geometry;
}

/*============================== Public slots ==============================*/

void BApplication::showAboutDialog()
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    d_func()->showAbout();
}

void BApplication::showSettingsDialog()
{
    showSettingsDialog(DefaultNavigation);
}

void BApplication::showSettingsDialog(SettingsTabNavigation navigation)
{
    BSettingsDialog::Navigation nvg;
    switch (navigation)
    {
    case ListNavigation:
        nvg = BSettingsDialog::ListNavigation;
        break;
    case TabbedNavigation:
        nvg = BSettingsDialog::TabbedNavigation;
        break;
    default:
        nvg = (TabbedNavigation == d_func()->navigation) ? BSettingsDialog::TabbedNavigation :
                                                           BSettingsDialog::ListNavigation;
        break;
    }
    QScopedPointer<BSettingsDialog> sd( new BSettingsDialog( settingsTabMap(), nvg, QApplication::activeWindow() ) );
    if ( !sd->isValid() )
    {
        QMessageBox msg( QApplication::activeWindow() );
        msg.setWindowTitle( tr("No settings", "msgbox windowTitle") );
        msg.setIcon(QMessageBox::Information);
        msg.setText( tr("This application has no settings", "msgbox text") );
        msg.setInformativeText( tr("This message may be shown due to a bug. Try contact the authors",
                                   "msgbox informativeText") );
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }
    if (sd->exec() != BSettingsDialog::Accepted)
        return;
    handleSettings( sd->settingsMap() );
}

void BApplication::showHelpContents()
{
    d_func()->showHelp();
}

void BApplication::showContextualHelp()
{
    B_D(BApplication);
    d->showHelp( d->helpContext() );
}

void BApplication::openHomepage()
{
    const B_D(BApplication);
    QString url = !d->homepage.isEmpty() ? d->homepage : QApplication::organizationDomain();
    QDesktopServices::openUrl( QUrl::fromUserInput(url) );
}

/*============================== Protected methods =========================*/

BSettingsDialog::SettingsTabMap BApplication::settingsTabMap() const
{
    return BSettingsDialog::SettingsTabMap();
}

void BApplication::handleSettings(const BSettingsDialog::SettingsMap &s)
{
    //
}
