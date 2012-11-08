#include "mainwindow.h"

#include <QMenuBar>
#include <QMenu>
#include <QString>
#include <QVariant>

MainWindow::MainWindow(QWidget *parent) :
    BMainWindow("GUI/mainwindow", parent)
{
    menuBar()->addMenu( createHelpMenu() );
    setProperty("help", "mainwindow.html");
}
