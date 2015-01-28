/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
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

#include "bpersoninfoprovider.h"
#include "bpersoninfoprovider_p.h"

#include "bbase.h"
#include "bbase_p.h"
#include "bcoreapplication.h"
#include "bpersoninfo.h"
#include "bpersoninfolist.h"

#include <QDebug>
#include <QFile>
#include <QList>
#include <QLocale>
#include <QMap>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextStream>

/*============================================================================
================================ BPersonInfoProviderPrivate ==================
============================================================================*/

/*============================== Public constructors =======================*/

BPersonInfoProviderPrivate::BPersonInfoProviderPrivate(BPersonInfoProvider *q) :
    BBasePrivate(q)
{
    //
}

BPersonInfoProviderPrivate::~BPersonInfoProviderPrivate()
{
    //
}

/*============================== Static public methods =====================*/

BPersonInfo BPersonInfoProviderPrivate::infoForLocale(const PersonInfoMap &map, const QString &localeName,
                                                      bool noDefault)
{
    if (localeName.isEmpty())
        return BPersonInfo();
    if (map.contains(localeName))
        return map.value(localeName);
    if (localeName == "en")
        return BPersonInfo();
    QString s = (localeName.length() > 2) ? localeName.left(2) : QString(!noDefault ? "en" : "");
    return infoForLocale(map, s, noDefault);
}

void BPersonInfoProviderPrivate::tryAppendInfo(QList<PersonInfoMap> &where, PersonInfoMap what)
{
    if (what.isEmpty())
        return;
    QStringList keys = what.keys();
    QString defName = what.value("en").name();
    QString defSite = what.value("en").site();
    QString defMail = what.value("en").mail();
    QString defImage = what.value("en").image();
    if (defSite.isEmpty()) {
        foreach (const QString &key, keys) {
            if (!what.value(key).site().isEmpty()) {
                defSite = what.value(key).site();
                break;
            }
        }
    }
    if (defMail.isEmpty()) {
        foreach (const QString &key, keys) {
            if (!what.value(key).mail().isEmpty()) {
                defMail = what.value(key).mail();
                break;
            }
        }
    }
    if (defImage.isEmpty())
    {
        foreach (const QString &key, keys) {
            if (!what.value(key).image().isEmpty()) {
                defImage = what.value(key).image();
                break;
            }
        }
    }
    for (int i = 0; i < keys.size(); ++i) {
        if (what.value(keys.at(i)).name().isEmpty())
            what[keys.at(i)].setName(defName);
        if (what.value(keys.at(i)).site().isEmpty())
            what[keys.at(i)].setSite(defSite);
        if (what.value(keys.at(i)).mail().isEmpty())
            what[keys.at(i)].setMail(defMail);
        if (what.value(keys.at(i)).image().isEmpty())
            what[keys.at(i)].setImage(defImage);
    }
    foreach (const QString &key, what.keys()) {
        if (what.value(key).name().isEmpty())
            what.remove(key);
    }
    if (what.isEmpty())
        return;
    where << what;
}

/*============================== Public methods ============================*/

void BPersonInfoProviderPrivate::init()
{
    //
}

void BPersonInfoProviderPrivate::setFileName(const QString &fn)
{
    QFile f(fn);
    if (!f.open(QFile::ReadOnly))
        return;
    fileName = fn;
    QTextStream in(&f);
    in.setCodec("UTF-8");
    infos.clear();
    PersonInfoMap info;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.isEmpty()) {
            QStringList sl = line.split('=');
            if (sl.size() < 2)
                continue;
            QString id = sl.takeFirst();
            QString val = sl.join("=");
            if (id.isEmpty() || id.length() < 4 || val.isEmpty())
                continue;
            QString ln;
            if (id.at(id.length() - 1) == ']') {
                int i = id.length() - 2;
                while (i >= 0 && id.at(i) != '[')
                    ln.prepend( id.at(i--) );
            }
            if (ln.isEmpty() || (QLocale(ln).name() != ln && QLocale(ln).name().left(2) != ln))
                ln = "en";
            if (!id.left(5).compare("image", Qt::CaseInsensitive)) {
                info[ln].setImage(val);
                continue;
            }
            id = id.left(4);
            if (!id.compare("name", Qt::CaseInsensitive))
                info[ln].setName(val);
            else if (!id.compare("role", Qt::CaseInsensitive))
                info[ln].setRole(val);
            else if (!id.compare("site", Qt::CaseInsensitive))
                info[ln].setSite(val);
            else if (!id.compare("mail", Qt::CaseInsensitive))
                info[ln].setMail(val);
        } else {
            tryAppendInfo(infos, info);
            info.clear();
        }
    }
    tryAppendInfo(infos, info);
    f.close();
    QMetaObject::invokeMethod(q_func(), "reloaded");
}

/*============================================================================
================================ BPersonInfoProvider =========================
============================================================================*/

/*!
\class BPersonInfoProvider
\inmodule BeQtCore
\brief The BPersonInfoProvider class is used to provide information about persons/organizations, typically application
authors, translators, etc.

Information about persons is read from a file and is represented as a BPersonInfoList.

Example (authors.beqt-info):

\snippet src/corelib/bpersoninfoprovider.beqt-info 0

Example (main.cpp):

\snippet src/corelib/bpersoninfoprovider.cpp 0
*/

/*!
\property BPersonInfoProvider::fileName

This property gets and sets the BPersonInfoProvider's information source file name.

The information is automatically reloaded when this property is changed.

By default, this property contains an empty QString.
*/

/*!
\fn BPersonInfoProvider::reloaded()

This signal is emitted immediately after the information is loaded from the source file assigned to the person info
provider.

\sa reload(), setFileName()
*/

/*============================== Public constructors =======================*/

/*!
Constructs a person info provider and sets it's parent to \a parent.
*/

BPersonInfoProvider::BPersonInfoProvider(QObject *parent) :
    QObject(parent), BBase(*new BPersonInfoProviderPrivate(this))
{
    d_func()->init();
}

/*!
Constructs a person info provider and sets it's information source file name to \a fileName, and parent to \a parent.
*/

BPersonInfoProvider::BPersonInfoProvider(const QString &fileName, QObject *parent) :
    QObject(parent), BBase(*new BPersonInfoProviderPrivate(this))
{
    d_func()->init();
    setFileName(fileName);
}

/*!
Destroys the object, deleting the associated data object.
*/

BPersonInfoProvider::~BPersonInfoProvider()
{
    //
}

/*============================== Protected constructors ====================*/

/*!
Constructs an object and associates the given data object \a d with it. Parent is set to \a parent.
*/

BPersonInfoProvider::BPersonInfoProvider(BPersonInfoProviderPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

/*!
Returns the string representation of person info list \a list.

\sa infosString()
*/

QString BPersonInfoProvider::infoListToString(const BPersonInfoList &list)
{
    QString s;
    foreach (const BPersonInfo &info, list) {
        if (!info.isValid())
            continue;
        s += tr("Name:", "info") + " " + info.name() + "\n";
        if (!info.role().isEmpty())
            s += tr("Role:", "info") + " " + info.role() + "\n";
        if (!info.site().isEmpty())
            s += tr("Website:", "info") + " " + info.site() + "\n";
        if (!info.mail().isEmpty())
            s += tr("E-mail:", "info") + " " + info.mail() + "\n";
    }
    if (!s.isEmpty())
        s.remove(s.length() - 1, 1);
    return s;
}

/*!
\overload
Returns the string representation of person info list provided by \a prov. \a locale is used as a target locale.

If \a noDefault is true and there is no info for the \a locale for some person, the person will not be listed.

\sa infoListToString(), infos()
*/

QString BPersonInfoProvider::infosString(const BPersonInfoProvider *prov, bool noDefault, const QLocale &locale)
{
    if (!prov)
        return "";
    return prov->infosString(noDefault, locale);
}

/*!
\overload
Returns the string representation of person info list provided by \a prov. \a locale is used as a target locale.

If \a noDefault is true and there is no info for the \a locale for some person, the person will not be listed.

\sa infoListToString(), infos()
*/

QString BPersonInfoProvider::infosString(const BPersonInfoProvider *prov, const QLocale &locale, bool noDefault)
{
    if (!prov)
        return "";
    return prov->infosString(locale, noDefault);
}

/*============================== Public methods ============================*/

QString BPersonInfoProvider::fileName() const
{
    return d_func()->fileName;
}

/*!
Returns the person info list provided by this object. \a locale is used as a target locale.

If \a noDefault is true and there is no info for the \a locale for some person, the person will not be listed.

\sa infosString()
*/

BPersonInfoList BPersonInfoProvider::infos(bool noDefault, const QLocale &locale) const
{
    return infos(locale.name(), noDefault);
}

/*!
\overload
Returns the person info list provided by this object. \a locale is used as a target locale.

If \a noDefault is true and there is no info for the \a locale for some person, the person will not be listed.

\sa infosString()
*/

BPersonInfoList BPersonInfoProvider::infos(const QLocale &locale, bool noDefault) const
{
    BPersonInfoList list;
    foreach (const BPersonInfoProviderPrivate::PersonInfoMap &map, d_func()->infos) {
        BPersonInfo info = BPersonInfoProviderPrivate::infoForLocale(map, locale.name(), noDefault);
        if (info.isValid())
            list << info;
    }
    return list;
}

/*!
\overload
Returns the string representation of person info list provided by this object. \a locale is used as a target locale.

If \a noDefault is true and there is no info for the \a locale for some person, the person will not be listed.

\sa infoListToString(), infos()
*/

QString BPersonInfoProvider::infosString(bool noDefault, const QLocale &locale) const
{
    return infoListToString(infos(noDefault, locale));
}

/*!
\overload
Returns the string representation of person info list provided by this object. \a locale is used as a target locale.

If \a noDefault is true and there is no info for the \a locale for some person, the person will not be listed.

\sa infoListToString(), infos()
*/

QString BPersonInfoProvider::infosString(const QLocale &locale, bool noDefault) const
{
    return infoListToString(infos(locale, noDefault));
}

void BPersonInfoProvider::setFileName(const QString &fileName)
{
    B_D(BPersonInfoProvider);
#if defined(Q_OS_WIN)
    static const Qt::CaseSensitivity Cs = Qt::CaseInsensitive;
#else
    static const Qt::CaseSensitivity Cs = Qt::CaseSensitive;
#endif
    if (fileName.isEmpty() || !fileName.compare(d->fileName, Cs))
        return;
    d->setFileName(fileName);
}

/*============================== Public slots ==============================*/

/*!
This slot forces the person info provider to read information from the source file associated with it.

May be useful if the associated source file's content is changed and the information must be reloaded.
*/

void BPersonInfoProvider::reload()
{
    B_D(BPersonInfoProvider);
    if (d->fileName.isEmpty())
        return;
    d->setFileName(d->fileName);
}
