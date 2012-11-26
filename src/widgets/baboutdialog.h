#ifndef BABOUTDIALOG_H
#define BABOUTDIALOG_H

class BAboutDialogPrivate;

class QWidget;
class QIcon;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>
#include <BeQtCore/BPluginInterface>
#include <BeQtCore/BPersonInfoProvider>

#include <QDialog>
#include <QList>
#include <QString>
#include <QCoreApplication>

/*============================================================================
================================ About Dialog
============================================================================*/

class B_WIDGETS_EXPORT BAboutDialog : public QDialog, public BBase
{
    B_DECLARE_PRIVATE(BAboutDialog)
    Q_OBJECT
public:
    explicit BAboutDialog(QWidget *parent = 0);
    explicit BAboutDialog( QWidget *parent, const QString &appName, const QString &appVersion = QString() );
    ~BAboutDialog();
    //
    void setAppName(const QString &name);
    void setAppVersion(const QString &version);
    void setPixmap(const QPixmap &pixmap);
    void setPixmap(const QString &fileName);
    void setAbout( const QString &description, const QString &copyright, const QString &website = QString() );
    void setChangeLog(const QString &text);
    void setChangeLog(const QString &fileName, const char *codecName);
    void setAuthors(BPersonInfoProvider *prov);
    void setAuthorsInfos(const BPersonInfoProvider::PersonInfoList &list);
    void setTranslation(BPersonInfoProvider *prov);
    void setTranslationInfos(const BPersonInfoProvider::PersonInfoList &list);
    void setThanksTo(BPersonInfoProvider *prov);
    void setThanksToInfos(const BPersonInfoProvider::PersonInfoList &list);
    void setLicense(const QString &text);
    void setLicense(const QString &fileName, const char *codecName);
    void setAboutQtShown(bool b);
    void setAboutBeqtShown(bool b);
public slots:
    void resetTabs();
protected:
    BAboutDialog(BAboutDialogPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BAboutDialog)
};

#endif // BABOUTDIALOG_H
