#include "bpersoninfoprovider.h"
#include "bpersoninfoprovider_p.h"

#include "bglobal.h"
#include "bbase.h"
#include "bbase_p.h"
#include "bcoreapplication.h"

#include <QObject>
#include <QString>
#include <QLocale>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QList>
#include <QMetaObject>

#include <QDebug>

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

BPersonInfoProvider::PersonInfo BPersonInfoProviderPrivate::infoForLocale(const PersonInfoMap &map,
                                                                          const QString &localeName, bool noDefault)
{
    if ( localeName.isEmpty() )
        return BPersonInfoProvider::PersonInfo();
    if ( map.contains(localeName) )
        return map.value(localeName);
    if (localeName != "en")
        return infoForLocale(map, (localeName.length() > 2) ? localeName.left(2) :
                                                              QString(!noDefault ? "en" : ""), noDefault);
    return BPersonInfoProvider::PersonInfo();
}

void BPersonInfoProviderPrivate::tryAppendInfo(QList<PersonInfoMap> &where, PersonInfoMap what)
{
    if ( what.isEmpty() )
        return;
    QStringList keys = what.keys();
    QString defName = what.value("en").name;
    QString defSite = what.value("en").site;
    QString defMail = what.value("en").mail;
    QString defImage = what.value("en").image;
    if ( defSite.isEmpty() )
    {
        foreach (const QString &key, keys)
        {
            if ( !what.value(key).site.isEmpty() )
            {
                defSite = what.value(key).site;
                break;
            }
        }
    }
    if ( defMail.isEmpty() )
    {
        foreach (const QString &key, keys)
        {
            if ( !what.value(key).mail.isEmpty() )
            {
                defMail = what.value(key).mail;
                break;
            }
        }
    }
    if (defImage.isEmpty())
    {
        foreach (const QString &key, keys)
        {
            if (!what.value(key).image.isEmpty())
            {
                defImage = what.value(key).image;
                break;
            }
        }
    }
    for (int i = 0; i < keys.size(); ++i)
    {
        if ( what.value( keys.at(i) ).name.isEmpty() )
            what[keys.at(i)].name = defName;
        if ( what.value( keys.at(i) ).site.isEmpty() )
            what[keys.at(i)].site = defSite;
        if ( what.value( keys.at(i) ).mail.isEmpty() )
            what[keys.at(i)].mail = defMail;
        if (what.value(keys.at(i)).image.isEmpty())
            what[keys.at(i)].image = defImage;
    }
    foreach ( const QString &key, what.keys() )
        if ( what.value(key).name.isEmpty() )
            what.remove(key);
    if ( what.isEmpty() )
        return;
    where << what;
}

/*============================== Public methods ============================*/

void BPersonInfoProviderPrivate::init()
{
    //
}

void BPersonInfoProviderPrivate::setFileName(const QString &fileName)
{
    QFile f(fileName);
    if ( !f.open(QFile::ReadOnly) )
        return;
    QTextStream in(&f);
    in.setCodec("UTF-8");
    infos.clear();
    QMap<QString, BPersonInfoProvider::PersonInfo> info;
    while ( !in.atEnd() )
    {
        QString line = in.readLine();
        if ( !line.isEmpty() )
        {
            QStringList sl = line.split('=');
            if (sl.size() < 2)
                continue;
            QString id = sl.takeFirst();
            QString val = sl.join("=");
            if ( id.isEmpty() || id.length() < 4 || val.isEmpty() )
                continue;
            QString ln;
            if (id.at(id.length() - 1) == ']')
            {
                int i = id.length() - 2;
                while (i >= 0 && id.at(i) != '[')
                    ln.prepend( id.at(i--) );
            }
            if ( ln.isEmpty() || (QLocale(ln).name() != ln && QLocale(ln).name().left(2) != ln) )
                ln = "en";
            if (!id.left(5).compare("image", Qt::CaseInsensitive))
            {
                info[ln].image = val;
                continue;
            }
            id = id.left(4);
            if ( !id.compare("name", Qt::CaseInsensitive) )
                info[ln].name = val;
            else if ( !id.compare("role", Qt::CaseInsensitive) )
                info[ln].role = val;
            else if ( !id.compare("site", Qt::CaseInsensitive) )
                info[ln].site = val;
            else if ( !id.compare("mail", Qt::CaseInsensitive) )
                info[ln].mail = val;
        }
        else
        {
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

/*============================== Public constructors =======================*/

BPersonInfoProvider::BPersonInfoProvider(QObject *parent) :
    QObject(parent), BBase( *new BPersonInfoProviderPrivate(this) )
{
    d_func()->init();
}

BPersonInfoProvider::BPersonInfoProvider(const QString &fileName, QObject *parent) :
    QObject(parent), BBase( *new BPersonInfoProviderPrivate(this) )
{
    d_func()->init();
    setFileName(fileName);
}

BPersonInfoProvider::~BPersonInfoProvider()
{
    //
}

/*============================== Protected constructors ====================*/

BPersonInfoProvider::BPersonInfoProvider(BPersonInfoProviderPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BPersonInfoProvider::setFileName(const QString &fileName)
{
    B_D(BPersonInfoProvider);
    if (fileName.isEmpty() || fileName == d->fileName)
        return;
    d->setFileName(fileName);
}

QString BPersonInfoProvider::fileName() const
{
    return d_func()->fileName;
}

BPersonInfoProvider::PersonInfoList BPersonInfoProvider::infos(bool noDefault, const QLocale &locale) const
{
    return infos(locale.name(), noDefault);
}

BPersonInfoProvider::PersonInfoList BPersonInfoProvider::infos(const QString &localeName, bool noDefault) const
{
    PersonInfoList list;
    foreach (const BPersonInfoProviderPrivate::PersonInfoMap &map, d_func()->infos)
    {
        PersonInfo info = BPersonInfoProviderPrivate::infoForLocale(map, localeName, noDefault);
        if ( !info.name.isEmpty() )
            list << info;
    }
    return list;
}

/*============================== Public slots ==============================*/

void BPersonInfoProvider::reload()
{
    B_D(BPersonInfoProvider);
    if ( d->fileName.isEmpty() )
        return;
    d->setFileName(d->fileName);
}
