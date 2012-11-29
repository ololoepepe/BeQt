#ifndef BABOUTDIALOG_H
#define BABOUTDIALOG_H

class BAboutDialogPrivate;

class QWidget;
class QIcon;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>
#include <BeQtCore/BPluginInterface>
#include <BeQtCore/BPersonInfoProvider>
#include <BeQtCore/BDirTools>

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
    void setDescriptionSource(const BDirTools::LocaleBasedSource &src);
    void setChangeLog(const QString &text);
    void setChangeLogSource(const BDirTools::LocaleBasedSource &src);
    void setLicense(const QString &text);
    void setLicenseSource(const BDirTools::LocaleBasedSource &src);
    void setAuthors(const BPersonInfoProvider::PersonInfoList &list);
    void setAuthorsFile(const QString &fileName);
    void setAuthorsProvider(BPersonInfoProvider *prov);
    void setTranslators(const BPersonInfoProvider::PersonInfoList &list);
    void setTranslatorsFile(const QString &fileName);
    void setTranslatorsProvider(BPersonInfoProvider *prov);
    void setThanksTo(const BPersonInfoProvider::PersonInfoList &list);
    void setThanksToFile(const QString &fileName);
    void setThanksToProvider(BPersonInfoProvider *prov);
    void setAboutQtShown(bool b);
    void setAboutBeqtShown(bool b);
public slots:
    void resetTabs();
private:
    Q_DISABLE_COPY(BAboutDialog)
};

#endif // BABOUTDIALOG_H
