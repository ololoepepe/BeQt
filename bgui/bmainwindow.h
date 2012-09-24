#ifndef BMAINWINDOW_H
#define BMAINWINDOW_H

class BAbstractSettingsTab;
class BAboutDialog;

class QMenu;
class QCloseEvent;
class QWidget;
class QAction;
class QMenuBar;

#include <QMainWindow>
#include <QList>
#include <QPointer>
#include <QIcon>
#include <QByteArray>
#include <QMap>
#include <QVariantMap>
#include <QLocale>
#include <QString>
#include <QRect>
#include <QStringList>
#include <QApplication>
#include <QDesktopWidget>
#include <QSize>

#if defined(BGUI_LIBRARY)
#  define BGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define BGUISHARED_EXPORT Q_DECL_IMPORT
#endif

class BGUISHARED_EXPORT BMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum StandardMenu
    {
        MenuFile = 0x01,
        MenuEdit = 0x02,
        MenuHelp = 0x04
    };
    //
    struct SettingsOptions
    {
        bool language;
        bool multipleInstances;
        bool plugins;
    };
    struct PersonInfo
    {
        QString name;
        QString mail;
        QString site;
        QString role;
    };
    //
    typedef QList<PersonInfo> PersonInfoList;
    //
    explicit BMainWindow(
            const QRect &defaultGeometry = QApplication::desktop()->availableGeometry().adjusted(25, 25, -50, -50),
            const QString &settingsGroup = QString() );
    //
    void setSettingsOptions(const SettingsOptions &opt);
    void setHelpDir(const QString &dir);
    void setHelpIndex(const QString &fileName);
    void setContextualHelpEnabled(bool enabled);
    void setMenuBarEnabled(bool enabled);
    void setAboutIcon(const QString &fileName);
    void setAboutText(const QString &text, bool html = false);
    void setAboutChangeLog(const QString &fileName, const char *codecName);
    void setAboutAuthors(const PersonInfoList &list);
    void setAboutTranslators(const PersonInfoList &list);
    void setAboutThanksTo(const PersonInfoList &list, bool beqt = true, bool coelho = true);
    void setAboutLicense( const QString &fileName, const char *codecName, const QString &iconFileName = QString() );
    void insertMenu(QMenu *menu, StandardMenu beforeMenu = MenuHelp);
    void insertAction(QAction *action, StandardMenu beforeMenu = MenuHelp);
    void addToMenu(StandardMenu standardMenu, QAction *action);
    void addToMenu(StandardMenu standardMenu, const QList<QAction *> &actions);
    void addToMenu(StandardMenu standardMenu, QMenu *menu);
    void addSeparatorToMenu(StandardMenu standardMenu);
    QAction *whatsThisAction() const;
    const QString &settingsGroup() const;
    const QString &helpDir() const;
    bool menuBarEnabled() const;
public slots:
    void saveGuiSettings();
protected:
    void closeEvent(QCloseEvent *event);
    virtual bool handleClosing();
    virtual QMap<QString, BAbstractSettingsTab *> userSettingsTabMap() const;
    virtual void handleUserSettings(const QMap<QString, QVariantMap> &settings);
private:
    static const int _m_StateVersion;
    static const QSize _m_HelpWgtSizeDef;
    //
    static const QString _m_GroupMainWindow;
      static const QString _m_KeyGeometry;
      static const QString _m_KeyState;
      static const QString _m_KeyIsMaximized;
    //
    const QString _m_CSettingsGroup;
    //
    QRect _m_prevGeom;
    QByteArray _m_prevState;
    bool _m_maximized;
    QString _m_hlpDir;
    QString _m_hlpIndex;
    bool _m_isInitialized;
    BAboutDialog *_m_aboutDlg;
    SettingsOptions _m_settingsOptions;
    //
    QMenuBar *_m_mnuBar;
      QMenu *_m_mnuFile;
        QAction *_m_actExit;
      QMenu *_m_mnuEdit;
        QAction *_m_actSettings;
      QMenu *_m_mnuHelp;
        QAction *_m_actHomepage;
        //separator
        QAction *_m_actHelpContents;
        QAction *_m_actContextualHelp;
        QAction *_m_actWhatsThis;
        //separator
        QAction *_m_actAbout;
        QAction *_m_actAboutQt;
    //
    void _m_saveSettings();
    void _m_loadSettings();
    QString _m_hlpFileName(QWidget *widget);
    QMenu * _m_menu(StandardMenu menu) const;
    QAction *_m_menuDefAction(StandardMenu menu) const;
private slots:
    void _m_retranslateUi();
    void _m_restoreState();
    void _m_showHide();
    void _m_actSettingsTriggered();
    void _m_actHomepageTriggered();
    void _m_actHelpContentsTriggered();
    void _m_actContextualHelpTriggered();
    void _m_actAboutTriggered();
    void _m_actAboutQtTriggered();
};

#endif // BMAINWINDOW_H
