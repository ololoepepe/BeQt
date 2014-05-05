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

#ifndef BABOUTDIALOG_P_H
#define BABOUTDIALOG_P_H

class BPersonInfoProvider;

class QString;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QTabWidget;
class QTextBrowser;
class QToolButton;
class QDialogButtonBox;
class QSize;
class QWidget;

#include "baboutdialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BPersonInfo>
#include <BeQtCore/BTranslation>

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
    static const QString HtmlSpaceQuadruple;
    static const QString HtmlLT;
    static const QString HtmlGT;
public:
    explicit BAboutDialogPrivate(BAboutDialog *q);
    ~BAboutDialogPrivate();
public:
    static QString processChangeLog(const QString &text);
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
    void fillTab(DialogTab t, const BPersonInfoList &infos);
    void resetAuthorsProvider(BPersonInfoProvider *prov = 0);
    void resetTranslationProvider(BPersonInfoProvider *prov = 0);
    void resetThanksToProvider(BPersonInfoProvider *prov = 0);
    void resetDescriptionFile( const QString &fileName = QString() );
    void resetChangeLogFile( const QString &fileName = QString() );
    void resetLicenseFile( const QString &fileName = QString() );
    void resetDescription();
    void resetChangeLog();
    void resetLicense();
    void setupFromApplicationData();
public Q_SLOTS:
   void retranslateUi();
   void resetAuthors();
   void resetTranslations();
   void resetThanksTo();
   void tbtnAboutBeqtClicked();
public:
   static QSize defMinSize;
public:
    QString appName;
    QString appVersion;
    QString organization;
    QString copyrightPeriod;
    QString website;
    QString descriptionFileName;
    QString changeLogFileName;
    QString licenseFileName;
    QString description;
    QString changeLog;
    QString license;
    BPersonInfoProvider *authorsProvider;
    BPersonInfoProvider *translationsProvider;
    BPersonInfoProvider *thanksToProvider;
    QMap<DialogTab, QTextBrowser *> tbrsrs;
    QMap<QWidget *, BTranslation> customTabTitles;
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
