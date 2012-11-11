#ifndef BAPPLICATION_P_H
#define BAPPLICATION_P_H

class BApplicationPrivate;

class QStringList;

#include "bapplication.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bcoreapplication_p.h>

#include <QString>

class B_WIDGETS_EXPORT BApplicationPrivate : public BCoreApplicationPrivate
{
    B_DECLARE_PUBLIC(BApplication)
    Q_DECLARE_TR_FUNCTIONS(BApplication)
public:
    BApplicationPrivate(BApplication *q, const BCoreApplication::AppOptions &options);
    ~BApplicationPrivate();
    //
    void initAboutDlg();
    void showAbout();
    QString helpContext(QWidget *widget = 0) const;
    QString getHelpIndex() const;
    QStringList helpSearchPaths() const;
    void showHelp( const QString &file = QString() );
    //
    QString homepage;
    BAboutDialog *aboutDlg;
    BApplication::SettingsTabNavigation navigation;
    QString helpIndex;
    QStringList iconPaths;
private:
    Q_DISABLE_COPY(BApplicationPrivate)
    //
    friend class BApplicationPrivateObject;
};

#endif // BAPPLICATION_P_H
