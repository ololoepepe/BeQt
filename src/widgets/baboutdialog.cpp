/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtWidgets module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

class QWidget;

#include "baboutdialog.h"
#include "baboutdialog_p.h"

#include "bapplication.h"
#include "bdialog.h"

#include <BeQtCore/BApplicationBase>
#include <BeQtCore/BDirTools>
#include <BeQtCore/BPersonInfo>
#include <BeQtCore/BPersonInfoProvider>
#include <BeQtCore/BTextTools>
#include <BeQtCore/BVersion>

#include <QApplication>
#include <QChar>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFont>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QList>
#include <QPixmap>
#include <QPushButton>
#include <QRegExp>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QTabWidget>
#include <QTextBrowser>
#include <QToolButton>
#include <QVBoxLayout>

/*============================================================================
================================ BAboutDialogPrivate =========================
============================================================================*/

/*============================== Static public constants ===================*/

const QString BAboutDialogPrivate::HtmlSpace = "&nbsp;";
const QString BAboutDialogPrivate::HtmlSpaceDouble = BAboutDialogPrivate::HtmlSpace + BAboutDialogPrivate::HtmlSpace;
const QString BAboutDialogPrivate::HtmlSpaceQuadruple =
        BAboutDialogPrivate::HtmlSpaceDouble + BAboutDialogPrivate::HtmlSpaceDouble;

/*============================== Static public variables ===================*/

QSize BAboutDialogPrivate::defMinSize = QSize(700, 500);

/*============================== Public constructors =======================*/

BAboutDialogPrivate::BAboutDialogPrivate(BAboutDialog *q) :
    BBaseObjectPrivate(q)
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
    QString ntext = text;
    ntext.remove('\r');
    QString s;
    foreach (QString line, ntext.split('\n')) {
        if (QRegExp("^\\s*==.*$").exactMatch(line) || QRegExp("^\\s*=.*=\\s*$").exactMatch(line)) {
            s += ("<font color=\"darkred\">" + BTextTools::toHtml(line) + "</font>");
        } else if (QRegExp("^\\s+\\*\\s+#\\d+:.*$").exactMatch(line)) {
            line = BTextTools::toHtml(line);
            int ind = line.indexOf(':');
            line.insert(ind + 1, "</font>");
            ind = line.indexOf('#');
            line.insert(ind, "</font><font color=\"blue\">");
            ind = line.indexOf('*');
            line.insert(ind, "<font color=\"darkgreen\">");
            s += line;
        } else if (QRegExp("^\\s+\\*.*$").exactMatch(line)) {
            line = BTextTools::toHtml(line);
            int ind = line.indexOf('*');
            line.insert(ind + 1, "</font><font color=\"blue\">");
            line.insert(ind, "<font color=\"darkgreen\">");
            line.append("</font>");
            s += line;
        }
        s += "<br>";
    }
    if (!s.isEmpty())
        s.remove(s.length() - 4, 4);
    return s;
}

/*============================== Public methods ============================*/

void BAboutDialogPrivate::fillTab(DialogTab t, const QString &text, bool html)
{
    if (text.isEmpty())
        return removeTab(t);
    QTextBrowser *&tab = tbrsrs[t];
    if (!tab) {
        tab = new QTextBrowser;
        tab->setLineWrapMode(QTextBrowser::NoWrap);
        tab->setOpenExternalLinks(true);
        twgt->insertTab(tabIndex(t), tab, tabTitle(t));
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
    for (int i = 0; i < infos.size(); ++i) {
        const BPersonInfo &inf = infos.at(i);
        if (inf.name().isEmpty() || inf.role().isEmpty())
            continue;
        QString sp = "";
        QString dsp = HtmlSpaceDouble;
        s += "<div>";
        QString img = BDirTools::findResource(inf.image());
        QPixmap pm(img);
        if (pm.isNull()) {
            img = BDirTools::findResource("beqt/pixmaps/no_image.png");
            pm.load(img);
        }
        if (!pm.isNull()) {
            double k = 80.0 / double(qMax(pm.height(), pm.width()));
            QString h = QString::number((int) (k * (double) pm.height()));
            QString w = QString::number((int) (k * (double) pm.width()));
            s += "<img height=" + h + " width=" + w + " style=\"float: left\" src=\"" + img + "\""
                    + (inf.image().isEmpty() ? (" title=\"" + tr("No image available") + "\"") : QString("")) + ">";
            sp = HtmlSpaceDouble;
            dsp = HtmlSpaceQuadruple;
        }
        s += sp + "<b>" + inf.name() + "</b><br>" + dsp + inf.role();
        if (!inf.site().isEmpty()) {
            s += "<br>" + dsp + "<i>" + tr("Website", "personInfo text") +
                 "</i>: <a href = \"" + inf.site() + "\">" + inf.site() + "</a>";
        }
        if (!inf.mail().isEmpty()) {
            s += "<br>" + dsp + "<i>" + tr("E-mail", "personInfo text") +
                 "</i>: <a href=\"mailto:" + inf.mail() + "\">" + inf.mail() + "</a>";
        }
        s += "</div>";
        if (i < infos.size() - 1)
            s += "<br>";
    }
    fillTab(t, s, true);
}

void BAboutDialogPrivate::init()
{
    authorsProvider = 0;
    translatorsProvider = 0;
    thanksToProvider = 0;
    aboutBeqtDlg = 0;
    //
    B_Q(BAboutDialog);
    q->setMinimumSize(defMinSize);
    QVBoxLayout *vlt = new QVBoxLayout(q);
      QHBoxLayout *hltHeader = new QHBoxLayout;
        lblIcon = new QLabel(q);
          lblIcon->setVisible(false);
        hltHeader->addWidget(lblIcon);
        QVBoxLayout *vltHeader = new QVBoxLayout;
          lblName = new QLabel(q);
            QFont font;
            font.setPointSize(12);
            font.setBold(true);
            lblName->setFont(font);
          vltHeader->addWidget(lblName);
          QHBoxLayout *hltCRWebsite = new QHBoxLayout;
            lblCopyright = new QLabel(q);
              lblCopyright->setTextFormat(Qt::RichText);
              lblCopyright->setVisible(false);
            hltCRWebsite->addWidget(lblCopyright);
            lblCopyrightInfos = new QLabel(q);
              lblCopyrightInfos->setTextFormat(Qt::RichText);
              lblCopyrightInfos->setVisible(false);
              connect(lblCopyrightInfos, SIGNAL(linkActivated(QString)), this, SLOT(showCopyrightInfos()));
            hltCRWebsite->addWidget(lblCopyrightInfos);
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
          tbtnAboutQt->setIcon(QIcon(BApplication::beqtPixmap("qt_logo")));
          connect(tbtnAboutQt, SIGNAL(clicked()), QApplication::instance(), SLOT(aboutQt()));
        hltHeader->addWidget(tbtnAboutQt);
        tbtnAboutBeqt = new QToolButton(q);
          tbtnAboutBeqt->setVisible(false);
          tbtnAboutBeqt->setIcon(QIcon(BApplication::beqtPixmap("beqt_logo")));
          connect(tbtnAboutBeqt, SIGNAL(clicked()), this, SLOT(tbtnAboutBeqtClicked()));
        hltHeader->addWidget(tbtnAboutBeqt);
      vlt->addLayout(hltHeader);
      twgt = new QTabWidget(q);
      vlt->addWidget(twgt);
      dlgbbox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, q);
        dlgbbox->setFocus();
        QObject::connect(dlgbbox, SIGNAL(rejected()), q, SLOT(reject()));
      vlt->addWidget(dlgbbox);
    //
    q->setAppName(QApplication::applicationName());
    q->setAppVersion(QApplication::applicationVersion());
    q->setOrganization(QApplication::organizationName());
    q->setWebsite(QApplication::organizationDomain());
    retranslateUi();
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

void BAboutDialogPrivate::initAboutBeqtDialog()
{
    aboutBeqtDlg = new BAboutDialog(q_func(), "BeQt", bVersion());
    aboutBeqtDlg->setOrganization("Andrey Bogdanov");
    aboutBeqtDlg->setCopyrightPeriod("2012-2014");
    aboutBeqtDlg->setWebsite("https://github.com/ololoepepe/BeQt");
    aboutBeqtDlg->setPixmap(BApplication::beqtPixmap("beqt_logo"));
    aboutBeqtDlg->setAuthorsProvider(BApplication::beqtAuthorsProvider());
    aboutBeqtDlg->setTranslatorsProvider(BApplication::beqtTranslationsProvider());
    aboutBeqtDlg->setThanksToProvider(BApplication::beqtThanksToProvider());
    retranslateAboutBeqtDialog();
}

void BAboutDialogPrivate::removeTab(DialogTab t)
{
    QTextBrowser *tab = tbrsrs.take(t);
    if (!tab)
        return;
    twgt->removeTab( twgt->indexOf(tab) );
    tab->deleteLater();
}

void BAboutDialogPrivate::resetAuthorsProvider(BPersonInfoProvider *prov)
{
    if (authorsProvider) {
        disconnect(authorsProvider, SIGNAL(reloaded()), this, SLOT(resetAuthors()));
        if (!authorsProvider->parent() || (authorsProvider->parent() == (QObject *) this))
            authorsProvider->deleteLater();
    }
    authorsProvider = prov;
    if (authorsProvider) {
        if (!authorsProvider->parent())
            authorsProvider->setParent(this);
        connect(authorsProvider, SIGNAL(reloaded()), this, SLOT(resetAuthors()));
    }
    resetAuthors();
}

void BAboutDialogPrivate::resetChangeLog()
{
    QString fn = BDirTools::localeBasedFileName(changeLogFileName);
    fillTab(ChangeLogTab, processChangeLog(!fn.isEmpty() ? BDirTools::readTextFile(fn, "UTF-8") : changeLog), true);
}

void BAboutDialogPrivate::resetChangeLogFile(const QString &fileName)
{
    changeLogFileName = fileName;
    if (!fileName.isEmpty())
        changeLog.clear();
    resetChangeLog();
}

void BAboutDialogPrivate::resetDescription()
{
    QString fn = BDirTools::localeBasedFileName(descriptionFileName);
    QString s = !fn.isEmpty() ? BDirTools::readTextFile(fn, "UTF-8") : description;
    fillTab(DescriptionTab, s, Qt::mightBeRichText(s));
}

void BAboutDialogPrivate::resetDescriptionFile(const QString &fileName)
{
    descriptionFileName = fileName;
    if (!fileName.isEmpty())
        description.clear();
    resetDescription();
}

void BAboutDialogPrivate::resetLicense()
{
    QString fn = BDirTools::localeBasedFileName(licenseFileName);
    QString s = !fn.isEmpty() ? BDirTools::readTextFile(fn, "UTF-8") : license;
    fillTab(LicenseTab, s, Qt::mightBeRichText(s));
}

void BAboutDialogPrivate::resetLicenseFile(const QString &fileName)
{
    licenseFileName = fileName;
    if (!fileName.isEmpty())
        license.clear();
    resetLicense();
}

void BAboutDialogPrivate::resetThanksToProvider(BPersonInfoProvider *prov)
{
    if (thanksToProvider) {
        disconnect(thanksToProvider, SIGNAL(reloaded()), this, SLOT(resetThanksTo()));
        if (!thanksToProvider->parent() || (thanksToProvider->parent() == (QObject *) this))
            thanksToProvider->deleteLater();
    }
    thanksToProvider = prov;
    if (thanksToProvider) {
        if (!thanksToProvider->parent())
            thanksToProvider->setParent(this);
        connect(thanksToProvider, SIGNAL(reloaded()), this, SLOT(resetThanksTo()));
    }
    resetThanksTo();
}

void BAboutDialogPrivate::resetTranslatorsProvider(BPersonInfoProvider *prov)
{
    if (translatorsProvider) {
        disconnect(translatorsProvider, SIGNAL(reloaded()), this, SLOT(resetTranslations()));
        if (!translatorsProvider->parent() || (translatorsProvider->parent() == (QObject *) this))
            translatorsProvider->deleteLater();
    }
    translatorsProvider = prov;
    if (prov) {
        if (!prov->parent())
            prov->setParent(this);
        connect(prov, SIGNAL(reloaded()), this, SLOT(resetTranslations()));
    }
    resetTranslations();
}

void BAboutDialogPrivate::retranslateAboutBeqtDialog()
{
    if (!aboutBeqtDlg)
        return;
    aboutBeqtDlg->setWindowTitle(tr("About BeQt", "adlg windowTitle"));
    aboutBeqtDlg->setDescription(BApplicationBase::beqtInfo(BApplicationBase::Description));
    aboutBeqtDlg->setChangeLog(BApplicationBase::beqtInfo(BApplicationBase::ChangeLog));
    aboutBeqtDlg->setLicense(BApplicationBase::beqtInfo(BApplicationBase::License));
}

void BAboutDialogPrivate::setupFromApplicationData()
{
    B_Q(BAboutDialog);
    q->setOrganization(QApplication::organizationName());
    q->setCopyrightPeriod(BApplicationBase::applicationCopyrightPeriod());
    q->setWebsite(QApplication::organizationDomain());
    if (!BApplicationBase::applicationDescription().isEmpty())
        q->setDescription(BApplicationBase::applicationDescription());
    else
        q->setDescriptionFile(BApplicationBase::applicationDescriptionFile());
    if (!BApplicationBase::applicationChangeLog().isEmpty())
        q->setChangeLog(BApplicationBase::applicationChangeLog());
    else
        q->setChangeLogFile(BApplicationBase::applicationChangeLogFile());
    if (!BApplicationBase::applicationLicense().isEmpty())
        q->setLicense(BApplicationBase::applicationLicense());
    else
        q->setLicenseFile(BApplicationBase::applicationLicenseFile());
    if (!BApplicationBase::applicationAuthors().isEmpty())
        q->setAuthors(BApplicationBase::applicationAuthors());
    else
        q->setAuthorsProvider(BApplicationBase::applicationAuthorsProvider());
    if (!BApplicationBase::applicationTranslations().isEmpty())
        q->setTranslators(BApplicationBase::applicationTranslations());
    else
        q->setTranslatorsProvider(BApplicationBase::applicationTranslationsProvider());
    if (!BApplicationBase::applicationThanksTo().isEmpty())
        q->setThanksTo(BApplicationBase::applicationThanksTo());
    else
        q->setThanksToProvider(BApplicationBase::applicationThanksToProvider());
    q->setPixmap(QApplication::windowIcon().pixmap(100000)); //Using such an extent to get the largest pixmap
}

int BAboutDialogPrivate::tabIndex(DialogTab t) const
{
    switch (t) {
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

QString BAboutDialogPrivate::tabTitle(DialogTab t) const
{
    switch (t) {
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

void BAboutDialogPrivate::updateCopyright()
{
    if (organization.isEmpty()) {
        lblCopyright->setVisible(false);
        return;
    }
    lblCopyright->setVisible(true);
    QString cr = !copyrightPeriod.isEmpty() ? (copyrightPeriod + " ") : QString();
    lblCopyright->setText(tr("Copyright", "lbl text") + " &copy; " + cr + organization);
}

void BAboutDialogPrivate::updateWebsite()
{
    lblWebsite->setText("<a href=\"" + website + "\">[" + tr("Website", "lbl text") + "]</a>");
    lblWebsite->setToolTip(website);
}

void BAboutDialogPrivate::updateWindowTitle()
{
    q_func()->setWindowTitle(tr("About", "windowTitle") + " " + appName);
}

/*============================== Public slots ==============================*/

void BAboutDialogPrivate::resetAuthors()
{
    fillTab(AuthorsTab, authorsProvider ? authorsProvider->infos() : authors);
}

void BAboutDialogPrivate::resetThanksTo()
{
    fillTab(ThanksToTab, thanksToProvider ? thanksToProvider->infos() : thanksTo);
}

void BAboutDialogPrivate::resetTranslations()
{
    fillTab(TranslatorsTab, translatorsProvider ? translatorsProvider->infos(true) : translators);
}


void BAboutDialogPrivate::retranslateUi()
{
    updateWindowTitle();
    updateCopyright();
    lblCopyrightInfos->setText("(<a href=\"fake\">" + tr("more...", "lbl text") + "</a>)");
    lblCopyrightInfos->setToolTip(tr("Show extended copyrights information", "lbl text"));
    updateWebsite();
    foreach (DialogTab t, tbrsrs.keys())
        twgt->setTabText(tabIndex(t), tabTitle(t));
    resetAuthors();
    resetTranslations();
    resetThanksTo();
    resetDescription();
    resetChangeLog();
    resetLicense();
    tbtnAboutQt->setToolTip(tr("About Qt", "tbtn toolTip"));
    tbtnAboutBeqt->setToolTip(tr("About BeQt", "tbtn toolTip"));
    retranslateAboutBeqtDialog();
    foreach (int i, bRangeD(0, twgt->count() - 1)) {
        QWidget *tab = twgt->widget(i);
        if (!customTabTitles.contains(tab))
            return;
        twgt->setTabText(i, customTabTitles.value(tab));
    }
}

void BAboutDialogPrivate::showCopyrightInfos()
{
    BDialog dlg(q_func());
    dlg.setMinimumSize(400, 200);
    dlg.setWindowTitle(tr("Copyrights information", "dlg windowTitle"));
    QTextBrowser *bsr = new QTextBrowser;
    bsr->setLineWrapMode(QTextBrowser::NoWrap);
    bsr->setOpenExternalLinks(true);
    bsr->setReadOnly(true);
    QString s;
    foreach (const BApplicationBase::CopyrightInfo &ci, copyrightInfos) {
        if (!s.isEmpty())
            s += "<br><br>";
        s += tr("Copyright", "tbsr text") + " &copy; " + ci.period + " " + ci.owner;
        if (!ci.email.isEmpty()) {
            s += "<br>" + HtmlSpaceDouble + tr("E-mail:", "tbsr text")
                    + " <a href=\"mailto:" + ci.email + "\">" + ci.email + "</a>";
        }
    }
    bsr->setHtml(s);
    dlg.setWidget(bsr);
    dlg.addButton(QDialogButtonBox::Close, SLOT(close()));
    dlg.exec();
}

void BAboutDialogPrivate::tbtnAboutBeqtClicked()
{
    if (!aboutBeqtDlg)
        initAboutBeqtDialog();
    if (aboutBeqtDlg->isVisible()) {
        aboutBeqtDlg->activateWindow();
    } else {
        aboutBeqtDlg->resetTabs();
        aboutBeqtDlg->exec();
    }
}

/*============================================================================
================================ BAboutDialog ================================
============================================================================*/

/*============================== Public constructors =======================*/

BAboutDialog::BAboutDialog(QWidget *parent) :
    QDialog(parent), BBaseObject(*new BAboutDialogPrivate(this))
{
    d_func()->init();
}

BAboutDialog::BAboutDialog(QWidget *parent, const QString &appName, const QString &appVersion) :
    QDialog(parent), BBaseObject(*new BAboutDialogPrivate(this))
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
    QDialog(parent), BBaseObject(d)
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
    setDefaultMinimumSize(QSize(width, height));
}

/*============================== Public methods ============================*/

bool BAboutDialog::aboutBeqtShown() const
{
    return d_func()->tbtnAboutBeqt->isVisibleTo(const_cast<BAboutDialog *>(this));
}

bool BAboutDialog::aboutQtShown() const
{
    return d_func()->tbtnAboutQt->isVisibleTo(const_cast<BAboutDialog *>(this));
}

void BAboutDialog::addTab(QWidget *tab, const BTranslation &title)
{
    if (!tab || !d_func()->customTabTitles.contains(tab))
        return;
    d_func()->twgt->addTab(tab, title);
    d_func()->customTabTitles.insert(tab, title);
}

QString BAboutDialog::appName() const
{
    return d_func()->appName;
}

QString BAboutDialog::appVersion() const
{
    return d_func()->appVersion;
}

BPersonInfoList BAboutDialog::authors() const
{
    return d_func()->authors;
}

QString BAboutDialog::authorsFileName() const
{
    return d_func()->authorsProvider ? d_func()->authorsProvider->fileName() : d_func()->authorsFileName;
}

BPersonInfoProvider *BAboutDialog::authorsProvider() const
{
    return d_func()->authorsProvider;
}

QString BAboutDialog::changeLog() const
{
    return d_func()->changeLog;
}

QString BAboutDialog::changeLogFileName() const
{
    return d_func()->changeLogFileName;
}

QString BAboutDialog::copyrightPeriod() const
{
    return d_func()->copyrightPeriod;
}

QString BAboutDialog::description() const
{
    return d_func()->description;
}

QString BAboutDialog::descriptionFileName() const
{
    return d_func()->descriptionFileName;
}

QList<BApplicationBase::CopyrightInfo> BAboutDialog::extendedCopyrightInfos() const
{
    return d_func()->copyrightInfos;
}

QString BAboutDialog::license() const
{
    return d_func()->license;
}

QString BAboutDialog::licenseFileName() const
{
    return d_func()->licenseFileName;
}

QString BAboutDialog::organization() const
{
    return d_func()->organization;
}

QPixmap BAboutDialog::pixmap() const
{
    const QPixmap *pm = d_func()->lblIcon->pixmap();
    return pm ? *pm : QPixmap();
}

void BAboutDialog::removeTab(QWidget *tab)
{
    if (!tab)
        return;
    int ind = d_func()->twgt->indexOf(tab);
    if (ind < 0)
        return;
    d_func()->twgt->removeTab(ind);
    d_func()->customTabTitles.remove(tab);
}

void BAboutDialog::setAboutBeqtShown(bool b)
{
    d_func()->tbtnAboutBeqt->setVisible(b);
}

void BAboutDialog::setAboutQtShown(bool b)
{
    d_func()->tbtnAboutQt->setVisible(b);
}

void BAboutDialog::setAppName(const QString &name)
{
    B_D(BAboutDialog);
    d->appName = name;
    d->updateWindowTitle();
    d->lblName->setText(d->appName + " v" + d->appVersion);
}

void BAboutDialog::setAppVersion(const QString &version)
{
    B_D(BAboutDialog);
    d->appVersion = version;
    d->lblName->setText(d->appName + " v" + d->appVersion);
}

void BAboutDialog::setAppVersion(const BVersion &version, BVersion::StatusRepresentation r, QChar versionSeparator,
                                 QChar statusSeparator)
{
    setAppVersion(version.toString(r, versionSeparator, statusSeparator));
}

void BAboutDialog::setAuthors(const BPersonInfoList &list)
{
    d_func()->authors = list;
    d_func()->authorsFileName.clear();
    d_func()->resetAuthorsProvider();
    d_func()->fillTab(BAboutDialogPrivate::AuthorsTab, list);
}

void BAboutDialog::setAuthorsFile(const QString &fileName)
{
    d_func()->authors.clear();
    d_func()->authorsFileName = fileName;
    setAuthorsProvider(new BPersonInfoProvider(fileName, d_func()));
}

void BAboutDialog::setAuthorsProvider(BPersonInfoProvider *prov)
{
    d_func()->authors.clear();
    d_func()->authorsFileName.clear();
    d_func()->resetAuthorsProvider(prov);
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

void BAboutDialog::setCopyrightPeriod(const QString &copyrightPeriod)
{
    B_D(BAboutDialog);
    d->copyrightPeriod = copyrightPeriod;
    d->updateCopyright();
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

void BAboutDialog::setExtendedCopyrightInfos(const QList<BApplicationBase::CopyrightInfo> &list)
{
    B_D(BAboutDialog);
    d->copyrightInfos = list;
    foreach (int i, bRangeR(d->copyrightInfos.size() - 1, 0)) {
        const BApplicationBase::CopyrightInfo &ci = d->copyrightInfos.at(i);
        if (ci.owner.isEmpty() || ci.period.isEmpty())
            d->copyrightInfos.removeAt(i);
    }
    d->lblCopyrightInfos->setVisible(!list.isEmpty());
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

void BAboutDialog::setOrganization(const QString &organization)
{
    B_D(BAboutDialog);
    d->organization = organization;
    d->updateCopyright();
}

void BAboutDialog::setPixmap(const QPixmap &pixmap)
{
    B_D(BAboutDialog);
    d->lblIcon->setPixmap( pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation) );
    d->lblIcon->setVisible(!pixmap.isNull());
}

void BAboutDialog::setPixmap(const QString &fileName)
{
    setPixmap(QPixmap(fileName));
}

void BAboutDialog::setThanksTo(const BPersonInfoList &list)
{
    d_func()->thanksTo = list;
    d_func()->thanksToFileName.clear();
    d_func()->resetThanksToProvider();
    d_func()->fillTab(BAboutDialogPrivate::ThanksToTab, list);
}

void BAboutDialog::setThanksToFile(const QString &fileName)
{
    d_func()->thanksTo.clear();
    d_func()->thanksToFileName = fileName;
    setThanksToProvider(new BPersonInfoProvider(fileName, d_func()));
}

void BAboutDialog::setThanksToProvider(BPersonInfoProvider *prov)
{
    d_func()->thanksTo.clear();
    d_func()->thanksToFileName.clear();
    d_func()->resetThanksToProvider(prov);
}

void BAboutDialog::setTranslators(const BPersonInfoList &list)
{
    d_func()->translators = list;
    d_func()->translatorsFileName.clear();
    d_func()->resetTranslatorsProvider();
    d_func()->fillTab(BAboutDialogPrivate::TranslatorsTab, list);
}

void BAboutDialog::setTranslatorsFile(const QString &fileName)
{
    d_func()->translators.clear();
    d_func()->translatorsFileName = fileName;
    setTranslatorsProvider(new BPersonInfoProvider(fileName, d_func()));
}

void BAboutDialog::setTranslatorsProvider(BPersonInfoProvider *prov)
{
    d_func()->translators.clear();
    d_func()->translatorsFileName.clear();
    d_func()->resetTranslatorsProvider(prov);
}

void BAboutDialog::setupWithApplicationData()
{
    d_func()->setupFromApplicationData();
}

void BAboutDialog::setWebsite(const QString &site)
{
    if (site.isEmpty())
        return;
    B_D(BAboutDialog);
    d->website = site;
    d->lblWebsite->setVisible(true);
    d->updateWebsite();
}

BPersonInfoList BAboutDialog::thanksTo() const
{
    return d_func()->thanksTo;
}

QString BAboutDialog::thanksToFileName() const
{
    return d_func()->thanksToProvider ? d_func()->thanksToProvider->fileName() : d_func()->thanksToFileName;
}

BPersonInfoProvider *BAboutDialog::thanksToProvider() const
{
    return d_func()->thanksToProvider;
}

BPersonInfoList BAboutDialog::translators() const
{
    return d_func()->translators;
}

QString BAboutDialog::translatorsFileName() const
{
    return d_func()->translatorsProvider ? d_func()->translatorsProvider->fileName() : d_func()->translatorsFileName;
}

BPersonInfoProvider *BAboutDialog::translatorsProvider() const
{
    return d_func()->translatorsProvider;
}

QString BAboutDialog::website() const
{
    return d_func()->website;
}

/*============================== Public slots ==============================*/

void BAboutDialog::resetTabs()
{
    QTabWidget *twgt = d_func()->twgt;
    if (!twgt->count())
        return;
    twgt->setCurrentIndex(0);
    twgt->widget(0)->setFocus();
}
