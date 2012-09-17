#ifndef BABOUTDIALOG_H
#define BABOUTDIALOG_H

class QString;
class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QTabWidget;
class QTextBrowser;
class QPushButton;

#include <QDialog>
#include <QList>

class BAboutDialog : public QDialog
{
    Q_OBJECT
public:
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
    //
    void setIcon(const QString &fileName);
    void setAboutText(const QString &text, bool html = false);
    void setChangeLog(const QString &text);
    void setChangeLog(const QString &fileName, const char *codecName);
    void setAuthorsInfos(const PersonInfoList &infos);
    void setTranslationInfos(const PersonInfoList &infos);
    void setThanksToInfos(const PersonInfoList &infos);
    void setLicense( const QString &text, const QString &iconFileName = QString() );
    void setLicense( const QString &fileName, const char *codecName, const QString &iconFileName = QString() );
    void resetTabs();
private:
    static const QString _m_HtmlSpace;
    static const QString _m_HtmlSpaceDouble;
    static const QString _m_HtmlLT;
    static const QString _m_HtmlGT;
    //
    QVBoxLayout *_m_vlt;
      QHBoxLayout *_m_hltHeader;
        QLabel *_m_lblIcon;
        QLabel *_m_lblText;
      QTabWidget *_m_tabWgt;
        QTextBrowser *_m_tbsrAbout;
        QTextBrowser *_m_tbsrChangeLog;
        QTextBrowser *_m_tbsrAuthors;
        QTextBrowser *_m_tbsrThanksTo;
        QTextBrowser *_m_tbsrTranslators;
        QTextBrowser *_m_tbsrLicense;
      QHBoxLayout *_m_hltActions;
        //stretch
        QPushButton *_m_btnClose;
    //
    QString _m_aboutTitle() const;
    QString _m_changeLogTitle() const;
    QString _m_authorsTitle() const;
    QString _m_thanksToTitle() const;
    QString _m_translatorsTitle() const;
    QString _m_licenseTitle() const;
    void _m_removeTbsr(QTextBrowser *&browser);
    QString _m_readFile(const QString &fileName, const char *codecName) const;
    void _m_fillTbsr(QTextBrowser *&browser, int index, const QString &title, const QString &text, bool html);
    void _m_fillTbsr(QTextBrowser *&browser, int index, const QString &title, const PersonInfoList &infos);
private slots:
    void _m_retranslateUi();
};

#endif // BABOUTDIALOG_H
