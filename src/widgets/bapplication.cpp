#include "bapplication.h"
#include "bapplication_p.h"
#include "baboutdialog.h"
#include "bsettingsdialog.h"
#include "bhelpbrowser.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BCoreApplication>
#include <BeQtCore/private/bcoreapplication_p.h>
#include <BeQtCore/BDirTools>

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

#include <QDebug>
#include <QPointer>

/*============================================================================
================================ Application Private
============================================================================*/

BApplicationPrivate::BApplicationPrivate(BApplication *q) :
    BCoreApplicationPrivate(q)
{
    aboutDlg = 0;
    navigation = BApplication::DefaultNavigation;
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

/*============================================================================
================================ Application
============================================================================*/

QIcon BApplication::icon(const QString &name, const QString &theme)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return QIcon();
    QStringList pplist = QIcon::themeSearchPaths();
    QStringList plist = pplist;
    foreach ( const QString &path, locations("icons") )
        plist.insert(plist.size() - 1, path);
    QIcon::setThemeSearchPaths(plist);
    QIcon icn = QIcon::fromTheme( !theme.isEmpty() ? theme : QIcon::themeName(), beqtIcon(name) );
    QIcon::setThemeSearchPaths(pplist);
    return icn;
}

QIcon BApplication::beqtIcon(const QString &name)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return QIcon();
    if ( name.isEmpty() )
        return QIcon();
    QString nm = !QFileInfo(name).suffix().isEmpty() ? name : (name + ".png");
    QString fn = BDirTools::findResource("beqt/icons/" + nm, BDirTools::GlobalOnly);
    if ( fn.isEmpty() )
        return QIcon();
    return QIcon(fn);
}

QPixmap BApplication::beqtPixmap(const QString &name, const QSize &scale)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return QPixmap();
    if ( name.isEmpty() )
        return QPixmap();
    QString nm = !QFileInfo(name).suffix().isEmpty() ? name : (name + ".png");
    QString fn = BDirTools::findResource("beqt/images/" + nm, BDirTools::GlobalOnly);
    if ( fn.isEmpty() )
        return QPixmap();
    QPixmap pm(fn);
    if ( pm.isNull() )
        return QPixmap();
    return scale.isEmpty() ? pm : pm.scaled(scale, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void BApplication::setAboutPixmap(const QPixmap &pixmap)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setPixmap(pixmap);
}

void BApplication::setAboutPixmap(const QString &fileName)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setPixmap(fileName);
}

void BApplication::setAbout(const QString &description, const QString &copyright, const QString &website)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->homepage = website;
    ds->initAboutDlg();
    ds->aboutDlg->setAbout(description, copyright, website);
}

void BApplication::setAboutChangeLog(const QString &fileName, const char *codecName)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setChangeLog(fileName, codecName);
}

void BApplication::setAboutAuthorsInfos(const BAboutDialog::PersonInfoList &infos)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setAuthorsInfos(infos);
}

void BApplication::setAboutTranslationInfos(const BAboutDialog::PersonInfoList &infos)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setTranslationInfos(infos);
}

void BApplication::setAboutThanksToInfos(const BAboutDialog::PersonInfoList &infos)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setThanksToInfos(infos);
}

void BApplication::setAboutLicense(const QString &text)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setLicense(text);
}

void BApplication::setAboutLicense(const QString &fileName, const char *codecName)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setLicense(fileName, codecName);
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
        //act->setIcon( beqtIcon("") );
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
        //act->setIcon( beqtIcon("") );
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
    act->setProperty("standard_action_type", type);
    retranslateStandardAction(act);
    return act;
}

void BApplication::retranslateStandardAction(QAction *action)
{
    if (!action)
        return;
    bool ok = false;
    StandardAction type = static_cast<StandardAction>( action->property("standard_action_type").toInt(&ok) );
    if (!ok || InvalidAction == type)
        return;
    switch (type)
    {
    case SettingsAction:
        action->setText( tr("Settings...", "act text") );
        action->setToolTip( tr("Show Settings dialog", "act toolTip") );
        action->setWhatsThis( tr("Use this action to show application settings dialog", "act whatsThis") );
        break;
    case HomepageAction:
        action->setText( tr("Homepage", "act text") );
        action->setToolTip( tr("Go to homepage", "act toolTip") );
        action->setWhatsThis( tr("Use this action to show application homepage with your browser", "act whatsThis") );
        break;
    case HelpContentsAction:
        action->setText( tr("Help contents", "act text") );
        action->setToolTip( tr("Show Help contents", "act toolTip") );
        action->setWhatsThis( tr("Use this action to show Help contents (index)", "act whatsThis") );
        break;
    case ContextualHelpAction:
        action->setText( tr("Contextual Help", "act text") );
        action->setToolTip( tr("Show contextual Help", "act toolTip") );
        action->setWhatsThis( tr("Use this action to open context-specific Help page", "act whatsThis") );
        break;
    case WhatsThisAction:
    {
        QAction *wta = QWhatsThis::createAction();
        action->setText( wta->text() );
        action->setToolTip( wta->toolTip() );
        wta->deleteLater();
        break;
    }
    case AboutAction:
        action->setText( tr("About", "act text") );
        action->setToolTip( tr("Show About dialog", "act toolTip") );
        action->setWhatsThis( tr("Use this action to show information about application, it's authors, etc.",
                                 "act whatsThis") );
        break;
    default:
        break;
    }
}

//

BApplication::BApplication() :
    BCoreApplication( *new BApplicationPrivate(this) )
{
    //
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
    //
}

BSettingsDialog::SettingsTabMap BApplication::settingsTabMap() const
{
    return BSettingsDialog::SettingsTabMap();
}

void BApplication::handleSettings(const BSettingsDialog::SettingsMap &s)
{
    //
}
