#include "bmainwindow.h"
#include "bmainwindow_p.h"
#include "bapplication.h"

#include <QObject>
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QString>
#include <QWhatsThis>
#include <QKeySequence>
#include <QSettings>
#include <QVariant>
#include <QRect>
#include <QByteArray>
#include <QDesktopWidget>
#include <QCloseEvent>

BMainWindowPrivateObject::BMainWindowPrivateObject(BMainWindowPrivate *p) :
    QObject(0), _m_p(p)
{
    //
}

BMainWindowPrivateObject::~BMainWindowPrivateObject()
{
    //
}

//

void BMainWindowPrivateObject::languageChanged()
{
    _m_p->languageChanged();
}

//

BMainWindowPrivate::BMainWindowPrivate(BMainWindow *q, const QString &sg) :
    BBasePrivate(q), _m_o( new BMainWindowPrivateObject(this) ), SettingsGroup(sg)
{
    QObject::connect( bApp, SIGNAL( languageChanged() ), _m_o, SLOT( languageChanged() ) );
}

BMainWindowPrivate::~BMainWindowPrivate()
{
    _m_o->deleteLater();
}

//

void BMainWindowPrivate::languageChanged()
{
    //
}

void BMainWindowPrivate::retranslateAction(QAction *action, Action type) const
{
    if (!action)
        return;
    switch (type)
    {
    case HomepageAction:
        action->setText( tr("Homepage", "act text") );
        action->setToolTip( tr("Homepage", "act toolTip") );
        action->setWhatsThis( tr("Homepage", "act whatsThis") );
        break;
    case ContentsAction:
        action->setText( tr("Contents", "act text") );
        action->setToolTip( tr("Contents", "act toolTip") );
        action->setWhatsThis( tr("Contents", "act whatsThis") );
        break;
    case ContextualAction:
        action->setText( tr("Contextual", "act text") );
        action->setToolTip( tr("Contextual", "act toolTip") );
        action->setWhatsThis( tr("Contextual", "act whatsThis") );
        break;
    case WhatsThisAction:
        action->setText( tr("What's this?", "act text") );
        action->setToolTip( tr("What's this?", "act toolTip") );
        action->setWhatsThis( tr("What's this?", "act whatsThis") );
        break;
    case AboutAction:
        action->setText( tr("About", "act text") );
        action->setToolTip( tr("About", "act toolTip") );
        action->setWhatsThis( tr("About", "act whatsThis") );
        break;
    default:
        break;
    }
}

void BMainWindowPrivate::retranslateMenu(QMenu *menu, Menu type) const
{
    if (!menu)
        return;
    switch (type)
    {
    case HelpMenu:
        menu->setTitle( tr("Help", "mnu title") );
        //menu->setToolTip( tr("", "mnu toolTip") );
        //menu->setWhatsThis( tr("", "mnu whatsThis") );
        break;
    default:
        break;
    }
}

//

BMainWindow::BMainWindow(QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags), BBase( *new BMainWindowPrivate(this, "") )
{
    //
}

BMainWindow::BMainWindow(const QString &settingsGroup, QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags), BBase( *new BMainWindowPrivate(this, settingsGroup) )
{
    //
}

BMainWindow::~BMainWindow()
{
    //
}

//

void BMainWindow::retranslateUi()
{
    //
}

void BMainWindow::loadSettings(QSettings *s)
{
    if (!s)
        s = BApplication::createAppSettingsInstance();
    if (!s)
        return;
    QString sg = d_func()->SettingsGroup;
    if ( !sg.isEmpty() )
        sg += "/";
    restoreGeometry( s->value(sg + "geometry").toByteArray() );
    restoreState( s->value(sg + "state").toByteArray() );
}

void BMainWindow::saveSettings(QSettings *s)
{
    if (!s)
        s = BApplication::createAppSettingsInstance();
    if (!s)
        return;
    QString sg = d_func()->SettingsGroup;
    if ( !sg.isEmpty() )
        sg += "/";
    s->setValue( sg + "geometry", saveGeometry() );
    s->setValue( sg + "state", saveState() );
}

//

BMainWindow::BMainWindow(BMainWindowPrivate &d, QWidget *parent) :
    QMainWindow(parent), BBase(d)
{
    //
}

//

void BMainWindow::closeEvent(QCloseEvent *event)
{
    if ( handleClosing() )
        QMainWindow::closeEvent(event);
    else
        event->ignore();
}

QMenu *BMainWindow::createHelpMenu(const HelpMenuOptions &options)
{
    const B_D(BMainWindow);
    QMenu *menu = new QMenu(this);
    if (options.homepage)
    {
        QAction *act = new QAction(this);
        act->setObjectName("ActionHomepage");
        d->retranslateAction(act, BMainWindowPrivate::HomepageAction);
        connect( act, SIGNAL( triggered() ), bApp, SLOT( openHomepage() ) );
        menu->addAction(act);
    }
    if (options.contents)
    {
        QAction *act = new QAction(this);
        act->setObjectName("ActionContents");
        if (!options.contextual)
            act->setShortcut( QKeySequence("F1") );
        d->retranslateAction(act, BMainWindowPrivate::ContentsAction);
        connect( act, SIGNAL( triggered() ), bApp, SLOT( showHelpContents() ) );
        if ( !menu->isEmpty() )
            menu->addSeparator();
        menu->addAction(act);
    }
    if (options.contextual)
    {
        QAction *act = new QAction(this);
        act->setObjectName("ActionContextual");
        act->setShortcut( QKeySequence("F1") );
        d->retranslateAction(act, BMainWindowPrivate::ContextualAction);
        connect( act, SIGNAL( triggered() ), bApp, SLOT( showContextualHelp() ) );
        if (!menu->isEmpty() && !options.contents)
            menu->addSeparator();
        menu->addAction(act);
    }
    if (options.whatsThis)
    {
        QAction *act = QWhatsThis::createAction(this);
        act->setObjectName("ActionWhatsThis");
        if (!menu->isEmpty() && !options.contents && !options.contextual)
            menu->addSeparator();
        menu->addAction(act);
    }
    if (options.about)
    {
        QAction *act = new QAction(this);
        act->setObjectName("ActionAbout");
        d->retranslateAction(act, BMainWindowPrivate::AboutAction);
        connect( act, SIGNAL( triggered() ), bApp, SLOT( showAboutDialog() ) );
        if ( !menu->isEmpty() )
            menu->addSeparator();
        menu->addAction(act);
    }
    if ( menu->isEmpty() )
    {
        menu->deleteLater();
        menu = 0;
    }
    else
    {
        menu->setObjectName("MenuHelp");
        d->retranslateMenu(menu, BMainWindowPrivate::HelpMenu);
    }
    return menu;
}

bool BMainWindow::handleClosing()
{
    return true;
}
