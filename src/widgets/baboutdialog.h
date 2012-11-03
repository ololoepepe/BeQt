#ifndef BABOUTDIALOG_H
#define BABOUTDIALOG_H

class BAboutDialogPrivate;

class QWidget;
class QIcon;

#include <BeQtCore/BeQt>

#include <QDialog>
#include <QList>
#include <QString>
#include <QCoreApplication>

class B_WIDGETS_EXPORT BAboutDialog : public QDialog
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BAboutDialog)
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
    explicit BAboutDialog(QWidget *parent = 0);
    explicit BAboutDialog(const AboutOptions &options, QWidget *parent = 0);
    ~BAboutDialog();
    //
    void setIcon(const QString &fileName);
    void setAbout(const QString &description, const QString &copyright, const QString &website);
    void setChangeLog(const QString &text);
    void setChangeLog(const QString &fileName, const char *codecName);
    void setAuthorsInfos(const PersonInfoList &infos);
    void setTranslationInfos(const PersonInfoList &infos);
    void setThanksToInfos(const PersonInfoList &infos);
    void setLicense(const QString &text);
    void setLicense(const QString &fileName, const char *codecName);
    void resetTabs();
protected:
    BAboutDialog(BAboutDialogPrivate &d);
    //
    BAboutDialogPrivate *_m_d;
private:
    Q_DISABLE_COPY(BAboutDialog)
};

#endif // BABOUTDIALOG_H
