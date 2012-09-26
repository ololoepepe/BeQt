#include "bmainwindow.h"
#include "babstractsettingstab.h"
#include "bguiplugininterface.h"

#include "private/baboutdialog.h"
#include "private/bhelpwidget.h"
#include "private/bsettingsdialog.h"
#include "private/bgeneralsettingstab.h"

#include "../bcore/bcore.h"

#include <QString>
#include <QLayout>
#include <QRect>
#include <QApplication>
#include <QWidget>
#include <QTranslator>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QKeySequence>
#include <QTimer>
#include <QCloseEvent>
#include <QStringList>
#include <QSettings>
#include <QByteArray>
#include <QDir>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QList>
#include <QCursor>
#include <QPoint>
#include <QIcon>
#include <QScopedPointer>
#include <QWhatsThis>
#include <QSize>

#include <QDebug>

const QSize BMainWindow::_m_HelpWgtSizeDef = QSize(640, 640);
const int BMainWindow::_m_StateVersion = 1;
const QString BMainWindow::_m_GroupMainWindow = "beqt_main_window";
  const QString BMainWindow::_m_KeyGeometry = "geometry";
  const QString BMainWindow::_m_KeyState = "state";
  const QString BMainWindow::_m_KeyIsMaximized = "is_maximized";

//

  BMainWindow::BMainWindow(const QRect &defaultGeometry, const QString &settingsGroup) :
    QMainWindow(0), _m_CSettingsGroup(settingsGroup)
{
    _m_prevGeom = defaultGeometry;
    _m_maximized = false;
    _m_aboutDlg = new BAboutDialog(this);
    _m_aboutDlg->setFixedWidth(680);
    _m_aboutDlg->resize(680, 340);
    _m_settingsOptions.language = true;
    _m_settingsOptions.multipleInstances = true;
    _m_settingsOptions.plugins = true;
    //
    _m_mnuBar = menuBar();
    _m_mnuFile = new QMenu(this);
      _m_actExit = new QAction(this);
        _m_actExit->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_Q) );
        _m_actExit->setIcon( QIcon( BCore::beqtIcon("window_close") ) );
        connect( _m_actExit, SIGNAL( triggered() ), this, SLOT( close() ) );
      _m_mnuFile->addAction(_m_actExit);
    _m_mnuBar->addMenu(_m_mnuFile);
    _m_mnuEdit = new QMenu(this);
      _m_actSettings = new QAction(this);
        _m_actSettings->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_P) );
        _m_actSettings->setIcon( QIcon( BCore::beqtIcon("configure") ) );
        connect( _m_actSettings, SIGNAL( triggered() ), this, SLOT( _m_actSettingsTriggered() ) );
      _m_mnuEdit->addAction(_m_actSettings);
    _m_mnuBar->addMenu(_m_mnuEdit);
    _m_mnuHelp = new QMenu(this);
      _m_actHomepage = new QAction(this);
        _m_actHomepage->setIcon( QIcon( BCore::beqtIcon("network") ) );
        connect( _m_actHomepage, SIGNAL( triggered() ), this, SLOT( _m_actHomepageTriggered() ) );
      _m_mnuHelp->addAction(_m_actHomepage);
      _m_mnuHelp->addSeparator();
      _m_actHelpContents = new QAction(this);
        _m_actHelpContents->setIcon( QIcon( BCore::beqtIcon("help_contents") ) );
        connect( _m_actHelpContents, SIGNAL( triggered() ), this, SLOT( _m_actHelpContentsTriggered() ) );
      _m_mnuHelp->addAction(_m_actHelpContents);
      _m_actContextualHelp = new QAction(this);
        _m_actContextualHelp->setShortcut( QKeySequence(Qt::Key_F1) );
        _m_actContextualHelp->setIcon( QIcon( BCore::beqtIcon("help_contextual") ) );
        connect( _m_actContextualHelp, SIGNAL( triggered() ), this, SLOT( _m_actContextualHelpTriggered() ) );
      _m_mnuHelp->addAction(_m_actContextualHelp);
      _m_actWhatsThis = QWhatsThis::createAction(this);
        //_m_actWhatsThis->setCheckable(false); //TODO: maybe a Qt bug? Yhis action should not be checkable
      _m_mnuHelp->addAction(_m_actWhatsThis);
      _m_mnuHelp->addSeparator();
      _m_actAbout = new QAction(this);
        _m_actAbout->setIcon( QIcon( BCore::beqtIcon("help_about") ) );
        connect( _m_actAbout, SIGNAL( triggered() ), this, SLOT( _m_actAboutTriggered() ) );
      _m_mnuHelp->addAction(_m_actAbout);
      _m_actAboutQt = new QAction(this);
        _m_actAboutQt->setIcon( QIcon( BCore::beqtIcon("qt_logo") ) );
        connect( _m_actAboutQt, SIGNAL( triggered() ), this, SLOT( _m_actAboutQtTriggered() ) );
      _m_mnuHelp->addAction(_m_actAboutQt);
    _m_mnuBar->addMenu(_m_mnuHelp);
    //
    _m_loadSettings();
    _m_retranslateUi();
    connect( BCore::instance(), SIGNAL( localeChanged() ), this, SLOT( _m_retranslateUi() ) );
}

//

void BMainWindow::setSettingsOptions(const SettingsOptions &opt)
{
    _m_settingsOptions = opt;
}

void BMainWindow::setHelpDir(const QString &dir)
{
    _m_hlpDir = dir;
}

void BMainWindow::setHelpIndex(const QString &fileName)
{
    _m_hlpIndex = fileName;
}

void BMainWindow::setContextualHelpEnabled(bool enabled)
{
    _m_actContextualHelp->setVisible(enabled);
    if (enabled)
    {
        _m_actHelpContents->setShortcut( QKeySequence() );
        _m_actContextualHelp->setShortcut( QKeySequence("F1") );
    }
    else
    {
        _m_actHelpContents->setShortcut( QKeySequence("F1") );
        _m_actContextualHelp->setShortcut( QKeySequence() );
    }
}

void BMainWindow::setMenuBarEnabled(bool enabled)
{
    _m_mnuBar->setVisible(enabled);
}

void BMainWindow::setAboutIcon(const QString &fileName)
{
    _m_aboutDlg->setIcon(fileName);
}

void BMainWindow::setAboutText(const QString &text, bool html)
{
    _m_aboutDlg->setAboutText(text, html);
}

void BMainWindow::setAboutChangeLog(const QString &fileName, const char *codecName)
{
    _m_aboutDlg->setChangeLog(fileName, codecName);
}

void BMainWindow::setAboutAuthors(const PersonInfoList &list)
{
    BAboutDialog::PersonInfoList infos;
    BAboutDialog::PersonInfo info;
    for (int i = 0; i < list.size(); ++i)
    {
        const PersonInfo &pi = list.at(i);
        info.name = pi.name;
        info.mail = pi.mail;
        info.site = pi.site;
        info.role = pi.role;
        infos << info;
    }
    _m_aboutDlg->setAuthorsInfos(infos);
}

void BMainWindow::setAboutTranslators(const PersonInfoList &list)
{
    BAboutDialog::PersonInfoList infos;
    BAboutDialog::PersonInfo info;
    for (int i = 0; i < list.size(); ++i)
    {
        const PersonInfo &pi = list.at(i);
        info.name = pi.name;
        info.mail = pi.mail;
        info.site = pi.site;
        info.role = pi.role;
        infos << info;
    }
    _m_aboutDlg->setTranslationInfos(infos);
}

void BMainWindow::setAboutThanksTo(const PersonInfoList &list, bool beqt, bool coelho)
{
    BAboutDialog::PersonInfoList infos;
    BAboutDialog::PersonInfo info;
    for (int i = 0; i < list.size(); ++i)
    {
        const PersonInfo &pi = list.at(i);
        info.name = pi.name;
        info.mail = pi.mail;
        info.site = pi.site;
        info.role = pi.role;
        infos << info;
    }
    if (beqt)
    {
        info.name = tr("BeQt project", "aboutWgt infoName");
        info.mail = "";
        info.site = "https://github.com/the-dark-angel/BeQt";
        info.role = tr("This application is created using the BeQt libraries", "aboutWgt infoRole");
        infos << info;
    }
    if (coelho)
    {
        info.name = "Oxygen";
        info.mail = "";
        info.site = "http://www.oxygen-icons.org/";
        info.role = tr("BeQt uses Oxygen as an icon set", "aboutWgt infoRole");
        infos << info;
    }
    _m_aboutDlg->setThanksToInfos(infos);
}

void BMainWindow::setAboutLicense(const QString &fileName, const char *codecName, const QString &iconFileName)
{
    _m_aboutDlg->setLicense(fileName, codecName, iconFileName);
}

void BMainWindow::insertMenu(QMenu *menu, StandardMenu beforeMenu)
{
    if (!menu)
        return;
    QMenu *before = _m_menu(beforeMenu);
    if (!before)
        return;
    _m_mnuBar->insertMenu(before->menuAction(), menu);
}

void BMainWindow::insertAction(QAction *action, StandardMenu beforeMenu)
{
    if (!action)
        return;
    QMenu *before = _m_menu(beforeMenu);
    if (!before)
        return;
    _m_mnuBar->insertAction(before->menuAction(), action);
}

void BMainWindow::addToMenu(StandardMenu standardMenu, QAction *action)
{
    if (!action)
        return;
    QMenu *menuTo = _m_menu(standardMenu);
    if (!menuTo)
        return;
    QAction *before = _m_menuDefAction(standardMenu);
    if (!before)
        return;
    menuTo->insertAction(before, action);
}

void BMainWindow::addToMenu(StandardMenu standardMenu,
                            const QList<QAction *> &actions)
{
    for (int i = 0; i < actions.size(); ++i)
        addToMenu( standardMenu, actions.at(i) );
}

void BMainWindow::addToMenu(StandardMenu standardMenu, QMenu *menu)
{
    if (!menu)
        return;
    QMenu *menuTo = _m_menu(standardMenu);
    if (!menuTo)
        return;
    QAction *before = _m_menuDefAction(standardMenu);
    if (!before)
        return;
    menuTo->insertMenu(before, menu);
}

void BMainWindow::addSeparatorToMenu(StandardMenu standardMenu)
{
    QMenu *menuTo = _m_menu(standardMenu);
    if (!menuTo)
        return;
    QAction *before = _m_menuDefAction(standardMenu);
    if (!before)
        return;
    menuTo->insertSeparator(before);
}

QAction *BMainWindow::whatsThisAction() const
{
    return _m_actWhatsThis;
}

const QString &BMainWindow::settingsGroup() const
{
    return _m_CSettingsGroup;
}

const QString &BMainWindow::helpDir() const
{
    return _m_hlpDir;
}

bool BMainWindow::menuBarEnabled() const
{
    return _m_mnuBar->isVisible();
}

//

void BMainWindow::saveGuiSettings()
{
    _m_saveSettings();
}

//

void BMainWindow::closeEvent(QCloseEvent *e)
{
    if ( handleClosing() )
    {
        _m_saveSettings();
        e->accept();
    }
    else
    {
        e->ignore();
    }
}

bool BMainWindow::handleClosing()
{
    return true;
}

QMap<QString, BAbstractSettingsTab *> BMainWindow::userSettingsTabMap() const
{
    return QMap<QString, BAbstractSettingsTab *>();
}

void BMainWindow::handleUserSettings(const QMap<QString, QVariantMap> &settings)
{
    //
}

//

void BMainWindow::_m_saveSettings()
{
    QScopedPointer<QSettings> s( BCore::newSettingsInstance() );
    if (!s)
        return;
    if ( !_m_CSettingsGroup.isEmpty() )
        s->beginGroup(_m_CSettingsGroup);
    s->beginGroup(_m_GroupMainWindow);
      s->setValue( _m_KeyGeometry, geometry() );
      s->setValue( _m_KeyState, saveState(_m_StateVersion) );
      s->setValue( _m_KeyIsMaximized, (bool) (windowState() & Qt::WindowMaximized) );
    s->endGroup();
    if ( !_m_CSettingsGroup.isEmpty() )
        s->endGroup();
}

void BMainWindow::_m_loadSettings()
{
    QScopedPointer<QSettings> s( BCore::newSettingsInstance() );
    if (!s)
        return;
    if ( !_m_CSettingsGroup.isEmpty() )
        s->beginGroup(_m_CSettingsGroup);
    s->beginGroup(_m_GroupMainWindow);
      _m_prevGeom = s->value(_m_KeyGeometry, _m_prevGeom).toRect();
      _m_prevState = s->value(_m_KeyState).toByteArray();
      _m_maximized = s->value(_m_KeyIsMaximized, false).toBool();
    s->endGroup();
    if ( !_m_CSettingsGroup.isEmpty() )
        s->endGroup();
    //
    setGeometry(_m_prevGeom);
    if (_m_maximized)
        setWindowState(Qt::WindowMaximized);
    QTimer::singleShot( 0, this, SLOT( _m_restoreState() ) );
}

QString BMainWindow::_m_hlpFileName(QWidget *widget)
{
    if (!widget)
        return _m_hlpIndex;
    QString fn = widget->property("help").toString();
    if ( fn.isEmpty() )
        fn = _m_hlpFileName( widget->parentWidget() );
    if ( fn.isEmpty() )
        fn = _m_hlpIndex;
    return fn;
}

QMenu *BMainWindow::_m_menu(StandardMenu menu) const
{
    switch (menu)
    {
    case MenuFile:
        return _m_mnuFile;
    case MenuEdit:
        return _m_mnuEdit;
    case MenuHelp:
        return _m_mnuHelp;
    default:
        return 0;
    }
}

QAction *BMainWindow::_m_menuDefAction(StandardMenu menu) const
{
    switch (menu)
    {
    case MenuFile:
        return _m_actExit;
    case MenuEdit:
        return _m_actSettings;
    case MenuHelp:
        return _m_actHomepage;
    default:
        return 0;
    }
}

//

void BMainWindow::_m_retranslateUi()
{
    //MenuFile
    _m_mnuFile->setTitle( tr("File", "mnu title") );
    _m_actExit->setText( tr("Exit", "act text") );
    //MenuEdit
    _m_mnuEdit->setTitle( tr("Edit", "mnu title") );
    _m_actSettings->setText(tr("Settings", "act text") + "...");
    _m_actSettings->setWhatsThis( tr("Use this action to configure the application and it's plugins (if any)",
                                     "act whatsThis") );
    //MenuHelp
    _m_mnuHelp->setTitle( tr("Help", "mnu title") );
    _m_actHomepage->setText( tr("Open homepage", "act text") );
    _m_actHomepage->setWhatsThis( tr("Use this action to open the application's home page with your web browser",
                                     "act whatsThis") );
    _m_actHelpContents->setText( tr("Contents", "act text") );
    _m_actHelpContents->setWhatsThis( tr("Use this action to show the application's Help contents", "act whatsThis") );
    _m_actContextualHelp->setText( tr("Contextual help", "act text") );
    _m_actContextualHelp->setWhatsThis( tr("Use this action to show Help for the currently active part "
                                           "of the application", "act whatsThis") );
    QAction *wtact = QWhatsThis::createAction();
    _m_actWhatsThis->setText( wtact->text() );
    _m_actWhatsThis->setToolTip( wtact->toolTip() );
    wtact->deleteLater();
    _m_actAbout->setText(tr("About", "act text") + "...");
    _m_actAbout->setWhatsThis( tr("Use this action to show information about the application, "
                                  "it's version, authors, license, etc.", "act whatsThis") );
    _m_actAboutQt->setText(tr("About the Qt framework", "act text") + "...");
    _m_actAboutQt->setWhatsThis( tr("Use this action to show information about the libraries "
                                    "with which this application is built", "act whatsThis") );
    //AboutDialog
    setAboutThanksTo( PersonInfoList() );
}

void BMainWindow::_m_restoreState()
{
    if (QByteArray() != _m_prevState)
        restoreState(_m_prevState, _m_StateVersion);
}

void BMainWindow::_m_showHide()
{
    if ( isVisible() )
    {
        _m_prevGeom = geometry();
        hide();
    }
    else
    {
        show();
        setGeometry(_m_prevGeom);
    }
}

void BMainWindow::_m_actSettingsTriggered()
{
    QMap<QString, BAbstractSettingsTab *> m;
    QVariantMap gstm;
    if (_m_settingsOptions.language)
        gstm.insert( BGeneralSettingsTab::IdLocale, BCore::currentLocale() );
    if (_m_settingsOptions.multipleInstances)
        gstm.insert( BGeneralSettingsTab::IdMultipleInstances, BCore::multipleInstancesEnabled() );
    if (_m_settingsOptions.plugins)
        gstm.insert( BGeneralSettingsTab::IdPlugins, QVariant::fromValue< QList<QObject *> >( BCore::plugins() ) );
    if ( !gstm.isEmpty() )
        m.insert( BGeneralSettingsTab::Id, new BGeneralSettingsTab(gstm) );
    m.unite( userSettingsTabMap() );
    QScopedPointer<BSettingsDialog> sd( new BSettingsDialog(m, this) );
    if (sd->exec() != BSettingsDialog::Accepted)
        return;
    QMap<QString, QVariantMap> s = sd->valueMapMap();
    if ( s.contains(BGeneralSettingsTab::Id) )
    {
        QVariantMap gsts = s.take(BGeneralSettingsTab::Id);
        if ( gsts.contains(BGeneralSettingsTab::IdLocale) )
            BCore::setLocale( gsts.value(BGeneralSettingsTab::IdLocale).toLocale() );
        if ( gsts.contains(BGeneralSettingsTab::IdMultipleInstances) )
            BCore::setMultipleInstancesEnabled( gsts.value(BGeneralSettingsTab::IdMultipleInstances).toBool() );
    }
    handleUserSettings(s);
}

void BMainWindow::_m_actHomepageTriggered()
{
    QDesktopServices::openUrl( QUrl( QApplication::organizationDomain() ) );
}

void BMainWindow::_m_actHelpContentsTriggered()
{
    QString url = QUrl::fromLocalFile(_m_hlpDir + "/" + _m_hlpIndex).toString();
    BHelpWidget *hw = new BHelpWidget(_m_CSettingsGroup, url, url);
    hw->resize(_m_HelpWgtSizeDef);
    hw->setAttribute(Qt::WA_DeleteOnClose, true);
    hw->show();
}

void BMainWindow::_m_actContextualHelpTriggered()
{
    QString url;
    QWidget *fw = QApplication::focusWidget();
    url = _m_hlpFileName(fw);
    if ( url.isEmpty() )
        url = _m_hlpIndex;
    QString burl = QUrl::fromLocalFile(_m_hlpDir).toString();
    BHelpWidget *hw = new BHelpWidget(_m_CSettingsGroup, burl + "/" + _m_hlpIndex, burl + "/" + url);
    hw->resize(_m_HelpWgtSizeDef);
    hw->setAttribute(Qt::WA_DeleteOnClose, true);
    hw->show();
}

void BMainWindow::_m_actAboutTriggered()
{
    _m_aboutDlg->resetTabs();
    _m_aboutDlg->exec();
}

void BMainWindow::_m_actAboutQtTriggered()
{
    QMessageBox::aboutQt(this);
}
