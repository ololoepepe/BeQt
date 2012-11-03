#include <BApplication>
#include <BTerminalIOHandler>

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QLocale>
#include <QEventLoop>
#include <QTimer>
#include <QTextStream>
#include <QMainWindow>
#include <QIcon>
#include <QLibraryInfo>

#include <QDebug>

int main(int argc, char **argv)
{
    QApplication *app = new QApplication(argc, argv);
    QApplication::setApplicationName("My App");
    QApplication::setOrganizationName("darkangel");
    QApplication::setApplicationVersion("0.1.0pa1");
    BApplication *bapp = new BApplication;
    //test
    BApplication::loadSettings();
    //qDebug() << QApplication::libraryPaths();
    QIcon ic = BApplication::beqtIcon("apply");
    QApplication::setWindowIcon(ic);
    QMainWindow *mw = new QMainWindow();
    mw->show();
    bApp->showAbout();
    int ret = app->exec();
    //end test
    BApplication::saveSettings();
    delete bapp;
    delete app;
    return ret;
}
