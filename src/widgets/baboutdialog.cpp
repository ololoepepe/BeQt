class QWidget;

#include "baboutdialog.h"
#include "baboutdialog_p.h"
#include "bapplication.h"

#include <BeQtCore/BDirTools>
#include <BeQtCore/BPluginInterface>

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

const QString BAboutDialogPrivate::HtmlSpace = "&nbsp;";
const QString BAboutDialogPrivate::HtmlSpaceDouble = BAboutDialogPrivate::HtmlSpace + BAboutDialogPrivate::HtmlSpace;
const QString BAboutDialogPrivate::HtmlLT = "&lt;";
const QString BAboutDialogPrivate::HtmlGT = "&gt;";

//

BAboutDialog::PersonInfoList BAboutDialog::fromPluginPersonInfoList(const BPluginInterface::PersonInfoList &list)
{
    PersonInfoList newList;
    foreach (const BPluginInterface::PersonInfo &info, list)
    {
        PersonInfo newInfo;
        newInfo.name = info.name;
        newInfo.role = info.role;
        newInfo.site = info.site;
        newInfo.mail = info.mail;
        newList << newInfo;
    }
    return newList;
}

//

BAboutDialogPrivate::BAboutDialogPrivate(BAboutDialog *q, const BAboutDialog::AboutOptions &options) :
    BBasePrivate(q)
{
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
            appName = !options.appName.isEmpty() ? options.appName : QApplication::applicationName();
            QString appVersion = !options.appVersion.isEmpty() ? options.appVersion :
                                                                 QApplication::applicationVersion();
            lblName->setText(appName + " v" + appVersion);
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
        if (options.aboutQtButton)
        {
            tbtnAboutQt = new QToolButton(q);
              tbtnAboutQt->setIcon( QIcon( BApplication::beqtPixmap("qt_logo") ) );
              connect( tbtnAboutQt, SIGNAL( clicked() ), QApplication::instance(), SLOT( aboutQt() ) );
            hltHeader->addWidget(tbtnAboutQt);
        }
        else
        {
            tbtnAboutQt = 0;
        }
        if (options.aboutBeQtButton)
        {
            tbtnAboutBeQt = new QToolButton(q);
              tbtnAboutBeQt->setIcon( QIcon( BApplication::beqtPixmap("beqt_logo") ) );
            hltHeader->addWidget(tbtnAboutBeQt);
            //
            BAboutDialog::AboutOptions opt;
            opt.appName = "BeQt";
            opt.appVersion = bVersion();
            aboutBeqtDlg = new BAboutDialog( opt, q_func() );
            aboutBeqtDlg->setWindowModality(Qt::NonModal);
            aboutBeqtDlg->setPixmap( BApplication::beqtPixmap("beqt_logo") );
            connect( tbtnAboutBeQt, SIGNAL( clicked() ), aboutBeqtDlg, SLOT( open() ) );
        }
        else
        {
            tbtnAboutBeQt = 0;
            aboutBeqtDlg = 0;
        }
      vlt->addLayout(hltHeader);
      twgt = new QTabWidget(q);
      vlt->addWidget(twgt);
      dlgbbox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, q);
        dlgbbox->setFocus();
        QObject::connect( dlgbbox, SIGNAL( rejected() ), q, SLOT( reject() ) );
      vlt->addWidget(dlgbbox);
    //
    retranslateUi();
    connect( BCoreApplication::instance(), SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

BAboutDialogPrivate::~BAboutDialogPrivate()
{
    //
}

//

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

void BAboutDialogPrivate::fillTab(DialogTab t, const BAboutDialog::PersonInfoList &infos)
{
    QString s;
    for (int i = 0; i < infos.size(); ++i)
    {
        const BAboutDialog::PersonInfo &inf = infos.at(i);
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

//

void BAboutDialogPrivate::retranslateUi()
{
    q_func()->setWindowTitle(tr("About", "windowTitle") + " " + appName);
    foreach ( DialogTab t, tbrsrs.keys() )
        twgt->setTabText( tabIndex(t), tabTitle(t) );
    if (tbtnAboutQt)
        tbtnAboutQt->setToolTip( tr("About Qt", "tbtn toolTip") );
    if (tbtnAboutBeQt)
        tbtnAboutBeQt->setToolTip( tr("About BeQt", "tbtn toolTip") );
    if (aboutBeqtDlg)
    {
        QString copyright = "2012 Andrey Bogdanov";
        QString website = "https://github.com/the-dark-angel/BeQt";
        aboutBeqtDlg->setAbout(BApplication::beqtInfo(BApplication::Description), copyright, website);
        aboutBeqtDlg->setChangeLog( BApplication::beqtInfo(BApplication::ChangeLog) );
        BAboutDialog::PersonInfo pi;
        pi.name = tr("Andrey Bogdanov", "info name");
        pi.role = tr("Main developer", "info role");
        pi.mail = "ololoepepe@gmail.com";
        pi.site = "https://github.com/the-dark-angel";
        aboutBeqtDlg->setAuthorsInfos(BAboutDialog::PersonInfoList() << pi);
        pi.role = tr("Translator", "info role");
        aboutBeqtDlg->setTranslationInfos(BAboutDialog::PersonInfoList() << pi);
        aboutBeqtDlg->setLicense( BApplication::beqtInfo(BApplication::License) );
    }
}

/*============================================================================
================================ About Dialog
============================================================================*/

BAboutDialog::BAboutDialog(QWidget *parent) :
    QDialog(parent), BBase( *new BAboutDialogPrivate( this, AboutOptions() ) )
{
    //
}

BAboutDialog::BAboutDialog(const AboutOptions &options, QWidget *parent) :
    QDialog(parent), BBase( *new BAboutDialogPrivate(this, options) )
{
    //
}

BAboutDialog::~BAboutDialog()
{
    //
}

//

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

void BAboutDialog::setAuthorsInfos(const PersonInfoList &infos)
{
    d_func()->fillTab(BAboutDialogPrivate::AuthorsTab, infos);
}

void BAboutDialog::setAuthorsInfos(const BPluginInterface::PersonInfoList &infos)
{
    setAuthorsInfos( fromPluginPersonInfoList(infos) );
}

void BAboutDialog::setTranslationInfos(const PersonInfoList &infos)
{
    d_func()->fillTab(BAboutDialogPrivate::TranslatorsTab, infos);
}

void BAboutDialog::setTranslationInfos(const BPluginInterface::PersonInfoList &infos)
{
    setTranslationInfos( fromPluginPersonInfoList(infos) );
}

void BAboutDialog::setThanksToInfos(const PersonInfoList &infos)
{
    d_func()->fillTab(BAboutDialogPrivate::ThanksToTab, infos);
}

void BAboutDialog::setThanksToInfos(const BPluginInterface::PersonInfoList &infos)
{
    setThanksToInfos( fromPluginPersonInfoList(infos) );
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
    //
}
