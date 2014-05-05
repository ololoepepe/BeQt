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

class QWidget;
class QIcon;
class QSize;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>
#include <BeQtCore/BPluginInterface>
#include <BeQtCore/BPersonInfo>
#include <BeQtCore/BDirTools>

#include <QDialog>
#include <QList>
#include <QString>
#include <QCoreApplication>

/*============================================================================
================================ BAboutDialog ================================
============================================================================*/

class B_WIDGETS_EXPORT BAboutDialog : public QDialog, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BAboutDialog)
public:
    explicit BAboutDialog(QWidget *parent = 0);
    explicit BAboutDialog( QWidget *parent, const QString &appName, const QString &appVersion = QString() );
    ~BAboutDialog();
protected:
    explicit BAboutDialog(BAboutDialogPrivate &d, QWidget *parent = 0);
public:
    static void setDefaultMinimumSize(const QSize &sz);
    static void setDefaultMinimumSize(int width, int height);
public:
    void setAppName(const QString &name);
    void setAppVersion(const QString &version);
    void setOrganization( const QString &organization, const QString &copyrightPeriod = QString() );
    void setWebsite(const QString &site);
    void setPixmap(const QPixmap &pixmap);
    void setPixmap(const QString &fileName);
    void setDescription(const QString &text);
    void setDescriptionFile(const QString &fileName);
    void setChangeLog(const QString &text);
    void setChangeLogFile(const QString &fileName);
    void setLicense(const QString &text);
    void setLicenseFile(const QString &fileName);
    void setAuthors(const BPersonInfoList &list);
    void setAuthorsFile(const QString &fileName);
    void setAuthorsProvider(BPersonInfoProvider *prov);
    void setTranslators(const BPersonInfoList &list);
    void setTranslatorsFile(const QString &fileName);
    void setTranslatorsProvider(BPersonInfoProvider *prov);
    void setThanksTo(const BPersonInfoList &list);
    void setThanksToFile(const QString &fileName);
    void setThanksToProvider(BPersonInfoProvider *prov);
    void setupWithApplicationData();
    void setAboutQtShown(bool b);
    void setAboutBeqtShown(bool b);
    void addTab(QWidget *tab, const BTranslation &title);
    void removeTab(QWidget *tab);
public Q_SLOTS:
    void resetTabs();
private:
    Q_DISABLE_COPY(BAboutDialog)
};

#endif // BABOUTDIALOG_H
