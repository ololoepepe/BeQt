#ifndef BABOUTDIALOG_H
#define BABOUTDIALOG_H

class BAboutDialogPrivate;

class QWidget;
class QIcon;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>
#include <BeQtCore/BPluginInterface>

#include <QDialog>
#include <QList>
#include <QString>
#include <QCoreApplication>

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
        //
        AboutOptions()
        {
            aboutQtButton = false;
            aboutBeQtButton = false;
            appName = QCoreApplication::applicationName();
            appVersion = QCoreApplication::applicationVersion();
        }
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
    static PersonInfoList fromPluginPersonInfoList(const BPluginInterface::PersonInfoList &list);
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
    void setAuthorsInfos(const PersonInfoList &infos);
    void setAuthorsInfos(const BPluginInterface::PersonInfoList &infos);
    void setTranslationInfos(const PersonInfoList &infos);
    void setTranslationInfos(const BPluginInterface::PersonInfoList &infos);
    void setThanksToInfos(const PersonInfoList &infos);
    void setThanksToInfos(const BPluginInterface::PersonInfoList &infos);
    void setLicense(const QString &text);
    void setLicense(const QString &fileName, const char *codecName);
    void resetTabs();
protected:
    BAboutDialog(BAboutDialogPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BAboutDialog)
};

#endif // BABOUTDIALOG_H
