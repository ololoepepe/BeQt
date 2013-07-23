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
#include <BeQtCore/BPersonInfo>

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
#include <QSize>

#include <QDebug>

/*============================================================================
================================ BAboutDialogPrivate =========================
============================================================================*/

/*============================== Static public constants ===================*/

const QString BAboutDialogPrivate::HtmlSpace = "&nbsp;";
const QString BAboutDialogPrivate::HtmlSpaceDouble = BAboutDialogPrivate::HtmlSpace + BAboutDialogPrivate::HtmlSpace;
const QString BAboutDialogPrivate::HtmlSpaceQuadruple =
        BAboutDialogPrivate::HtmlSpaceDouble + BAboutDialogPrivate::HtmlSpaceDouble;
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
                            if (l.at(k) == ':' && k < l.length() - 1 && l.at(k + 1) != ':'
                                    && k > 0 && l.at(k -1) != ':')
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
    q->setMinimumSize(defMinSize);
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
    aboutBeqtDlg = new BAboutDialog( q_func(), "BeQt", bVersion() );
    aboutBeqtDlg->setOrganization("Andrey Bogdanov", "2012-2013");
    aboutBeqtDlg->setWebsite("https://github.com/the-dark-angel/BeQt");
    aboutBeqtDlg->setPixmap( BApplication::beqtPixmap("beqt_logo") );
    aboutBeqtDlg->setAuthorsProvider(BApplication::ds_func()->beqtAuthors);
    aboutBeqtDlg->setTranslatorsProvider(BApplication::ds_func()->beqtTranslations);
    aboutBeqtDlg->setThanksToProvider(BApplication::ds_func()->beqtThanksTo);
    retranslateAboutBeqtDialog();
}

void BAboutDialogPrivate::retranslateAboutBeqtDialog()
{
    if (!aboutBeqtDlg)
        return;
    aboutBeqtDlg->setWindowTitle( tr("About BeQt", "adlg windowTitle") );
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
        tab->setLineWrapMode(QTextBrowser::NoWrap);
        tab->setOpenExternalLinks(true);
        twgt->insertTab( tabIndex(t), tab, tabTitle(t) );
    }
    tab->clear();
    if (html)
        tab->setHtml(text);
    else
        tab->setPlainText(text);
}

void BAboutDialogPrivate::fillTab(DialogTab t, const BPersonInfoList &infos)
{
    QString s;
    for (int i = 0; i < infos.size(); ++i)
    {
        const BPersonInfo &inf = infos.at(i);
        if ( inf.name.isEmpty() || inf.role.isEmpty() )
            continue;
        QString sp = "";
        QString dsp = HtmlSpaceDouble;
        s += "<div>";
        QString img = BDirTools::findResource(inf.image);
        QPixmap pm(img);
        if (pm.isNull())
        {
            img = BDirTools::findResource("beqt/pixmaps/no_image.png");
            pm.load(img);
        }
        if (!pm.isNull())
        {
            double k = 80.0 / (double) qMax(pm.height(), pm.width());
            QString h = QString::number((int) (k * (double) pm.height()));
            QString w = QString::number((int) (k * (double) pm.width()));
            s += "<img height=" + h + " width=" + w + " style=\"float: left\" src=\"" + img + "\""
                    + (inf.image.isEmpty() ? (" title=\"" + tr("No image available") + "\"") : QString("")) + ">";
            sp = HtmlSpaceDouble;
            dsp = HtmlSpaceQuadruple;
        }
        s += sp + "<b>" + inf.name + "</b><br>" + dsp + inf.role;
        if ( !inf.site.isEmpty() )
            s += "<br>" + dsp + "<i>" + tr("Website", "personInfo text") +
                 "</i>: <a href = \"" + inf.site + "\">" + inf.site + "</a>";
        if ( !inf.mail.isEmpty() )
            s += "<br>" + dsp + "<i>" + tr("E-mail", "personInfo text") +
                 "</i>: <a href=\"mailto:" + inf.mail + "\">" + inf.mail + "</a>";
        s += "</div>";
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
    QString s = !fn.isEmpty() ? BDirTools::readTextFile(fn, "UTF-8") : description;
    fillTab(DescriptionTab, s, Qt::mightBeRichText(s));
}

void BAboutDialogPrivate::resetChangeLog()
{
    QString fn = BDirTools::localeBasedFileName(changeLogFileName);
    fillTab(ChangeLogTab, processChangeLog(!fn.isEmpty() ? BDirTools::readTextFile(fn, "UTF-8") : changeLog), true);
}

void BAboutDialogPrivate::resetLicense()
{
    QString fn = BDirTools::localeBasedFileName(licenseFileName);
    QString s = !fn.isEmpty() ? BDirTools::readTextFile(fn, "UTF-8") : license;
    fillTab(LicenseTab, s, Qt::mightBeRichText(s));
}

void BAboutDialogPrivate::setupFromApplicationData()
{
    B_Q(BAboutDialog);
    BCoreApplicationPrivate *dd = BApplication::ds_func();
    if (!dd)
        return;
    q->setOrganization(QApplication::organizationName(), dd->appCopyrightYears);
    q->setWebsite(QApplication::organizationDomain());
    if (!dd->appDescription.isEmpty())
        q->setDescription(dd->appDescription);
    else
        q->setDescriptionFile(dd->appDescriptionFileName);
    if (!dd->appChangeLog.isEmpty())
        q->setDescription(dd->appChangeLog);
    else
        q->setChangeLogFile(dd->appChangeLogFileName);
    if (!dd->appLicense.isEmpty())
        q->setLicense(dd->appLicense);
    else
        q->setLicenseFile(dd->appLicenseFileName);
    if (!dd->appAuthorsList.isEmpty())
        q->setAuthors(dd->appAuthorsList);
    else
        q->setAuthorsProvider(dd->appAuthors);
    if (!dd->appTranslationsList.isEmpty())
        q->setTranslators(dd->appTranslationsList);
    else
        q->setTranslatorsProvider(dd->appTranslations);
    if (!dd->appThanksToList.isEmpty())
        q->setThanksTo(dd->appThanksToList);
    else
        q->setThanksToProvider(dd->appThanksTo);
    q->setPixmap(QApplication::windowIcon().pixmap(100000)); //Using such an extent to get the largest pixmap
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
    fillTab(AuthorsTab, authorsProvider ? authorsProvider->infos() : BPersonInfoList());
}

void BAboutDialogPrivate::resetTranslations()
{
    fillTab(TranslatorsTab, translationsProvider ? translationsProvider->infos(true) : BPersonInfoList());
}

void BAboutDialogPrivate::resetThanksTo()
{
    fillTab(ThanksToTab, thanksToProvider ? thanksToProvider->infos() : BPersonInfoList());
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
        aboutBeqtDlg->exec();
    }
}

/*============================== Static public variables ===================*/

QSize BAboutDialogPrivate::defMinSize = QSize(700, 500);

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

/*============================== Static public methods =====================*/

void BAboutDialog::setDefaultMinimumSize(const QSize &sz)
{
    BAboutDialogPrivate::defMinSize = sz;
}

void BAboutDialog::setDefaultMinimumSize(int width, int height)
{
    setDefaultMinimumSize( QSize(width, height) );
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

void BAboutDialog::setAuthors(const BPersonInfoList &list)
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

void BAboutDialog::setTranslators(const BPersonInfoList &list)
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

void BAboutDialog::setThanksTo(const BPersonInfoList &list)
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

void BAboutDialog::setupWithApplicationData()
{
    d_func()->setupFromApplicationData();
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
