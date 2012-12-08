#ifndef BAPPLICATION_P_H
#define BAPPLICATION_P_H

class BApplicationPrivate;

class QStringList;
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
    static QString findImage(const QString &subdir, const QString &name);
public:
    void init();
    void initAboutDlg();
    void showAbout();
    QString helpContext(QWidget *widget = 0) const;
    QString getHelpIndex() const;
    QStringList helpSearchPaths() const;
    void showHelp( const QString &file = QString() );
public slots:
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
private:
    Q_DISABLE_COPY(BApplicationPrivate)
};

#endif // BAPPLICATION_P_H
