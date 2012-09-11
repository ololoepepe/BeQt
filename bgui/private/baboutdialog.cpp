#include "baboutdialog.h"

class QWidget;

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
#include <QEvent>

const QString BAboutDialog::_m_HtmlSpace = "&nbsp;";
const QString BAboutDialog::_m_HtmlSpaceDouble = BAboutDialog::_m_HtmlSpace + BAboutDialog::_m_HtmlSpace;
const QString BAboutDialog::_m_HtmlLT = "&lt;";
const QString BAboutDialog::_m_HtmlGT = "&gt;";

//

BAboutDialog::BAboutDialog(QWidget *parent) :
    QDialog(parent)
{
    _m_vlt = new QVBoxLayout(this);
      _m_hltHeader = new QHBoxLayout(0);
        _m_lblIcon = new QLabel(this);
        _m_hltHeader->addWidget(_m_lblIcon);
        _m_lblText = new QLabel(this);
          _m_lblText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
          QFont font;
          font.setPointSize(12);
          font.setBold(true);
          _m_lblText->setFont(font);
          _m_lblText->setText( QApplication::applicationName() + " v. " + QApplication::applicationVersion() );
        _m_hltHeader->addWidget(_m_lblText);
      _m_vlt->addLayout(_m_hltHeader);
      _m_tabWgt = new QTabWidget(this);
        _m_tbsrAbout = 0;
        _m_tbsrChangeLog = 0;
        _m_tbsrAuthors = 0;
        _m_tbsrTranslators = 0;
        _m_tbsrThanksTo = 0;
        _m_tbsrLicense = 0;
      _m_vlt->addWidget(_m_tabWgt);
      _m_hltActions = new QHBoxLayout(0);
        _m_hltActions->addStretch();
        _m_btnClose = new QPushButton(this);
          connect( _m_btnClose, SIGNAL( clicked() ), this, SLOT( reject() ) );
        _m_hltActions->addWidget(_m_btnClose);
      _m_vlt->addLayout(_m_hltActions);
    _m_retranslateUi();
}

//

void BAboutDialog::setIcon(const QString &fileName)
{
    QPixmap pm(fileName);
    if ( pm.isNull() )
        return;
    _m_lblIcon->setPixmap( QPixmap(fileName).scaled(64, 64) );
}

void BAboutDialog::setAboutText(const QString &text, bool html)
{
    _m_fillTbsr(_m_tbsrAbout, 0, _m_aboutTitle(), text, html);
}

void BAboutDialog::setChangeLog(const QString &text)
{
    QString s;
    QStringList sl = text.split('\n');
    for (int i = 0; i < sl.size(); ++i)
    {
        QString l = sl.at(i);
        int len = l.length();
        l.replace('<', _m_HtmlLT);
        l.replace('>', _m_HtmlGT);
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
                            r += _m_HtmlSpace;
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
    int index = 0;
    if (_m_tbsrAbout)
        ++index;
    _m_fillTbsr(_m_tbsrChangeLog, index, _m_changeLogTitle(), s, true);
}

void BAboutDialog::setChangeLog(const QString &fileName, const char *codecName)
{
    setChangeLog( _m_readFile(fileName, codecName) );
}

void BAboutDialog::setAuthorsInfos(const PersonInfoList &infos)
{
    int index = 0;
    if (_m_tbsrAbout)
        ++index;
    if (_m_tbsrChangeLog)
        ++index;
    _m_fillTbsr(_m_tbsrAuthors, index, _m_authorsTitle(), infos);
}

void BAboutDialog::setTranslationInfos(const PersonInfoList &infos)
{
    int index = 0;
    if (_m_tbsrAbout)
        ++index;
    if (_m_tbsrChangeLog)
        ++index;
    if (_m_tbsrAuthors)
        ++index;
    _m_fillTbsr(_m_tbsrTranslators, index, _m_translatorsTitle(), infos);
}

void BAboutDialog::setThanksToInfos(const PersonInfoList &infos)
{
    int index = 0;
    if (_m_tbsrAbout)
        ++index;
    if (_m_tbsrChangeLog)
        ++index;
    if (_m_tbsrAuthors)
        ++index;
    if (_m_tbsrTranslators)
        ++index;
    _m_fillTbsr(_m_tbsrThanksTo, index, _m_thanksToTitle(), infos);
}

void BAboutDialog::setLicense(const QString &text, const QString &iconFileName)
{
    QTextBrowser *&browser = _m_tbsrLicense;
    if (!browser)
    {
        browser = new QTextBrowser;
        browser->setOpenExternalLinks(true);
        _m_tabWgt->addTab( browser, _m_licenseTitle() );
    }
    browser->clear();
    browser->setPlainText(text);
    if ( !QIcon(iconFileName).isNull() )
    {
        QString html = "<img align=right src=\"";
        html += QUrl::fromLocalFile(iconFileName).toString();
        html += "\" />";
        browser->insertHtml(html);
    }
}

void BAboutDialog::setLicense(const QString &fileName, const char *codecName, const QString &iconFileName)
{
    setLicense(_m_readFile(fileName, codecName), iconFileName);
}

void BAboutDialog::resetTabs()
{
    if (_m_tabWgt->count() < 1)
        return;
    _m_tabWgt->setCurrentIndex(0);
    _m_tabWgt->widget(0)->setFocus();
}

//

void BAboutDialog::changeEvent(QEvent *event)
{
    if (!event || event->type() != QEvent::LanguageChange)
        return QDialog::changeEvent(event);
    _m_retranslateUi();
}

//

void BAboutDialog::_m_retranslateUi()
{
    setWindowTitle( tr("About", "windowTitle") );
    _m_btnClose->setText( tr("Close", "btn text") );
    if (_m_tbsrAbout)
        _m_tabWgt->setTabText( _m_tabWgt->indexOf(_m_tbsrAbout), _m_aboutTitle() );
    if (_m_tbsrChangeLog)
        _m_tabWgt->setTabText( _m_tabWgt->indexOf(_m_tbsrChangeLog), _m_changeLogTitle() );
    if (_m_tbsrAuthors)
        _m_tabWgt->setTabText( _m_tabWgt->indexOf(_m_tbsrAuthors), _m_authorsTitle() );
    if (_m_tbsrTranslators)
        _m_tabWgt->setTabText( _m_tabWgt->indexOf(_m_tbsrTranslators), _m_translatorsTitle() );
    if (_m_tbsrThanksTo)
        _m_tabWgt->setTabText( _m_tabWgt->indexOf(_m_tbsrThanksTo), _m_thanksToTitle() );
    if (_m_tbsrLicense)
        _m_tabWgt->setTabText( _m_tabWgt->indexOf(_m_tbsrLicense), _m_licenseTitle() );
}

QString BAboutDialog::_m_aboutTitle() const
{
    return tr("About", "twgt tabText");
}

QString BAboutDialog::_m_changeLogTitle() const
{
    return tr("Changelog", "twgt tabText");
}

QString BAboutDialog::_m_authorsTitle() const
{
    return tr("Authors", "twgt tabText");
}

QString BAboutDialog::_m_translatorsTitle() const
{
    return tr("Translation", "twgt tabText");
}

QString BAboutDialog::_m_thanksToTitle() const
{
    return tr("Thanks to", "twgt tabText");
}

QString BAboutDialog::_m_licenseTitle() const
{
    return tr("License", "twgt tabText");
}

void BAboutDialog::_m_removeTbsr(QTextBrowser *&browser)
{
    if (!browser)
        return;
    _m_tabWgt->removeTab( _m_tabWgt->indexOf(browser) );
    browser->deleteLater();
    browser = 0;
}

QString BAboutDialog::_m_readFile(const QString &fileName, const char *codecName) const
{
    QFile f(fileName);
    if ( !f.open(QFile::ReadOnly) )
        return QString();
    QTextStream in(&f);
    in.setCodec(codecName);
    QString text = in.readAll();
    f.close();
    return text;
}

void BAboutDialog::_m_fillTbsr(QTextBrowser *&browser, int index, const QString &title, const QString &text, bool html)
{
    if ( title.isEmpty() || text.isEmpty() )
        return _m_removeTbsr(browser);
    if (!browser)
    {
        browser = new QTextBrowser;
        browser->setOpenExternalLinks(true);
        _m_tabWgt->insertTab(index, browser, title);
    }
    browser->clear();
    if (html)
        browser->setHtml(text);
    else
        browser->setPlainText(text);
}

void BAboutDialog::_m_fillTbsr(QTextBrowser *&browser, int index, const QString &title, const PersonInfoList &infos)
{
    QString s;
    for (int i = 0; i < infos.size(); ++i)
    {
        const PersonInfo &inf = infos.at(i);
        if ( inf.name.isEmpty() )
            continue;
        s += inf.name + "<br>";
        if ( !inf.mail.isEmpty() )
            s += _m_HtmlSpaceDouble + "E-mail: <a href=\"mailto:" + inf.mail + "\">" + inf.mail + "</a><br>";
        if ( !inf.site.isEmpty() )
            s += _m_HtmlSpaceDouble + "www: <a href = \"" + inf.site + "\">" + inf.site + "</a><br>";
        if ( !inf.role.isEmpty() )
            s += _m_HtmlSpaceDouble + inf.role + "<br>";
        if (i < infos.size() - 1)
            s += "<br>";
    }
    _m_fillTbsr(browser, index, title, s, true);
}
