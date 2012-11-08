#ifndef BMAINWINDOW_H
#define BMAINWINDOW_H

class BMainWindowPrivate;

class QWidget;
class QMenu;
class QSettings;
class QString;
class QCloseEvent;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QMainWindow>

class B_WIDGETS_EXPORT BMainWindow : public QMainWindow, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BMainWindow)
public:
    struct HelpMenuOptions
    {
        bool homepage;
        bool contents;
        bool contextual;
        bool whatsThis;
        bool about;
        //
        HelpMenuOptions()
        {
            homepage = true;
            contents = true;
            contextual = true;
            whatsThis = true;
            about = true;
        }
    };
    //
    explicit BMainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    explicit BMainWindow(const QString &settingsGroup, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~BMainWindow();
public slots:
    void retranslateUi();
    void loadSettings(QSettings *s = 0);
    void saveSettings(QSettings *s = 0);
protected:
    BMainWindow(BMainWindowPrivate &d, QWidget *parent = 0);
    //
    void closeEvent(QCloseEvent *event);
    QMenu *createHelpMenu( const HelpMenuOptions &options = HelpMenuOptions() );
    virtual bool handleClosing();
private:
    Q_DISABLE_COPY(BMainWindow)
};

#endif // BMAINWINDOW_H
