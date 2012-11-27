#ifndef BABOUTDIALOG_P_H
#define BABOUTDIALOG_P_H

class BAboutDialogPrivate;
class BPersonInfoProvider;

class QString;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QTabWidget;
class QTextBrowser;
class QToolButton;
class QDialogButtonBox;

#include "baboutdialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BPersonInfoProvider>

#include <QObject>
#include <QList>
#include <QMap>

/*============================================================================
================================ BAboutDialogPrivate =========================
============================================================================*/

class B_WIDGETS_EXPORT BAboutDialogPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BAboutDialog)
public:
    enum DialogTab
    {
        DescriptionTab,
        ChangeLogTab,
        AuthorsTab,
        TranslatorsTab,
        ThanksToTab,
        LicenseTab
    };
public:
    struct SourceInfo
    {
        QString fileName;
        QString defaultFileName;
        QString possibleSuffix;
    };
public:
    static const QString HtmlSpace;
    static const QString HtmlSpaceDouble;
    static const QString HtmlLT;
    static const QString HtmlGT;
public:
    explicit BAboutDialogPrivate(BAboutDialog *q);
    ~BAboutDialogPrivate();
public:
    static QString processChangeLog(const QString &text);
    static QString sourceFileName(const SourceInfo &src);
public:
    void init();
    void initAboutBeqtDialog();
    void retranslateAboutBeqtDialog();
    void updateWindowTitle();
    void updateCopyright();
    void updateWebsite();
    QString tabTitle(DialogTab t) const;
    int tabIndex(DialogTab t) const;
    void removeTab(DialogTab t);
    void fillTab(DialogTab t, const QString &text, bool html);
    void fillTab(DialogTab t, const BPersonInfoProvider::PersonInfoList &infos);
    void resetAuthorsProvider(BPersonInfoProvider *prov = 0);
    void resetTranslationProvider(BPersonInfoProvider *prov = 0);
    void resetThanksToProvider(BPersonInfoProvider *prov = 0);
    void resetDescriptionSource( const QString &fileName = QString(), const QString &defaultFileName = QString(),
                                 const QString &possibleSuffix = QString() );
    void resetChangeLogSource( const QString &fileName = QString(), const QString &defaultFileName = QString(),
                               const QString &possibleSuffix = QString() );
    void resetLicenseSource( const QString &fileName = QString(), const QString &defaultFileName = QString(),
                             const QString &possibleSuffix = QString() );
    void resetDescription();
    void resetChangeLog();
    void resetLicense();
public slots:
   void retranslateUi();
   void resetAuthors();
   void resetTranslations();
   void resetThanksTo();
   void tbtnAboutBeqtClicked();
public:
    QString appName;
    QString appVersion;
    QString organization;
    QString copyrightPeriod;
    QString website;
    SourceInfo descriptionSource;
    SourceInfo changeLogSource;
    SourceInfo licenseSource;
    BPersonInfoProvider *authorsProvider;
    BPersonInfoProvider *translationsProvider;
    BPersonInfoProvider *thanksToProvider;
    QMap<DialogTab, QTextBrowser *> tbrsrs;
    BAboutDialog *aboutBeqtDlg;
    QVBoxLayout *vlt;
      QHBoxLayout *hltHeader;
        QLabel *lblIcon;
        QVBoxLayout *vltHeader;
          QLabel *lblName;
          QHBoxLayout *hltCRWebsite;
            QLabel *lblCopyright;
            QLabel *lblWebsite;
        //stretch
        QToolButton *tbtnAboutQt;
        QToolButton *tbtnAboutBeqt;
      QTabWidget *twgt;
        //text browsers
      QDialogButtonBox *dlgbbox;
        //Close
private:
    Q_DISABLE_COPY(BAboutDialogPrivate)
};

#endif // BABOUTDIALOG_P_H
