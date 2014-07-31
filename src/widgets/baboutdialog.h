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

#ifndef BABOUTDIALOG_H
#define BABOUTDIALOG_H

class BAboutDialogPrivate;

class BPersonInfoProvider;
class BTranslation;

class QPixmap;
class QSize;
class QWidget;

#include <BeQtCore/BApplicationBase>
#include <BeQtCore/BBaseObject>
#include <BeQtCore/BPersonInfoList>
#include <BeQtCore/BVersion>

#include <QChar>
#include <QDialog>
#include <QList>
#include <QString>

/*============================================================================
================================ BAboutDialog ================================
============================================================================*/

class B_WIDGETS_EXPORT BAboutDialog : public QDialog, public BBaseObject
{
    Q_OBJECT
    Q_PROPERTY(bool aboutBeqtShown READ aboutBeqtShown WRITE setAboutBeqtShown)
    Q_PROPERTY(bool aboutQtShown READ aboutQtShown WRITE setAboutQtShown)
    Q_PROPERTY(QString appName READ appName WRITE setAppName)
    Q_PROPERTY(QString appVersion READ appVersion)
    Q_PROPERTY(BPersonInfoList authors READ authors WRITE setAuthors)
    Q_PROPERTY(QString authorsFileName READ authorsFileName WRITE setAuthorsFile)
    Q_PROPERTY(BPersonInfoProvider * authorsProvider READ authorsProvider WRITE setAuthorsProvider)
    Q_PROPERTY(QString changeLog READ changeLog WRITE setChangeLog)
    Q_PROPERTY(QString changeLogFileName READ changeLogFileName WRITE setChangeLogFile)
    Q_PROPERTY(QString copyrightPeriod READ copyrightPeriod WRITE setCopyrightPeriod)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QString descriptionFileName READ descriptionFileName WRITE setDescriptionFile)
    Q_PROPERTY(QString license READ license WRITE setLicense)
    Q_PROPERTY(QString licenseFileName READ licenseFileName WRITE setLicenseFile)
    Q_PROPERTY(QString organization READ organization WRITE setOrganization)
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)
    Q_PROPERTY(BPersonInfoList thanksTo READ thanksTo WRITE setThanksTo)
    Q_PROPERTY(QString thanksToFileName READ thanksToFileName WRITE setThanksToFile)
    Q_PROPERTY(BPersonInfoProvider * thanksToProvider READ thanksToProvider WRITE setThanksToProvider)
    Q_PROPERTY(BPersonInfoList translators READ translators WRITE setTranslators)
    Q_PROPERTY(QString translatorsFileName READ translatorsFileName WRITE setTranslatorsFile)
    Q_PROPERTY(BPersonInfoProvider * translatorsProvider READ translatorsProvider WRITE setTranslatorsProvider)
    Q_PROPERTY(QString website READ website WRITE setWebsite)
    B_DECLARE_PRIVATE(BAboutDialog)
public:
    explicit BAboutDialog(QWidget *parent = 0);
    explicit BAboutDialog(QWidget *parent, const QString &appName, const QString &appVersion = QString());
    ~BAboutDialog();
protected:
    explicit BAboutDialog(BAboutDialogPrivate &d, QWidget *parent = 0);
public:
    static void setDefaultMinimumSize(const QSize &sz);
    static void setDefaultMinimumSize(int width, int height);
public:
    bool aboutBeqtShown() const;
    bool aboutQtShown() const;
    void addTab(QWidget *tab, const BTranslation &title);
    QString appName() const;
    QString appVersion() const;
    BPersonInfoList authors() const;
    QString authorsFileName() const;
    BPersonInfoProvider *authorsProvider() const;
    QString changeLog() const;
    QString changeLogFileName() const;
    QString copyrightPeriod() const;
    QString description() const;
    QString descriptionFileName() const;
    QList<BApplicationBase::CopyrightInfo> extendedCopyrightInfos() const;
    QString license() const;
    QString licenseFileName() const;
    QString organization() const;
    QPixmap pixmap() const;
    void removeTab(QWidget *tab);
    void setAboutBeqtShown(bool b);
    void setAboutQtShown(bool b);
    void setAppName(const QString &name);
    void setAppVersion(const QString &version);
    void setAppVersion(const BVersion &version, BVersion::StatusRepresentation r = BVersion::ShortLowercase,
                       QChar versionSeparator = '.', QChar statusSeparator = '-');
    void setAuthors(const BPersonInfoList &list);
    void setAuthorsFile(const QString &fileName);
    void setAuthorsProvider(BPersonInfoProvider *prov);
    void setChangeLog(const QString &text);
    void setChangeLogFile(const QString &fileName);
    void setCopyrightPeriod(const QString &copyrightPeriod);
    void setDescription(const QString &text);
    void setDescriptionFile(const QString &fileName);
    void setExtendedCopyrightInfos(const QList<BApplicationBase::CopyrightInfo> &list);
    void setLicense(const QString &text);
    void setLicenseFile(const QString &fileName);
    void setOrganization(const QString &organization);
    void setPixmap(const QPixmap &pixmap);
    void setPixmap(const QString &fileName);
    void setThanksTo(const BPersonInfoList &list);
    void setThanksToFile(const QString &fileName);
    void setThanksToProvider(BPersonInfoProvider *prov);
    void setTranslators(const BPersonInfoList &list);
    void setTranslatorsFile(const QString &fileName);
    void setTranslatorsProvider(BPersonInfoProvider *prov);
    void setupWithApplicationData();
    void setWebsite(const QString &site);
    BPersonInfoList thanksTo() const;
    QString thanksToFileName() const;
    BPersonInfoProvider *thanksToProvider() const;
    BPersonInfoList translators() const;
    QString translatorsFileName() const;
    BPersonInfoProvider *translatorsProvider() const;
    QString website() const;
public Q_SLOTS:
    void resetTabs();
private:
    Q_DISABLE_COPY(BAboutDialog)
};

#endif // BABOUTDIALOG_H
