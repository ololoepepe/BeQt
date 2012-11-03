#ifndef BABOUTDIALOG_P_H
#define BABOUTDIALOG_P_H

class QString;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QTabWidget;
class QTextBrowser;
class QPushButton;
class QToolButton;

#include "baboutdialog.h"

#include <BeQtCore/BeQt>

#include <QObject>
#include <QList>
#include <QMap>

class BAboutDialogPrivate : public QObject
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BAboutDialog)
public:
    int a;
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
    BAboutDialogPrivate(BAboutDialog *q, const BAboutDialog::AboutOptions &options);
    ~BAboutDialogPrivate();
    //
    QString tabTitle(DialogTab t) const;
    int tabIndex(DialogTab t) const;
    void removeTab(DialogTab t);
    QString readFile(const QString &fileName, const char *codecName) const;
    void fillTab(DialogTab t, const QString &text, bool html);
    void fillTab(DialogTab t, const BAboutDialog::PersonInfoList &infos);
    //
    BAboutDialog *const _m_q;
    QMap<DialogTab, QTextBrowser *> tbrsrs;
    BAboutDialog *aboutBeqtDlg;
    QVBoxLayout *vlt;
      QHBoxLayout *hltHeader;
        QLabel *lblIcon;
        QLabel *lblText;
        QLabel *lblWebsite;
        //stretch
        QToolButton *tbtnAboutQt;
        QToolButton *tbtnAboutBeQt;
      QTabWidget *twgt;
        //text browsers
        QTextBrowser *tbsrAbout;
        QTextBrowser *tbsrChangeLog;
        QTextBrowser *tbsrAuthors;
        QTextBrowser *tbsrThanksTo;
        QTextBrowser *tbsrTranslators;
        QTextBrowser *tbsrLicense;
      QHBoxLayout *hltActions;
        QLabel *lblCopyright;
        //stretch
        QPushButton *btnClose;
private:
    static const QString HtmlSpace;
    static const QString HtmlSpaceDouble;
    static const QString HtmlLT;
    static const QString HtmlGT;
    //
    Q_DISABLE_COPY(BAboutDialogPrivate)
private slots:
    void retranslateUi();
    void tbtnAboutBeQtClicked();
};

#endif // BABOUTDIALOG_P_H
