class QWidget;

#include "baboutdialog.h"
#include "baboutdialog_p.h"
#include "bapplication.h"
#include "bapplication_p.h"

#include <BeQtCore/BDirTools>
#include <BeQtCore/BPluginInterface>
#include <BeQtCore/BCoreApplication>
#include <BeQtCore/BPersonInfoProvider>
#include <BeQtCore/BDirTools>

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
================================ BAboutDialogPrivate =========================
============================================================================*/

/*============================== Static public constants ===================*/

const QString BAboutDialogPrivate::HtmlSpace = "&nbsp;";
const QString BAboutDialogPrivate::HtmlSpaceDouble = BAboutDialogPrivate::HtmlSpace + BAboutDialogPrivate::HtmlSpace;
const QString BAboutDialogPrivate::HtmlLT = "&lt;";
const QString BAboutDialogPrivate::HtmlGT = "&gt;";

/*============================== Public constructors =======================*/

BAboutDialogPrivate::BAboutDialogPrivate(BAboutDialog *q) :
    BBasePrivate(q)
{
    //
}

BAboutDialogPrivate::~BAboutDialogPrivate()
{
    //
}

/*============================== Static public methods =====================*/

QString BAboutDialogPrivate::processChangeLog(const QString &text)
{
    QString s;
    QStringList sl = text.split('\n');
    for (int i = 0; i < sl.size(); ++i)
    {
        QString l = sl.at(i);
        int len = l.length();
        l.replace('<', HtmlLT);
        l.replace('>', HtmlGT);
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
                            r += HtmlSpace;
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
    return s;
}

/*============================== Public methods ============================*/

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
    q->setOrganization( QApplication::organizationName() );
    q->setWebsite( QApplication::organizationDomain() );
    retranslateUi();
    connect( BCoreApplication::instance(), SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

void BAboutDialogPrivate::initAboutBeqtDialog()
{
    aboutBeqtDlg = new BAboutDialog( 0, "BeQt", bVersion() );
    aboutBeqtDlg->setWindowModality(Qt::NonModal);
    aboutBeqtDlg->setOrganization("Andrey Bogdanov", "2012");
    aboutBeqtDlg->setWebsite("https://github.com/the-dark-angel/BeQt");
    aboutBeqtDlg->setPixmap( BApplication::beqtPixmap("beqt_logo") );
    QString loc = BApplication::location(BApplication::BeqtPath, BApplication::SharedResources) + "/";
    aboutBeqtDlg->setDescriptionFile(loc + "description/DESCRIPTION.txt");
    aboutBeqtDlg->setChangeLogFile(loc + "changelog/ChangeLog.txt");
    aboutBeqtDlg->setLicenseFile(loc + "copying/COPYING.txt");
    aboutBeqtDlg->setAuthorsProvider(BApplication::ds_func()->beqtAuthors);
    aboutBeqtDlg->setTranslatorsProvider(BApplication::ds_func()->beqtTranslations);
    aboutBeqtDlg->setThanksToProvider(BApplication::ds_func()->beqtThanksTo);
    retranslateAboutBeqtDialog();
}

void BAboutDialogPrivate::retranslateAboutBeqtDialog()
{
    if (!aboutBeqtDlg)
        return;
    aboutBeqtDlg->setDescription( BApplication::beqtInfo(BApplication::Description) );
    aboutBeqtDlg->setChangeLog( BApplication::beqtInfo(BApplication::ChangeLog) );
    aboutBeqtDlg->setLicense( BApplication::beqtInfo(BApplication::License) );
}

void BAboutDialogPrivate::updateWindowTitle()
{
    q_func()->setWindowTitle(tr("About", "windowTitle") + " " + appName);
}

void BAboutDialogPrivate::updateCopyright()
{
    QString cr = !copyrightPeriod.isEmpty() ? (copyrightPeriod + " ") : QString();
    lblCopyright->setText(tr("Copyright", "lbl text") + " &copy; " + cr + organization);
}

void BAboutDialogPrivate::updateWebsite()
{
    lblWebsite->setText("<a href=\"" + website + "\">[" + tr("Website", "lbl text") + "]</a>");
    lblWebsite->setToolTip(website);
}

QString BAboutDialogPrivate::tabTitle(DialogTab t) const
{
    switch (t)
    {
    case DescriptionTab:
        return tr("Description", "twgt tabText");
    case ChangeLogTab:
        return tr("Change log", "twgt tabText");
    case AuthorsTab:
        return tr("Authors", "twgt tabText");
    case TranslatorsTab:
        return tr("Translators", "twgt tabText");
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
    case DescriptionTab:
        return 0;
    case ChangeLogTab:
        return tbrsrs.contains(DescriptionTab) ? 1 : 0;
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

void BAboutDialogPrivate::resetDescriptionFile(const QString &fileName)
{
    descriptionFileName = fileName;
    if ( !fileName.isEmpty() )
        description.clear();
    resetDescription();
}

void BAboutDialogPrivate::resetChangeLogFile(const QString &fileName)
{
    changeLogFileName = fileName;
    if ( !fileName.isEmpty() )
        changeLog.clear();
    resetChangeLog();
}

void BAboutDialogPrivate::resetLicenseFile(const QString &fileName)
{
    licenseFileName = fileName;
    if ( !fileName.isEmpty() )
        license.clear();
    resetLicense();
}

void BAboutDialogPrivate::resetDescription()
{
    QString fn = BDirTools::localeBasedFileName(descriptionFileName);
    fillTab(DescriptionTab, !fn.isEmpty() ? BDirTools::readTextFile(fn, "UTF-8") : description, false);
}

void BAboutDialogPrivate::resetChangeLog()
{
    QString fn = BDirTools::localeBasedFileName(changeLogFileName);
    fillTab(ChangeLogTab, processChangeLog(!fn.isEmpty() ? BDirTools::readTextFile(fn, "UTF-8") : changeLog), true);
}

void BAboutDialogPrivate::resetLicense()
{
    QString fn = BDirTools::localeBasedFileName(licenseFileName);
    fillTab(LicenseTab, !fn.isEmpty() ? BDirTools::readTextFile(fn, "UTF-8") : license, false);
}

/*============================== Public slots ==============================*/

void BAboutDialogPrivate::retranslateUi()
{
    updateWindowTitle();
    updateCopyright();
    updateWebsite();
    foreach ( DialogTab t, tbrsrs.keys() )
        twgt->setTabText( tabIndex(t), tabTitle(t) );
    resetAuthors();
    resetTranslations();
    resetThanksTo();
    resetDescription();
    resetChangeLog();
    resetLicense();
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
    if ( aboutBeqtDlg->isVisible() )
    {
        aboutBeqtDlg->activateWindow();
    }
    else
    {
        aboutBeqtDlg->resetTabs();
        aboutBeqtDlg->open();
    }
}

/*============================================================================
================================ BAboutDialog ================================
============================================================================*/

/*============================== Public constructors =======================*/

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

/*============================== Protected constructors ====================*/

BAboutDialog::BAboutDialog(BAboutDialogPrivate &d, QWidget *parent) :
    QDialog(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

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

void BAboutDialog::setOrganization(const QString &organization, const QString &copyrightPeriod)
{
    if ( organization.isEmpty() )
        return;
    B_D(BAboutDialog);
    d->organization = organization;
    d->copyrightPeriod = copyrightPeriod;
    d->lblCopyright->setVisible(true);
    d->updateCopyright();
}

void BAboutDialog::setWebsite(const QString &site)
{
    if ( site.isEmpty() )
        return;
    B_D(BAboutDialog);
    d->website = site;
    d->lblWebsite->setVisible(true);
    d->updateWebsite();

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

void BAboutDialog::setDescription(const QString &text)
{
    B_D(BAboutDialog);
    d->description = text;
    d->resetDescriptionFile();
}

void BAboutDialog::setDescriptionFile(const QString &fileName)
{
    d_func()->resetDescriptionFile(fileName);
}

void BAboutDialog::setChangeLog(const QString &text)
{
    B_D(BAboutDialog);
    d->changeLog = text;
    d->resetChangeLogFile();
}

void BAboutDialog::setChangeLogFile(const QString &fileName)
{
    d_func()->resetChangeLogFile(fileName);
}

void BAboutDialog::setLicense(const QString &text)
{
    B_D(BAboutDialog);
    d->license = text;
    d->resetLicenseFile();
}

void BAboutDialog::setLicenseFile(const QString &fileName)
{
    d_func()->resetLicenseFile(fileName);
}

void BAboutDialog::setAuthors(const BPersonInfoProvider::PersonInfoList &list)
{
    d_func()->resetAuthorsProvider();
    d_func()->fillTab(BAboutDialogPrivate::AuthorsTab, list);
}

void BAboutDialog::setAuthorsFile(const QString &fileName)
{
    setAuthorsProvider( new BPersonInfoProvider( fileName, d_func() ) );
}

void BAboutDialog::setAuthorsProvider(BPersonInfoProvider *prov)
{
    d_func()->resetAuthorsProvider(prov);
}

void BAboutDialog::setTranslators(const BPersonInfoProvider::PersonInfoList &list)
{
    d_func()->resetTranslationProvider();
    d_func()->fillTab(BAboutDialogPrivate::TranslatorsTab, list);
}

void BAboutDialog::setTranslatorsFile(const QString &fileName)
{
    setTranslatorsProvider( new BPersonInfoProvider( fileName, d_func() ) );
}

void BAboutDialog::setTranslatorsProvider(BPersonInfoProvider *prov)
{
    d_func()->resetTranslationProvider(prov);
}

void BAboutDialog::setThanksTo(const BPersonInfoProvider::PersonInfoList &list)
{
    d_func()->resetThanksToProvider();
    d_func()->fillTab(BAboutDialogPrivate::ThanksToTab, list);
}

void BAboutDialog::setThanksToFile(const QString &fileName)
{
    setThanksToProvider( new BPersonInfoProvider( fileName, d_func() ) );
}

void BAboutDialog::setThanksToProvider(BPersonInfoProvider *prov)
{
    d_func()->resetThanksToProvider(prov);
}

void BAboutDialog::setAboutQtShown(bool b)
{
    d_func()->tbtnAboutQt->setVisible(b);
}

void BAboutDialog::setAboutBeqtShown(bool b)
{
    d_func()->tbtnAboutBeqt->setVisible(b);
}

/*============================== Public slots ==============================*/

void BAboutDialog::resetTabs()
{
    QTabWidget *twgt = d_func()->twgt;
    if ( !twgt->count() )
        return;
    twgt->setCurrentIndex(0);
    twgt->widget(0)->setFocus();
}
