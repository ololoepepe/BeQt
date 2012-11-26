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

#include <QDebug>
#include <QPointer>

/*============================================================================
================================ Application Private
============================================================================*/

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
        act->setText( trq("Settings...", "act text") );
        act->setToolTip( trq("Show Settings dialog", "act toolTip") );
        act->setWhatsThis( trq("Use this action to show application settings dialog", "act whatsThis") );
        break;
    case BApplication::HomepageAction:
        act->setText( trq("Homepage", "act text") );
        act->setToolTip( trq("Go to homepage", "act toolTip") );
        act->setWhatsThis( trq("Use this action to show application homepage with your browser", "act whatsThis") );
        break;
    case BApplication::HelpContentsAction:
        act->setText( trq("Help contents", "act text") );
        act->setToolTip( trq("Show Help contents", "act toolTip") );
        act->setWhatsThis( trq("Use this action to show Help contents (index)", "act whatsThis") );
        break;
    case BApplication::ContextualHelpAction:
        act->setText( trq("Contextual Help", "act text") );
        act->setToolTip( trq("Show contextual Help", "act toolTip") );
        act->setWhatsThis( trq("Use this action to open context-specific Help page", "act whatsThis") );
        break;
    case BApplication::WhatsThisAction:
    {
        QAction *wta = QWhatsThis::createAction();
        act->setText( wta->text() );
        act->setToolTip( wta->toolTip() );
        wta->deleteLater();
        break;
    }
    case BApplication::AboutAction:
        act->setText( trq("About", "act text") );
        act->setToolTip( trq("Show About dialog", "act toolTip") );
        act->setWhatsThis( trq("Use this action to show information about application, it's authors, etc.",
                               "act whatsThis") );
        break;
    default:
        break;
    }
}

QString BApplicationPrivate::findImage(const QString &subdir, const QString &name)
{
    if ( !testCoreInit("BApplication") )
        return "";
    if ( subdir.isEmpty() || name.isEmpty() )
        return "";
    QString bfn = QFileInfo(name).baseName();
    QStringList suffixes;
    suffixes << QFileInfo(name).suffix();
    if ( QImageReader::supportedImageFormats().contains( QByteArray("svg") ) )
        suffixes << "svgz";
    suffixes << "png";
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

BApplicationPrivate::BApplicationPrivate(BApplication *q) :
    BCoreApplicationPrivate(q)
{
    //
}

BApplicationPrivate::~BApplicationPrivate()
{
    if (aboutDlg)
    {
        aboutDlg->close();
        aboutDlg->deleteLater();
    }
}

//

void BApplicationPrivate::init()
{
    aboutDlg = 0;
    iconCaching = false;
    navigation = BApplication::DefaultNavigation;
    connect( q_func(), SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

void BApplicationPrivate::initAboutDlg()
{
    if (aboutDlg)
        return;
    BAboutDialog::AboutOptions opt;
    opt.aboutQtButton = true;
    opt.aboutBeQtButton = true;
    aboutDlg = new BAboutDialog(opt);
    aboutDlg->setWindowModality(Qt::NonModal);
}

void BApplicationPrivate::showAbout()
{
    initAboutDlg();
    if ( aboutDlg->isVisible() )
        aboutDlg->activateWindow();
    else
        aboutDlg->open();
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
    hb->show();
}

//

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
================================ Application
============================================================================*/

QIcon BApplication::icon(const QString &name, const QString &theme)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return QIcon();
    B_DS(BApplication);
    if ( ds->iconCaching && ds->iconCache.contains(name) )
        return ds->iconCache.value(name);
    QStringList pplist = QIcon::themeSearchPaths();
    QStringList plist = pplist;
    foreach ( const QString &path, locations("icons") )
        plist.insert(plist.size() - 1, path);
    QIcon::setThemeSearchPaths(plist);
    QIcon icn = QIcon::fromTheme( !theme.isEmpty() ? theme : QIcon::themeName(), beqtIcon(name) );
    QIcon::setThemeSearchPaths(pplist);
    if ( ds->iconCaching && !icn.isNull() )
        ds->iconCache.insert(name, icn);
    return icn;
}

QIcon BApplication::beqtIcon(const QString &name)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return QIcon();
    QString fn = BApplicationPrivate::findImage("beqt/icons", name);
    if ( fn.isEmpty() )
        return QIcon();
    return QIcon(fn);
}

QPixmap BApplication::beqtPixmap(const QString &name, const QSize &scale)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return QPixmap();
    QString fn = BApplicationPrivate::findImage("beqt/images", name);
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
    return QFont( QFontInfo( QFont("monospace") ).family() );
}

//

BApplication::BApplication() :
    BCoreApplication( *new BApplicationPrivate(this) )
{
    d_func()->init();
}

BApplication::~BApplication()
{
    //
}

//

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

//

BApplication::BApplication(BApplicationPrivate &d) :
    BCoreApplication(d)
{
    d_func()->init();
}

BSettingsDialog::SettingsTabMap BApplication::settingsTabMap() const
{
    return BSettingsDialog::SettingsTabMap();
}

void BApplication::handleSettings(const BSettingsDialog::SettingsMap &s)
{
    //
}
