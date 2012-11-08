#include "mainwindow.h"

#include <QMenuBar>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent) :
    BMainWindow("GUI/mainwindow", parent)
{
    menuBar()->addMenu( createHelpMenu() );
}
