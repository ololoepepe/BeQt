#include "mainwindow.h"

#include <BApplication>

#include <QMenuBar>
#include <QMenu>
#include <QString>
#include <QVariant>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QMenu *mnu = new QMenu(this);
    mnu->setTitle("Help");
    mnu->addAction( BApplication::createStandardAction(BApplication::HomepageAction, this) );
    mnu->addSeparator();
    mnu->addAction( BApplication::createStandardAction(BApplication::HelpContentsAction, this) );
    mnu->addAction( BApplication::createStandardAction(BApplication::ContextualHelpAction, this) );
    mnu->addAction( BApplication::createStandardAction(BApplication::WhatsThisAction, this) );
    mnu->addSeparator();
    mnu->addAction( BApplication::createStandardAction(BApplication::AboutAction, this) );
    menuBar()->addMenu(mnu);
    mnu = new QMenu(this);
    mnu->setTitle("Settings");
    mnu->addAction( BApplication::createStandardAction(BApplication::SettingsAction, this) );
    menuBar()->addMenu(mnu);
    setProperty("help", "mainwindow.html");
}
