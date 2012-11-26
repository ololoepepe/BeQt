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
    struct AboutOptions
    {
        bool aboutQtButton;
        bool aboutBeQtButton;
        QString appName;
        QString appVersion;
    };
    //
    explicit BAboutDialog(QWidget *parent = 0);
    explicit BAboutDialog(const AboutOptions &options, QWidget *parent = 0);
    ~BAboutDialog();
    //
    void setPixmap(const QPixmap &pixmap);
    void setPixmap(const QString &fileName);
    void setAbout( const QString &description, const QString &copyright, const QString &website = QString() );
    void setChangeLog(const QString &text);
    void setChangeLog(const QString &fileName, const char *codecName);
    void setAuthorsInfos(const BPersonInfoProvider::PersonInfoList &infos);
    void setTranslationInfos(const BPersonInfoProvider::PersonInfoList &infos);
    void setThanksToInfos(const BPersonInfoProvider::PersonInfoList &infos);
    void setLicense(const QString &text);
    void setLicense(const QString &fileName, const char *codecName);
    void resetTabs();
protected:
    BAboutDialog(BAboutDialogPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BAboutDialog)
};

#endif // BABOUTDIALOG_H
