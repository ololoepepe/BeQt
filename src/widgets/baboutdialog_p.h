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
================================ About Dialog Private
============================================================================*/

class B_WIDGETS_EXPORT BAboutDialogPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BAboutDialog)
    Q_OBJECT
public:
    enum DialogTab
    {
        AboutTab,
        ChangeLogTab,
        AuthorsTab,
        TranslatorsTab,
        ThanksToTab,
        LicenseTab
    };
    //
    static BAboutDialog::AboutOptions createAboutOptions();
    //
    BAboutDialogPrivate(BAboutDialog *q, const BAboutDialog::AboutOptions &options);
    ~BAboutDialogPrivate();
    //
    void init();
    QString tabTitle(DialogTab t) const;
    int tabIndex(DialogTab t) const;
    void removeTab(DialogTab t);
    void fillTab(DialogTab t, const QString &text, bool html);
    void fillTab(DialogTab t, const BPersonInfoProvider::PersonInfoList &infos);
    //
    //
    static const QString HtmlSpace;
    static const QString HtmlSpaceDouble;
    static const QString HtmlLT;
    static const QString HtmlGT;
    //
    const BAboutDialog::AboutOptions Options;
    //
    QString appName;
    BPersonInfoProvider *beqtAuthors;
    BPersonInfoProvider *beqtTranslations;
    BPersonInfoProvider *beqtThanksTo;
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
        QToolButton *tbtnAboutBeQt;
      QTabWidget *twgt;
        //text browsers
      QDialogButtonBox *dlgbbox;
        //Close
public slots:
   void retranslateUi();
   void resetAuthors();
   void resetTranslations();
   void resetThanksTo();
private:
    Q_DISABLE_COPY(BAboutDialogPrivate)
};

#endif // BABOUTDIALOG_P_H
