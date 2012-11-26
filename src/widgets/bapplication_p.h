#ifndef BAPPLICATION_P_H
#define BAPPLICATION_P_H

class BApplicationPrivate;

class QStringList;
class QAction;

#include "bapplication.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bcoreapplication_p.h>

#include <QObject>
#include <QString>
#include <QMap>
#include <QIcon>
#include <QPointer>

/*============================================================================
================================ Application Private
============================================================================*/

class B_WIDGETS_EXPORT BApplicationPrivate : public BCoreApplicationPrivate
{
    B_DECLARE_PUBLIC(BApplication)
    Q_OBJECT
    B_DECLARE_TR_FUNCTIONS(BApplication, q)
public:
    static void retranslateStandardAction(QAction *act);
    static QString findImage(const QString &subdir, const QString &name);
    //
    BApplicationPrivate(BApplication *q);
    ~BApplicationPrivate();
    //
    void init();
    void initAboutDlg();
    void showAbout();
    QString helpContext(QWidget *widget = 0) const;
    QString getHelpIndex() const;
    QStringList helpSearchPaths() const;
    void showHelp( const QString &file = QString() );
    //
    QString homepage;
    QPointer<BAboutDialog> aboutDlg;
    BApplication::SettingsTabNavigation navigation;
    QString helpIndex;
    QMap<QString, QIcon> iconCache;
    bool iconCaching;
    QMap<QObject *, QAction *> actions;
public slots:
    void retranslateUi();
    void actionDestroyed(QObject *act);
private:
    Q_DISABLE_COPY(BApplicationPrivate)
};

#endif // BAPPLICATION_P_H
