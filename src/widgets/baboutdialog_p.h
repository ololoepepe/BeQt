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

class QDialogButtonBox;
class QLabel;
class QTabWidget;
class QTextBrowser;
class QToolButton;
class QWidget;

#include "baboutdialog.h"

#include <BeQtCore/BApplicationBase>
#include <BeQtCore/BPersonInfoList>
#include <BeQtCore/BTranslation>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QList>
#include <QMap>
#include <QObject>
#include <QSize>
#include <QString>

/*============================================================================
================================ BAboutDialogPrivate =========================
============================================================================*/

class B_WIDGETS_EXPORT BAboutDialogPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BAboutDialog)
public:
    enum DialogTab
    {
        DescriptionTab = 1,
        ChangeLogTab,
        AuthorsTab,
        TranslatorsTab,
        ThanksToTab,
        LicenseTab
    };
    struct SourceInfo
    {
        QString defaultFileName;
        QString fileName;
        QString possibleSuffix;
    };
public:
    static const QString HtmlSpace;
    static const QString HtmlSpaceDouble;
    static const QString HtmlSpaceQuadruple;
public:
    static QSize defMinSize;
public:
    BAboutDialog *aboutBeqtDlg;
    QString appName;
    QString appVersion;
    BPersonInfoList authors;
    QString authorsFileName;
    BPersonInfoProvider *authorsProvider;
    QString changeLog;
    QString changeLogFileName;
    QList<BApplicationBase::CopyrightInfo> copyrightInfos;
    QString copyrightPeriod;
    QMap<QWidget *, BTranslation> customTabTitles;
    QString description;
    QString descriptionFileName;
    QDialogButtonBox *dlgbbox;
    QLabel *lblCopyright;
    QLabel *lblCopyrightInfos;
    QLabel *lblIcon;
    QLabel *lblName;
    QLabel *lblWebsite;
    QString license;
    QString licenseFileName;
    QString organization;
    QToolButton *tbtnAboutBeqt;
    QToolButton *tbtnAboutQt;
    BPersonInfoList thanksTo;
    QString thanksToFileName;
    BPersonInfoProvider *thanksToProvider;
    BPersonInfoList translators;
    QString translatorsFileName;
    BPersonInfoProvider *translatorsProvider;
    QMap<DialogTab, QTextBrowser *> tbrsrs;
    QTabWidget *twgt;
    QString website;
public:
    explicit BAboutDialogPrivate(BAboutDialog *q);
    ~BAboutDialogPrivate();
public:
    static QString processChangeLog(const QString &text);
public:
    void fillTab(DialogTab t, const QString &text, bool html);
    void fillTab(DialogTab t, const BPersonInfoList &infos);
    void init();
    void initAboutBeqtDialog();
    void removeTab(DialogTab t);
    void resetAuthorsProvider(BPersonInfoProvider *prov = 0);
    void resetChangeLog();
    void resetChangeLogFile(const QString &fileName = QString());
    void resetDescription();
    void resetDescriptionFile(const QString &fileName = QString());
    void resetLicense();
    void resetLicenseFile(const QString &fileName = QString());
    void resetThanksToProvider(BPersonInfoProvider *prov = 0);
    void resetTranslatorsProvider(BPersonInfoProvider *prov = 0);
    void retranslateAboutBeqtDialog();
    void setupFromApplicationData();
    int tabIndex(DialogTab t) const;
    QString tabTitle(DialogTab t) const;
    void updateCopyright();
    void updateWebsite();
    void updateWindowTitle();
public Q_SLOTS:
    void resetAuthors();
    void resetThanksTo();
    void resetTranslations();
    void retranslateUi();
    void showCopyrightInfos();
    void tbtnAboutBeqtClicked();
private:
    Q_DISABLE_COPY(BAboutDialogPrivate)
};

#endif // BABOUTDIALOG_P_H
