#ifndef BABOUTDIALOG_P_H
#define BABOUTDIALOG_P_H

class BAboutDialogPrivate;

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

#include <QObject>
#include <QList>
#include <QMap>

class B_WIDGETS_EXPORT BAboutDialogPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BAboutDialogPrivateObject(BAboutDialogPrivate *p);
    ~BAboutDialogPrivateObject();
    //
    BAboutDialogPrivate *const _m_p;
public slots:
    void languageChanged();
private:
    Q_DISABLE_COPY(BAboutDialogPrivateObject)
};

class B_WIDGETS_EXPORT BAboutDialogPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BAboutDialog)
    Q_DECLARE_TR_FUNCTIONS(BAboutDialog)
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
    static const QString HtmlSpace;
    static const QString HtmlSpaceDouble;
    static const QString HtmlLT;
    static const QString HtmlGT;
    //
    BAboutDialogPrivate(BAboutDialog *q, const BAboutDialog::AboutOptions &options);
    ~BAboutDialogPrivate();
    //
    QString tabTitle(DialogTab t) const;
    int tabIndex(DialogTab t) const;
    void removeTab(DialogTab t);
    void fillTab(DialogTab t, const QString &text, bool html);
    void fillTab(DialogTab t, const BAboutDialog::PersonInfoList &infos);
    void retranslateUi();
    //
    BAboutDialogPrivateObject *const _m_o;
    //
    QString appName;
    QMap<DialogTab, QTextBrowser *> tbrsrs;
    BAboutDialog *aboutBeqtDlg;
    QVBoxLayout *vlt;
      QHBoxLayout *hltHeader;
        QLabel *lblIcon;
        QLabel *lblText;
        //stretch
        QToolButton *tbtnAboutQt;
        QToolButton *tbtnAboutBeQt;
      QTabWidget *twgt;
        //text browsers
      QHBoxLayout *hltActions;
        QLabel *lblCopyright;
        QLabel *lblWebsite;
        //stretch
        QDialogButtonBox *dlgbbox;
          //Close
private:
    Q_DISABLE_COPY(BAboutDialogPrivate)
    //
    friend class BAboutDialogPrivateObject;
};

#endif // BABOUTDIALOG_P_H
