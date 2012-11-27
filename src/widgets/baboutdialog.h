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
================================ BAboutDialog ================================
============================================================================*/

class B_WIDGETS_EXPORT BAboutDialog : public QDialog, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BAboutDialog)
public:
    explicit BAboutDialog(QWidget *parent = 0);
    explicit BAboutDialog( QWidget *parent, const QString &appName, const QString &appVersion = QString() );
    ~BAboutDialog();
protected:
    explicit BAboutDialog(BAboutDialogPrivate &d, QWidget *parent = 0);
public:
    void setAppName(const QString &name);
    void setAppVersion(const QString &version);
    void setOrganization( const QString &organization, const QString &copyrightPeriod = QString() );
    void setWebsite(const QString &site);
    void setPixmap(const QPixmap &pixmap);
    void setPixmap(const QString &fileName);
    void setDescription(const QString &text);
    void setDescriptionSource( const QString &fileName, const QString &defaultFileName,
                               const QString &possibleSuffix = QString() );
    void setChangeLog(const QString &text);
    void setChangeLogSource( const QString &fileName, const QString &defaultFileName,
                             const QString &possibleSuffix = QString() );
    void setLicense(const QString &text);
    void setLicenseSource( const QString &fileName, const QString &defaultFileName,
                           const QString &possibleSuffix = QString() );
    void setAuthors(const BPersonInfoProvider::PersonInfoList &list);
    void setAuthorsProvider(BPersonInfoProvider *prov);
    void setTranslators(const BPersonInfoProvider::PersonInfoList &list);
    void setTranslatorsProvider(BPersonInfoProvider *prov);
    void setThanksTo(const BPersonInfoProvider::PersonInfoList &list);
    void setThanksToProvider(BPersonInfoProvider *prov);
    void setAboutQtShown(bool b);
    void setAboutBeqtShown(bool b);
public slots:
    void resetTabs();
private:
    Q_DISABLE_COPY(BAboutDialog)
};

#endif // BABOUTDIALOG_H
