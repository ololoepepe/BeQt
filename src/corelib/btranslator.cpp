#include "btranslator.h"
#include "btranslator_p.h"

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

BTranslatorPrivate::BTranslatorPrivate(BTranslator *q) :
    _m_q(q)
{
    installed = false;
}

BTranslatorPrivate::~BTranslatorPrivate()
{
    remove();
}

//

void BTranslatorPrivate::install()
{
    if (installed)
        return;
    QLocale l = BCoreApplication::locale();
    QStringList dirs = BCoreApplication::locations(BCoreApplication::TranslationsPath);
    for (int i = dirs.size() - 1; i >= 0; --i) //User translators come last, having higher priority
    {
        QTranslator *t = new QTranslator;
        if ( t->load(l,fileName, "_", dirs.at(i), ".qm") )
        {
            translators << t;
            QCoreApplication::installTranslator(t);
        }
        else
        {
            t->deleteLater();
        }
    }
    installed = true;
}

void BTranslatorPrivate::remove()
{
    if (!installed)
        return;
    foreach (QTranslator *t, translators)
    {
        QCoreApplication::removeTranslator(t);
        t->deleteLater();
    }
    translators.clear();
    installed = false;
}

void BTranslatorPrivate::emitLanguageChange()
{
    if ( !BCoreApplicationPrivate::testCoreInit("BTranslator") )
        return;
    BCoreApplication::instance()->d_func()->emitLanguageChange();
}

//

BTranslator::BTranslator(const QString &fileName, QObject *parent) :
    QObject(parent), _m_d( new BTranslatorPrivate(this) )
{
    d_func()->fileName = fileName;
}

BTranslator::~BTranslator()
{
    delete _m_d;
}

//

void BTranslator::setFileName(const QString &fileName, bool languageChange)
{
    BTranslatorPrivate *const d = d_func();
    bool wasInstalled = isInstalled();
    if (wasInstalled)
        d->remove();
    d->fileName = fileName;
    if ( !isValid() )
        return;
    if (wasInstalled)
        d->install();
    if (languageChange)
        d->emitLanguageChange();
}

bool BTranslator::isValid() const
{
    return !fileName().isEmpty();
}

bool BTranslator::isInstalled() const
{
    return d_func()->installed;
}

QString BTranslator::fileName() const
{
    return d_func()->fileName;
}

QList<QLocale> BTranslator::availableLocales() const
{
    if ( !isValid() )
        return QList<QLocale>();
    const BTranslatorPrivate *const d = d_func();
    QList<QLocale> list;
    QStringList validFiles;
    int fnLen = d->fileName.length();
    QStringList paths = BCoreApplication::locations(BCoreApplication::TranslationsPath);
    for (int i = 0; i < paths.size(); ++i)
    {
        QDir dir( paths.at(i) );
        QStringList files = dir.entryList(QStringList() << "*.qm", QDir::Files);
        for (int i = 0; i < files.size(); ++i)
        {
            const QString &file = files.at(i);
            if ( file.left(fnLen) == d->fileName && !validFiles.contains(file) )
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
