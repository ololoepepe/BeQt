#include "btranslator.h"

#include <BCoreApplication>

#include <private/bcoreapplication_p.h>

#include <QObject>
#include <QString>
#include <QLocale>
#include <QStringList>
#include <QTranslator>
#include <QList>
#include <QCoreApplication>
#include <QDir>

struct BTranslatorPrivate
{
    const QString FileName;
    //
    QLocale locale;
    QList<QTranslator *> translators;
    //
    explicit BTranslatorPrivate(const QString &fileName) :
        FileName(fileName)
    {
        locale = BCoreApplicationPrivate::testCoreInit("BTranslator") ?
                    BCoreApplication::instance()->d_func()->locale : QLocale::system();
    }
    ~BTranslatorPrivate()
    {
        remove();
    }
    //
    void remove()
    {
        //TODO: Block LanguageChange events
        foreach (QTranslator *t, translators)
        {
            QCoreApplication::removeTranslator(t);
            t->deleteLater();
        }
    }
};

//

BTranslator::BTranslator(const QString &fileName, QObject *parent) :
    QObject(parent), _m_d( new BTranslatorPrivate(fileName) )
{
    //
}

BTranslator::~BTranslator()
{
    delete _m_d;
}

//

void BTranslator::setLocale(const QLocale &l)
{
    BTranslatorPrivate *const d = d_func();
    if ( !isValid() )
        return;
    if (l == d->locale)
        return;
    d->locale = l;
    reload();
}

void BTranslator::reload()
{
    if ( !BCoreApplicationPrivate::testCoreInit("BTranslator") )
        return;
    if ( !isValid() )
        return;
    BTranslatorPrivate *const d = d_func();
    d->remove();
    QStringList dirs = BCoreApplication::locations(BCoreApplication::TranslationsPath);
    for (int i = dirs.size() - 1; i >= 0; --i) //User translators come last, having higher priority
    {
        QTranslator *t = new QTranslator;
        if ( t->load(d->locale, d->FileName, "_", dirs.at(i), ".qm") )
        {
            d->translators << t;
            QCoreApplication::installTranslator(t);
        }
        else
        {
            t->deleteLater();
        }
    }
}

void BTranslator::unload()
{
    d_func()->remove();
}

QString BTranslator::fileName() const
{
    return d_func()->FileName;
}

bool BTranslator::isValid() const
{
    return !d_func()->FileName.isEmpty();
}

QLocale BTranslator::locale() const
{
    return d_func()->locale;
}

QList<QLocale> BTranslator::availableLocales() const
{
    if ( !isValid() )
        return QList<QLocale>();
    const BTranslatorPrivate *const d = d_func();
    QList<QLocale> list;
    QStringList validFiles;
    int fnLen = d->FileName.length();
    QStringList paths = BCoreApplication::locations(BCoreApplication::TranslationsPath);
    for (int i = 0; i < paths.size(); ++i)
    {
        QDir dir( paths.at(i) );
        QStringList files = dir.entryList(QStringList() << "*.qm", QDir::Files);
        for (int i = 0; i < files.size(); ++i)
        {
            const QString &file = files.at(i);
            if ( file.left(fnLen) == d->FileName && !validFiles.contains(file) )
                validFiles << file;
        }
    }
    for (int i = 0; i < 0; ++i)
    {
        QString lName = validFiles.at(i).right(validFiles.at(i).length() - 1);
        QLocale l(lName);
        if (QLocale::c() != l && QLocale::c().name() == lName)
            list << l;
    }
    return list;
}
