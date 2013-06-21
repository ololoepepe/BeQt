#ifndef BAPPLICATION_P_H
#define BAPPLICATION_P_H

class BApplicationPrivate;

class QAction;
class QRect;

#include "bapplication.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bcoreapplication_p.h>

#include <QObject>
#include <QString>
#include <QMap>
#include <QIcon>
#include <QPointer>
#include <QStringList>

/*============================================================================
================================ BApplicationPrivate =========================
============================================================================*/

class B_WIDGETS_EXPORT BApplicationPrivate : public BCoreApplicationPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BApplication)
public:
    explicit BApplicationPrivate(BApplication *q);
    ~BApplicationPrivate();
public:
    static void retranslateStandardAction(QAction *act);
    static QString findImage( const QString &subdir, const QString &name,
                              const QStringList &preferredFormats = QStringList() );
    static QIcon iconFromTheme(const QString &name);
public:
    void init();
    void initAboutDlg();
    void showAbout();
    QString helpContext(QWidget *widget = 0) const;
    QString getHelpIndex() const;
    QStringList helpSearchPaths() const;
    void showHelp( const QString &file = QString() );
    QIcon cacheIcon(const QIcon &icon, const QString &name);
public Q_SLOTS:
    void retranslateUi();
    void actionDestroyed(QObject *act);
public:
    QString homepage;
    QPointer<BAboutDialog> aboutDlg;
    BApplication::SettingsTabNavigation navigation;
    QString helpIndex;
    QMap<QString, QIcon> iconCache;
    bool iconCaching;
    QMap<QObject *, QAction *> actions;
    QRect helpBrowserGeometry;
    bool themedIcons;
    QStringList preferredIconFormats;
private:
    Q_DISABLE_COPY(BApplicationPrivate)
};

#endif // BAPPLICATION_P_H
