class QWidget;

#include "baboutdialog.h"
#include "baboutdialog_p.h"
#include "bapplication.h"
#include "bapplication_p.h"

#include <BeQtCore/BDirTools>
#include <BeQtCore/BPluginInterface>
#include <BeQtCore/BCoreApplication>
#include <BeQtCore/BPersonInfoProvider>

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QTextBrowser>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QApplication>
#include <QPixmap>
#include <QFont>
#include <QSizePolicy>
#include <QStringList>
#include <QList>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QIcon>
#include <QUrl>
#include <QToolButton>
#include <QTextOption>
#include <QDialogButtonBox>

#include <QDebug>

/*============================================================================
================================ About Dialog Private
============================================================================*/

BAboutDialogPrivate::BAboutDialogPrivate(BAboutDialog *q) :
    BBasePrivate(q)
{
    //
}

BAboutDialogPrivate::~BAboutDialogPrivate()
{
    //
}

//

void BAboutDialogPrivate::init()
{
    authorsProvider = 0;
    translationsProvider = 0;
    thanksToProvider = 0;
    aboutBeqtDlg = 0;
    //
    B_Q(BAboutDialog);
    q->setMinimumHeight(400);
    q->setMinimumWidth(600);
    vlt = new QVBoxLayout(q);
      hltHeader = new QHBoxLayout;
        lblIcon = new QLabel(q);
          lblIcon->setVisible(false);
        hltHeader->addWidget(lblIcon);
        vltHeader = new QVBoxLayout;
          lblName = new QLabel(q);
            QFont font;
            font.setPointSize(12);
            font.setBold(true);
            lblName->setFont(font);
            //lblName->setText(appName + " v" + appVersion);
          vltHeader->addWidget(lblName);
          hltCRWebsite = new QHBoxLayout;
            lblCopyright = new QLabel(q);
              lblCopyright->setTextFormat(Qt::RichText);
              lblCopyright->setVisible(false);
            hltCRWebsite->addWidget(lblCopyright);
            lblWebsite = new QLabel(q);
              lblWebsite->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
              lblWebsite->setTextFormat(Qt::RichText);
              lblWebsite->setOpenExternalLinks(true);
              lblWebsite->setVisible(false);
            hltCRWebsite->addWidget(lblWebsite);
          vltHeader->addLayout(hltCRWebsite);
        hltHeader->addLayout(vltHeader);
        hltHeader->addStretch();
        tbtnAboutQt = new QToolButton(q);
          tbtnAboutQt->setVisible(false);
          tbtnAboutQt->setIcon( QIcon( BApplication::beqtPixmap("qt_logo") ) );
          connect( tbtnAboutQt, SIGNAL( clicked() ), QApplication::instance(), SLOT( aboutQt() ) );
        hltHeader->addWidget(tbtnAboutQt);
        tbtnAboutBeqt = new QToolButton(q);
          tbtnAboutBeqt->setVisible(false);
          tbtnAboutBeqt->setIcon( QIcon( BApplication::beqtPixmap("beqt_logo") ) );
          connect( tbtnAboutBeqt, SIGNAL( clicked() ), this, SLOT( tbtnAboutBeqtClicked() ) );
        hltHeader->addWidget(tbtnAboutBeqt);
      vlt->addLayout(hltHeader);
      twgt = new QTabWidget(q);
      vlt->addWidget(twgt);
      dlgbbox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, q);
        dlgbbox->setFocus();
        QObject::connect( dlgbbox, SIGNAL( rejected() ), q, SLOT( reject() ) );
      vlt->addWidget(dlgbbox);
    //
    q->setAppName( QApplication::applicationName() );
    q->setAppVersion( QApplication::applicationVersion() );
    retranslateUi();
    connect( BCoreApplication::instance(), SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

void BAboutDialogPrivate::initAboutBeqtDialog()
{
    aboutBeqtDlg = new BAboutDialog( 0, "BeQt", bVersion() );
    aboutBeqtDlg->setWindowModality(Qt::NonModal);
    aboutBeqtDlg->setPixmap( BApplication::beqtPixmap("beqt_logo") );
    aboutBeqtDlg->setAuthors(BApplication::ds_func()->beqtAuthors);
    aboutBeqtDlg->setTranslation(BApplication::ds_func()->beqtTranslations);
    aboutBeqtDlg->setThanksTo(BApplication::ds_func()->beqtThanksTo);
    retranslateAboutBeqtDialog();
}

void BAboutDialogPrivate::retranslateAboutBeqtDialog()
{
    if (!aboutBeqtDlg)
        return;
    QString copyright = "2012 Andrey Bogdanov";
    QString website = "https://github.com/the-dark-angel/BeQt";
    aboutBeqtDlg->setAbout(BApplication::beqtInfo(BApplication::Description), copyright, website);
    aboutBeqtDlg->setChangeLog( BApplication::beqtInfo(BApplication::ChangeLog) );
    aboutBeqtDlg->setLicense( BApplication::beqtInfo(BApplication::License) );
}

void BAboutDialogPrivate::updateWindowTitle()
{
    q_func()->setWindowTitle(tr("About", "windowTitle") + " " + appName);
}

QString BAboutDialogPrivate::tabTitle(DialogTab t) const
{
    switch (t)
    {
    case AboutTab:
        return tr("About", "twgt tabText");
    case ChangeLogTab:
        return tr("Changelog", "twgt tabText");
    case AuthorsTab:
        return tr("Authors", "twgt tabText");
    case TranslatorsTab:
        return tr("Translation", "twgt tabText");
    case ThanksToTab:
        return tr("Thanks to", "twgt tabText");
    case LicenseTab:
        return tr("License", "twgt tabText");
    default:
        return "";
    }
}

int BAboutDialogPrivate::tabIndex(DialogTab t) const
{
    switch (t)
    {
    case AboutTab:
        return 0;
    case ChangeLogTab:
        return tbrsrs.contains(AboutTab) ? 1 : 0;
    case AuthorsTab:
        return tabIndex(ChangeLogTab) + (tbrsrs.contains(ChangeLogTab) ? 1 : 0);
    case TranslatorsTab:
        return tabIndex(AuthorsTab) + (tbrsrs.contains(AuthorsTab) ? 1 : 0);
    case ThanksToTab:
        return tabIndex(TranslatorsTab) + (tbrsrs.contains(TranslatorsTab) ? 1 : 0);
    case LicenseTab:
        return tabIndex(ThanksToTab) + (tbrsrs.contains(ThanksToTab) ? 1 : 0);
    default:
        return -1;
    }
}

void BAboutDialogPrivate::removeTab(DialogTab t)
{
    QTextBrowser *tab = tbrsrs.take(t);
    if (!tab)
        return;
    twgt->removeTab( twgt->indexOf(tab) );
    tab->deleteLater();
}

void BAboutDialogPrivate::fillTab(DialogTab t, const QString &text, bool html)
{
    if ( text.isEmpty() )
        return removeTab(t);
    QTextBrowser *&tab = tbrsrs[t];
    if (!tab)
    {
        tab = new QTextBrowser;
        tab->setOpenExternalLinks(true);
        twgt->insertTab( tabIndex(t), tab, tabTitle(t) );
    }
    tab->clear();
    if (html)
        tab->setHtml(text);
    else
        tab->setPlainText(text);
}

void BAboutDialogPrivate::fillTab(DialogTab t, const BPersonInfoProvider::PersonInfoList &infos)
{
    QString s;
    for (int i = 0; i < infos.size(); ++i)
    {
        const BPersonInfoProvider::PersonInfo &inf = infos.at(i);
        if ( inf.name.isEmpty() || inf.role.isEmpty() )
            continue;
        s += "<b>" + inf.name + "</b><br>" + HtmlSpaceDouble + inf.role + "<br>";
        if ( !inf.site.isEmpty() )
            s += HtmlSpaceDouble + "<i>Website</i>: <a href = \"" + inf.site + "\">" + inf.site + "</a><br>";
        if ( !inf.mail.isEmpty() )
            s += HtmlSpaceDouble + "<i>E-mail</i>: <a href=\"mailto:" + inf.mail + "\">" + inf.mail + "</a><br>";
        if (i < infos.size() - 1)
            s += "<br>";
    }
    fillTab(t, s, true);
}

void BAboutDialogPrivate::resetAuthorsProvider(BPersonInfoProvider *prov)
{
    if (authorsProvider)
    {
        disconnect( authorsProvider, SIGNAL( reloaded() ), this, SLOT( resetAuthors() ) );
        if (!authorsProvider->parent() || authorsProvider->parent() == (QObject *) this)
            authorsProvider->deleteLater();
    }
    authorsProvider = prov;
    if (authorsProvider)
    {
        if ( !authorsProvider->parent() )
            authorsProvider->setParent(this);
        connect( authorsProvider, SIGNAL( reloaded() ), this, SLOT( resetAuthors() ) );
    }
    resetAuthors();
}

void BAboutDialogPrivate::resetTranslationProvider(BPersonInfoProvider *prov)
{
    if (translationsProvider)
    {
        disconnect( translationsProvider, SIGNAL( reloaded() ), this, SLOT( resetTranslations() ) );
        if (!translationsProvider->parent() || translationsProvider->parent() == (QObject *) this)
            translationsProvider->deleteLater();
    }
    translationsProvider = prov;
    if (translationsProvider)
    {
        if ( !translationsProvider->parent() )
            translationsProvider->setParent(this);
        connect( translationsProvider, SIGNAL( reloaded() ), this, SLOT( resetTranslations() ) );
    }
    resetTranslations();
}

void BAboutDialogPrivate::resetThanksToProvider(BPersonInfoProvider *prov)
{
    if (thanksToProvider)
    {
        disconnect( thanksToProvider, SIGNAL( reloaded() ), this, SLOT( resetThanksTo() ) );
        if (!thanksToProvider->parent() || thanksToProvider->parent() == (QObject *) this)
            thanksToProvider->deleteLater();
    }
    thanksToProvider = prov;
    if (thanksToProvider)
    {
        if ( !thanksToProvider->parent() )
            thanksToProvider->setParent(this);
        connect( thanksToProvider, SIGNAL( reloaded() ), this, SLOT( resetThanksTo() ) );
    }
    resetThanksTo();
}

//

const QString BAboutDialogPrivate::HtmlSpace = "&nbsp;";
const QString BAboutDialogPrivate::HtmlSpaceDouble = BAboutDialogPrivate::HtmlSpace + BAboutDialogPrivate::HtmlSpace;
const QString BAboutDialogPrivate::HtmlLT = "&lt;";
const QString BAboutDialogPrivate::HtmlGT = "&gt;";

//

void BAboutDialogPrivate::retranslateUi()
{
    updateWindowTitle();
    foreach ( DialogTab t, tbrsrs.keys() )
        twgt->setTabText( tabIndex(t), tabTitle(t) );
    resetAuthors();
    resetTranslations();
    resetThanksTo();
    tbtnAboutQt->setToolTip( tr("About Qt", "tbtn toolTip") );
    tbtnAboutBeqt->setToolTip( tr("About BeQt", "tbtn toolTip") );
    retranslateAboutBeqtDialog();
}

void BAboutDialogPrivate::resetAuthors()
{
    fillTab( AuthorsTab, authorsProvider ? authorsProvider->infos() : BPersonInfoProvider::PersonInfoList() );
}

void BAboutDialogPrivate::resetTranslations()
{
    fillTab( TranslatorsTab, translationsProvider ? translationsProvider->infos() :
                                                    BPersonInfoProvider::PersonInfoList() );
}

void BAboutDialogPrivate::resetThanksTo()
{
    fillTab( ThanksToTab, thanksToProvider ? thanksToProvider->infos() : BPersonInfoProvider::PersonInfoList() );
}

void BAboutDialogPrivate::tbtnAboutBeqtClicked()
{
    if (!aboutBeqtDlg)
        initAboutBeqtDialog();
    aboutBeqtDlg->open();
}

/*============================================================================
================================ About Dialog
============================================================================*/

BAboutDialog::BAboutDialog(QWidget *parent) :
    QDialog(parent), BBase( *new BAboutDialogPrivate(this) )
{
    d_func()->init();
}

BAboutDialog::BAboutDialog(QWidget *parent, const QString &appName, const QString &appVersion) :
    QDialog(parent), BBase( *new BAboutDialogPrivate(this) )
{
    d_func()->init();
    setAppName(appName);
    setAppVersion(appVersion);
}

BAboutDialog::~BAboutDialog()
{
    //
}

//

void BAboutDialog::setAppName(const QString &name)
{
    if ( name.isEmpty() )
        return;
    B_D(BAboutDialog);
    d->appName = name;
    d->updateWindowTitle();
    d->lblName->setText(d->appName + " v" + d->appVersion);
}

void BAboutDialog::setAppVersion(const QString &version)
{
    if ( version.isEmpty() )
        return;
    B_D(BAboutDialog);
    d->appVersion = version;
    d->lblName->setText(d->appName + " v" + d->appVersion);
}

void BAboutDialog::setPixmap(const QPixmap &pixmap)
{
    B_D(BAboutDialog);
    d->lblIcon->setPixmap( pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation) );
    d->lblIcon->setVisible( !pixmap.isNull() );
}

void BAboutDialog::setPixmap(const QString &fileName)
{
    setPixmap( QPixmap(fileName) );
}

void BAboutDialog::setAbout(const QString &description, const QString &copyright, const QString &website)
{
    B_D(BAboutDialog);
    bool b = !description.isEmpty() && !copyright.isEmpty();
    d->lblCopyright->setVisible(b);
    d->lblWebsite->setVisible(b);
    if (!b)
        return;
    d->fillTab(BAboutDialogPrivate::AboutTab, description, false);
    d->lblCopyright->setText(tr("Copyright", "about") + " &copy; " + copyright);
    QString s = !website.isEmpty() ? ("<a href=\"" + website + "\">[" + tr("Website", "lbl text") + "]</a>") : "";
    d->lblWebsite->setText(s);
    d->lblWebsite->setToolTip(website);
}

void BAboutDialog::setChangeLog(const QString &text)
{
    QString s;
    QStringList sl = text.split('\n');
    for (int i = 0; i < sl.size(); ++i)
    {
        QString l = sl.at(i);
        int len = l.length();
        l.replace('<', BAboutDialogPrivate::HtmlLT);
        l.replace('>', BAboutDialogPrivate::HtmlGT);
        if (len > 1)
        {
            if (l.at(0) == '=' && l.at(len - 1) == '=')
            {
                l.prepend("<font color='dark red'>").append("</font>");
            }
            else
            {
                for (int j = 0; j < len; ++j)
                {
                    if (l.at(j) == '*')
                    {
                        l.insert(j + 1, "</font><font color=blue>");
                        l.insert(j, "<font color='dark green'>");
                        bool b = false;
                        for (int k = j + 1; k < l.length(); ++k)
                        {
                            if (l.at(k) == ':')
                            {
                                l.insert(k + 1, "</font>");
                                b = true;
                                break;
                            }
                        }
                        if (!b)
                            l.append("</font>");
                        QString r;
                        for (int k = 0; k < j; ++k)
                            r += BAboutDialogPrivate::HtmlSpace;
                        l.replace(0, j, r);
                        break;
                    }
                    else if (l.at(j) != ' ' && l.at(j) != '\t')
                    {
                        break;
                    }
                }
            }
        }
        s += l;
        if (i < sl.size() - 1)
            s += "<br>";
    }
    d_func()->fillTab(BAboutDialogPrivate::ChangeLogTab, s, true);
}

void BAboutDialog::setChangeLog(const QString &fileName, const char *codecName)
{
    setChangeLog( BDirTools::readTextFile(fileName, codecName) );
}

void BAboutDialog::setAuthors(BPersonInfoProvider *prov)
{
    d_func()->resetAuthorsProvider(prov);
}

void BAboutDialog::setAuthorsInfos(const BPersonInfoProvider::PersonInfoList &list)
{
    d_func()->resetAuthorsProvider();
    d_func()->fillTab(BAboutDialogPrivate::AuthorsTab, list);
}

void BAboutDialog::setTranslation(BPersonInfoProvider *prov)
{
    d_func()->resetTranslationProvider(prov);
}

void BAboutDialog::setTranslationInfos(const BPersonInfoProvider::PersonInfoList &list)
{
    d_func()->resetTranslationProvider();
    d_func()->fillTab(BAboutDialogPrivate::TranslatorsTab, list);
}

void BAboutDialog::setThanksTo(BPersonInfoProvider *prov)
{
    d_func()->resetThanksToProvider(prov);
}

void BAboutDialog::setThanksToInfos(const BPersonInfoProvider::PersonInfoList &list)
{
    d_func()->resetThanksToProvider();
    d_func()->fillTab(BAboutDialogPrivate::ThanksToTab, list);
}

void BAboutDialog::setLicense(const QString &text)
{
    B_D(BAboutDialog);
    if ( text.isEmpty() )
        return d->removeTab(BAboutDialogPrivate::LicenseTab);
    QTextBrowser *&tab = d->tbrsrs[BAboutDialogPrivate::LicenseTab];
    if (!tab)
    {
        tab = new QTextBrowser;
        tab->setOpenExternalLinks(true);
        d->twgt->addTab( tab, d->tabTitle(BAboutDialogPrivate::LicenseTab) );
    }
    tab->clear();
    tab->setPlainText(text);
}

void BAboutDialog::setLicense(const QString &fileName, const char *codecName)
{
    setLicense( BDirTools::readTextFile( fileName, codecName) );
}

void BAboutDialog::setAboutQtShown(bool b)
{
    d_func()->tbtnAboutQt->setVisible(b);
}

void BAboutDialog::setAboutBeqtShown(bool b)
{
    d_func()->tbtnAboutBeqt->setVisible(b);
}

void BAboutDialog::resetTabs()
{
    QTabWidget *twgt = d_func()->twgt;
    if ( !twgt->count() )
        return;
    twgt->setCurrentIndex(0);
    twgt->widget(0)->setFocus();
}

//

BAboutDialog::BAboutDialog(BAboutDialogPrivate &d, QWidget *parent) :
    QDialog(parent), BBase(d)
{
    d_func()->init();
}
